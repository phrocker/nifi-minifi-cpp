/**
 * @file ConvertHeartBeat.cpp
 * ConvertHeartBeat class implementation
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
#include "ConvertHeartBeat.h"
#include "PayloadSerializer.h"
#include "utils/ByteArrayCallback.h"
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

core::Property ConvertHeartBeat::MQTTControllerService("MQTT Controller Service", "Name of controller service that will be used for MQTT interactivity", "");
core::Relationship ConvertHeartBeat::Success("success", "All files are routed to success");
void ConvertHeartBeat::initialize() {
  // Set the supported properties
  std::set<core::Property> properties;
  properties.insert(MQTTControllerService);
  setSupportedProperties(properties);
  // Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert(Success);
  setSupportedRelationships(relationships);
}

void ConvertHeartBeat::onSchedule(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSessionFactory> &sessionFactory) {

  std::string controller_service_name = "";
  if (context->getProperty(MQTTControllerService.getName(), controller_service_name) && !controller_service_name.empty()) {
    auto service = context->getControllerService(controller_service_name);
    mqtt_service_ = std::static_pointer_cast<controllers::MQTTContextService>(service);
  }
  mqtt_service_->subscribeToTopic("heartbeats");
}

void ConvertHeartBeat::onTrigger(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) {
  if (nullptr == mqtt_service_) {
    context->yield();
    return;
  }
  std::vector<uint8_t> heartbeat;
  bool received_heartbeat = false;
  while (mqtt_service_->get(100, "heartbeats", heartbeat)) {
    received_heartbeat = true;
    if (heartbeat.size() > 0) {
      c2::C2Payload &&payload = c2::mqtt::PayloadSerializer::deserialize(heartbeat);
      auto serialized = serializeJsonRootPayload(payload);
      logger_->log_debug("Converted JSON output %s", serialized);
      minifi::utils::StreamOutputCallback byteCallback(serialized.size()+1);
      byteCallback.write( const_cast<char*>(serialized.c_str()),serialized.size() );
      std::cout << "sizse of callback is " << byteCallback.getSize() << std::endl;
      std::cout << "sizse of serialized is " << serialized.size() << std::endl;
      auto newff = session->create();
      session->write(newff,&byteCallback);

      session->transfer(newff,Success);
      received_heartbeat = true;
    } else
      break;
    // convert the heartbeat to JSON

    //serializeJsonRootPayload()
  }

  std::cout << "yielding" << std::endl;
  if (!received_heartbeat) {
    context->yield();
  }

}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
