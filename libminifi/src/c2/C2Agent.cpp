/**
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "c2/C2Agent.h"
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include "c2/protocols/RESTProtocol.h"
#include "core/state/UpdateController.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace c2 {

C2Agent::C2Agent(const std::shared_ptr<core::controller::ControllerServiceProvider> &controller,
                 const std::shared_ptr<state::StateMonitor> &updateSink,
                 const std::shared_ptr<Configure> &configure)
    : controller_(controller),
      update_sink_(updateSink),
      configuration_(configure),
      logger_(logging::LoggerFactory<C2Agent>::getLogger()) {
  last_run_ = std::chrono::steady_clock::now();

  std::string clazz, heartbeat_period;

  if (!configuration_->get("c2.agent.protocol.class", clazz)) {
    protocol = std::unique_ptr<C2Protocol>(new RESTProtocol(controller, configure));
  }

  if (configuration_->get("c2.agent.heartbeat.period", heartbeat_period)) {
    try {
      heart_beat_period_ = std::stoi(heartbeat_period);
    } catch (const std::invalid_argument &ie) {
      heart_beat_period_ = 3000;
    }
  } else {
    heart_beat_period_ = 3000;
  }
  c2_producer_ = [&]() {
    auto now = std::chrono::steady_clock::now();
    auto time_since = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_run_).count();

    if ( time_since > heart_beat_period_ ) {
      last_run_ = now;
      performHeartBeat();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return state::Update(state::UpdateStatus(state::UpdateState::READ_COMPLETE, false));
  };

  functions_.push_back(c2_producer_);

  c2_consumer_ = [&]() {
    auto now = std::chrono::steady_clock::now();
    if ( queue_mutex.try_lock_until(now + std::chrono::seconds(1)) ) {
      if (responses.size() > 0) {
        const C2Payload payload(std::move(responses.back()));
        responses.pop_back();
        extractPayload(std::move(payload));
      }
      queue_mutex.unlock();
    }
    return state::Update(state::UpdateStatus(state::UpdateState::READ_COMPLETE, false));
  };

  functions_.push_back(c2_consumer_);
}

void C2Agent::performHeartBeat() {
  C2Payload payload(Operation::HEARTBEAT);

  logger_->log_debug("Performing heartbeat");

  {
    std::lock_guard<std::timed_mutex> lock(metrics_mutex_);
    if (metrics_map_.size() > 0) {
      C2Payload metrics(Operation::HEARTBEAT);
      metrics.setLabel("metrics");
      for (auto metric : metrics_map_) {
        serializeMetrics(metrics, metric.first, metric.second->serialize());
      }
      payload.addPayload(std::move(metrics));
    }
  }

  C2ContentResponse state(Operation::HEARTBEAT);
  state.name = "state";
  if (update_sink_->isRunning()) {
    state.content["running"] = "true";
  } else {
    state.content["running"] = "false";
  }
  state.content["uptime"] = std::to_string(update_sink_->getUptime());

  payload.addContent(std::move(state));

  C2Payload && response = protocol->send(payload);

  enqueue_c2_server_response(std::move(response));
}

void C2Agent::serializeMetrics(C2Payload &metric_payload, const std::string &name, const std::vector<state::metrics::MetricResponse> &metrics) {
  for (auto metric : metrics) {
    if (metric.children.size() > 0) {
      C2Payload child_metric_payload(metric_payload.getOperation());
      child_metric_payload.setLabel(name);
      serializeMetrics(child_metric_payload, metric.name, metric.children);
      metric_payload.addPayload(std::move(child_metric_payload));
    } else {
      C2ContentResponse response(metric_payload.getOperation());
      response.name = name;

      response.content[metric.name] = metric.value;

      metric_payload.addContent(std::move(response));
    }
  }
}

void C2Agent::extractPayload(const C2Payload &&resp) {
  if (resp.getStatus().getState() == state::UpdateState::NESTED) {
    const std::vector<C2Payload> &payloads = resp.getNestedPayloads();

    for (const auto &payload : payloads) {
      extractPayload(std::move(payload));
    }
    return;
  }
  switch (resp.getStatus().getState()) {
    case state::UpdateState::INITIATE:
      logger_->log_debug("Received initiation event");
      break;
    case state::UpdateState::READ_COMPLETE:
      logger_->log_debug("Received Ack from Server");
      // we have a heartbeat response.
      for (const auto &server_response : resp.getContent()) {
        handle_c2_server_response(server_response);
      }

      break;

    case state::UpdateState::FULLY_APPLIED:
      logger_->log_debug("Received fully applied event");
      break;
    case state::UpdateState::PARTIALLY_APPLIED:
      logger_->log_debug("Received partially applied event");
      break;
    case state::UpdateState::NOT_APPLIED:
      logger_->log_debug("Received not applied event");
      break;
    case state::UpdateState::SET_ERROR:
      logger_->log_debug("Received error event");
      break;
    case state::UpdateState::READ_ERROR:
      logger_->log_debug("Received error event");
      break;
    case state::UpdateState::NESTED:  // multiple updates embedded into one

    default:
      logger_->log_debug("Received nested event");
      break;
  }
}

void C2Agent::extractPayload(const C2Payload &resp) {
  if (resp.getStatus().getState() == state::UpdateState::NESTED) {
    const std::vector<C2Payload> &payloads = resp.getNestedPayloads();
    for (const auto &payload : payloads) {
      extractPayload(payload);
    }
  }
  switch (resp.getStatus().getState()) {
    case state::UpdateState::READ_COMPLETE:
      // we have a heartbeat response.
      for (const auto &server_response : resp.getContent()) {
        handle_c2_server_response(server_response);
      }
      break;
    default:
      break;
  }
}

void C2Agent::handle_c2_server_response(const C2ContentResponse &resp) {
  switch (resp.op) {
    case Operation::CLEAR:
      // we've been told to clear something
      if (resp.name == "connection") {
        logger_->log_debug("Clearing connection %s", resp.name);
        for (auto connection : resp.content) {
          update_sink_->clearConnection(connection.second);
        }
      } else if (resp.name == "repositories") {
        update_sink_->drainRepositories();
      } else {
        logger_->log_debug("Clearing unknown %s", resp.name);
      }
      break;
    case Operation::UPDATE:
      // we've been told to update something
      if (resp.name == "configuration") {
        auto url = resp.content.find("location");
        if (url != resp.content.end()) {
          // just get the raw data.
          C2Payload payload(Operation::UPDATE, false, true);

          C2Payload &&response = protocol->send(url->second, payload, RECEIVE, false);

          update_sink_->applyUpdate(response.getRawData());
          // send
        } else {
          auto update_text = resp.content.find("configuration_data");
          if (update_text != resp.content.end()) {
            update_sink_->applyUpdate(update_text->second);
          }
        }
      }
      break;
    case Operation::START:
      // start something.
      logger_->log_debug("Starting");
      update_sink_->start();
      break;
    case Operation::STOP:
      logger_->log_debug("Stopping");
      update_sink_->stop(true);
      //
      break;
    default:
      break;
      // do nothing
  }
}

int16_t C2Agent::setMetrics(const std::shared_ptr<state::metrics::Metrics> &metric) {
  auto now = std::chrono::steady_clock::now();
  if (metrics_mutex_.try_lock_until(now + std::chrono::seconds(1))) {
    metrics_map_[metric->getName()] = metric;
    metrics_mutex_.unlock();
    return 0;
  }
  return -1;
}

} /* namespace c2 */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
