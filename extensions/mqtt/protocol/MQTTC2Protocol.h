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
#ifndef EXTENSIONS_MQTT_PROTOCOL_MQTTC2PROTOCOL_H_
#define EXTENSIONS_MQTT_PROTOCOL_MQTTC2PROTOCOL_H_

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <map>
#include <string>
#include <vector>
#include "c2/C2Protocol.h"
#include "io/BaseStream.h"
#include "agent/agent_version.h"
#include "MQTTContextService.h"
#include "PayloadSerializer.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace c2 {

class MQTTC2Protocol : public C2Protocol {
 public:
  explicit MQTTC2Protocol(std::string name, uuid_t uuid = nullptr);

  virtual ~MQTTC2Protocol();

  virtual C2Payload consumePayload(const std::string &url, const C2Payload &payload, Direction direction, bool async) override {

    if (direction == Direction::TRANSMIT) {
      return serialize(payload);
//      outputConfig = serializeJsonRootPayload(payload);

    }

    return C2Payload(payload.getOperation(), state::UpdateState::READ_ERROR, true);
  }

  virtual C2Payload consumePayload(const C2Payload &payload, Direction direction, bool async) override {
    if (direction == Direction::TRANSMIT) {
      std::cout << "oh hi" << std::endl;
      return serialize(payload);
//      outputConfig = serializeJsonRootPayload(payload);

    }

    return C2Payload(payload.getOperation(), state::UpdateState::READ_ERROR, true);
  }

  virtual void update(const std::shared_ptr<Configure> &configure) override {

  }

  virtual void initialize(const std::shared_ptr<core::controller::ControllerServiceProvider> &controller, const std::shared_ptr<Configure> &configure) override {
    if (configure->get("nifi.c2.mqtt.connector.service", controller_service_name_)) {
      std::cout << "service name is " << controller_service_name_ << std::endl;
      auto service = controller->getControllerService(controller_service_name_);
      mqtt_service_ = std::static_pointer_cast<controllers::MQTTContextService>(service);
    } else
      mqtt_service_ = nullptr;

    std::string baseTopic = configure->getAgentIdentifier();
    std::cout << "base topic is " << baseTopic << std::endl;

    std::stringstream outputStream;
    //outputStream << baseTopic << "/out";
    out_topic_ = "heartbeats";  // outputStream.str();

    std::stringstream inputStream;
    inputStream << baseTopic << "/in";
    in_topic_ = inputStream.str();

    if (mqtt_service_) {
      mqtt_service_->subscribeToTopic(in_topic_);
    }
  }

 protected:

  C2Payload serialize(const C2Payload &payload) {

    if (mqtt_service_ == nullptr || !mqtt_service_->isRunning()) {
      std::cout << "mqtt service not running" << std::endl;
      return C2Payload(payload.getOperation(), state::UpdateState::READ_ERROR, true);
    }

    auto stream = c2::mqtt::PayloadSerializer::serialize(payload);

    std::cout << "oh hi transmit " << out_topic_ << " " << stream->getSize() << std::endl;
    auto transmit_id = mqtt_service_->send(out_topic_, stream->getBuffer(), stream->getSize());

    std::vector<uint8_t> response;
    if (transmit_id > 0 && mqtt_service_->awaitResponse(5000, transmit_id, in_topic_, response)) {

      std::cout << "response is " << response.size() << std::endl;
      return C2Payload(payload.getOperation(), state::UpdateState::READ_COMPLETE, true);
    }
    return C2Payload(payload.getOperation(), state::UpdateState::READ_ERROR, true);
  }

  std::string in_topic_;
  std::string out_topic_;

  std::shared_ptr<controllers::MQTTContextService> mqtt_service_;
  std::shared_ptr<logging::Logger> logger_;
  std::string controller_service_name_;

};
} /* namespace c2 */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
#endif /* EXTENSIONS_MQTT_PROTOCOL_MQTTC2PROTOCOL_H_ */
