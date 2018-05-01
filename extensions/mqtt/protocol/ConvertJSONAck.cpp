/**
 * @file ConvertAck.cpp
 * ConvertAck class implementation
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
#include "ConvertJSONAck.h"

#include <stdio.h>
#include <algorithm>
#include <memory>
#include <string>
#include <map>
#include <set>
#include "utils/TimeUtil.h"
#include "utils/StringUtils.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"
#include "PayloadSerializer.h"
#include "utils/ByteArrayCallback.h"
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

core::Property ConvertJSONAck::MQTTControllerService("MQTT Controller Service", "Name of controller service that will be used for MQTT interactivity", "");
core::Relationship ConvertJSONAck::Success("success", "All files are routed to success");
void ConvertJSONAck::initialize() {
  // Set the supported properties
  std::set<core::Property> properties;
  properties.insert(MQTTControllerService);
  setSupportedProperties(properties);
  // Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert(Success);
  setSupportedRelationships(relationships);
}

void ConvertJSONAck::onSchedule(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSessionFactory> &sessionFactory) {

  std::string controller_service_name = "";
  if (context->getProperty(MQTTControllerService.getName(), controller_service_name) && !controller_service_name.empty()) {
    auto service = context->getControllerService(controller_service_name);
    mqtt_service_ = std::static_pointer_cast<controllers::MQTTContextService>(service);
  }
  mqtt_service_->subscribeToTopic("heartbeats");
}

std::string ConvertJSONAck::parseTopicName(const std::string &json) {
  std::string topic;
  rapidjson::Document root;

  try {
    rapidjson::ParseResult ok = root.Parse(json.c_str());
    if (ok) {
      if (root.HasMember("agentInfo")) {
        if (root["agentInfo"].HasMember("identifier")) {
          std::stringstream topicStr;
          topicStr << root["agentInfo"]["identifier"].GetString() << "/in";
          return topicStr.str();
        }
      }
    }
  } catch (...) {

  }
  return topic;
}
void ConvertJSONAck::onTrigger(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) {
  if (nullptr == mqtt_service_) {
    context->yield();
    return;
  }
  auto flow = session->get();

  if (!flow) {
    return;
  }

  std::string topic;
  {
    // expect JSON response from InvokeHTTP and thus we expect a heartbeat and then the output from the HTTP
    c2::C2Payload response_payload(c2::Operation::HEARTBEAT, state::UpdateState::READ_COMPLETE, true, true);
    ReadCallback callback;
    session->read(flow, &callback);

    topic = parseTopicName(std::string(callback.buffer_.data(), callback.buffer_.size()));

    session->transfer(flow, Success);

  }
  flow = session->get();

  if (!flow) {
    return;
  }

  ReadCallback callback;
  session->read(flow, &callback);

  c2::C2Payload response_payload(c2::Operation::HEARTBEAT, state::UpdateState::READ_COMPLETE, true, true);

  auto payload = parseJsonResponse(response_payload, callback.buffer_);
  if (!topic.empty()) {
    auto stream = c2::mqtt::PayloadSerializer::serialize(payload);

    mqtt_service_->send(topic, stream->getBuffer(), stream->getSize());

  } else {
    std::cout << "topic name is empty" << std::endl;
  }

  session->transfer(flow, Success);

}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
