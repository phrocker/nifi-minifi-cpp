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

#include "c2/protocols/RESTProtocol.h"
#include <memory>
#include <utility>
#include <map>
#include <string>
#include <vector>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace c2 {

RESTProtocol::RESTProtocol(const std::shared_ptr<core::controller::ControllerServiceProvider> &controller,
                           const std::shared_ptr<Configure> &configure)
    : C2Protocol(controller, configure) {
  // base URL when one is not specified.
  if (nullptr != configure) {
    configure->get("c2.rest.url", rest_uri_);
  }
}
C2Payload RESTProtocol::send(const std::string &url, const C2Payload &payload, Direction direction, bool async) {
  std::string operation_request_str = getOperation(payload);
  std::string outputConfig;
  if (direction == Direction::TRANSMIT) {
    Json::Value json_payload;
    json_payload["operation"] = operation_request_str;
    const std::vector<C2ContentResponse> &content = payload.getContent();
    for (const auto &payload_content : content) {
      if (payload_content.op == payload.getOperation()) {
        Json::Value payload_content_values(Json::arrayValue);
        Json::Value new_content;
        for (auto content : payload_content.content) {
          new_content[content.first] = content.second;
        }
        payload_content_values.append(new_content);
        json_payload[payload_content.name] = payload_content_values;
      }
    }

    for (const auto &nested_payload : payload.getNestedPayloads()) {
      json_payload[nested_payload.getLabel()] = serializeJsonPayload(json_payload, nested_payload);
    }

    Json::StyledWriter writer;
    outputConfig = writer.write(json_payload);
  }
  return std::move(sendPayload(url, direction, payload, outputConfig));
}

Json::Value RESTProtocol::serializeJsonPayload(Json::Value &json_root, const C2Payload &payload) {
  // get the name from the content
  Json::Value json_payload;
  std::map<std::string, std::vector<Json::Value>> children;
  for (const auto &nested_payload : payload.getNestedPayloads()) {
    Json::Value child_payload = serializeJsonPayload(json_payload, nested_payload);
    children[nested_payload.getLabel()].push_back(child_payload);
  }
  for (auto child_vector : children) {
    if (child_vector.second.size() > 1) {
      Json::Value children_json(Json::arrayValue);
      for (auto child : child_vector.second) {
        children_json.append(child);
      }
      json_payload[child_vector.first] = children_json;
    } else {
      if (child_vector.second.size() == 1) {
        json_payload[child_vector.first] = child_vector.second.at(0);
      }
    }
  }

  const std::vector<C2ContentResponse> &content = payload.getContent();
  for (const auto &payload_content : content) {
    if (payload_content.op == payload.getOperation()) {
      Json::Value payload_content_values(Json::arrayValue);
      Json::Value new_content;
      for (auto content : payload_content.content) {
        new_content[content.first] = content.second;
      }
      payload_content_values.append(new_content);
      json_payload[payload_content.name] = payload_content_values;
    }
  }
  return json_payload;
}

C2Payload RESTProtocol::send(const C2Payload &payload, Direction direction, bool async) {
  return send(rest_uri_, payload, direction, async);
}

const C2Payload RESTProtocol::parseJsonResponse(const C2Payload &payload, const std::string response) {
  Json::Reader reader;
  Json::Value root;
  try {
    if (reader.parse(response, root)) {
      std::string requested_operation = getOperation(payload);
      if (root["operation"].asString() == requested_operation) {
        if (root["requests"].size() == 0) {
          return std::move(C2Payload(payload.getOperation(), state::UpdateState::READ_COMPLETE, true));
        }
        C2Payload new_payload(payload.getOperation(), state::UpdateState::NESTED, true);

        for (const Json::Value& request : root["requests"]) {
          Operation newOp = stringToOperation(request["operation"].asString());
          C2Payload nested_payload(newOp, state::UpdateState::READ_COMPLETE, true);
          C2ContentResponse new_command(newOp);
          new_command.delay = 0;
          new_command.required = true;
          new_command.ttl = -1;
          new_command.name = request["name"].asString();

          if (request["content"].size() > 0) {
            for (const auto &content : request["content"]) {
              for (const auto &name : content.getMemberNames()) {
                new_command.content[name] = content[name].asString();
              }
            }
            nested_payload.addContent(std::move(new_command));
          }
          new_payload.addPayload(std::move(nested_payload));
        }
        // we have a response for this request
        return std::move(new_payload);
      }
    }
  } catch (...) {
  }

  return std::move(C2Payload(payload.getOperation(), state::UpdateState::READ_ERROR, true));
}

const C2Payload RESTProtocol::sendPayload(const std::string url, const Direction direction, const C2Payload &payload, const std::string outputConfig) {
  utils::HTTPClient client(url, ssl_context_service_);

  utils::ByteInputCallBack *input = new utils::ByteInputCallBack();
  input->write(outputConfig);
  utils::HTTPUploadCallback *callback = new utils::HTTPUploadCallback();
  callback->ptr = input;
  callback->pos = 0;
  if (direction == Direction::TRANSMIT) {
    client.set_request_method("POST");
    client.setUploadCallback(callback);
  } else {
    // we do not need to set the uplaod callback
    // since we are not uploading anything on a get
    client.set_request_method("GET");
  }

  if (client.submit() == CURLE_OK && client.getResponseCode() == 200) {
    if (payload.isRaw()) {
      C2Payload response_payload(payload.getOperation(), state::UpdateState::READ_COMPLETE, true, true);
      response_payload.setRawData(client.getResponseBody());
      return std::move(response_payload);
    }
    return parseJsonResponse(payload, client.getResponseBody());
  } else {
    return C2Payload(payload.getOperation(), state::UpdateState::READ_ERROR, true);
  }
}

std::string RESTProtocol::getOperation(const C2Payload &payload) {
  switch (payload.getOperation()) {
    case Operation::HEARTBEAT:
      return "heartbeat";
    case Operation::STOP:
      return "stop";
    case Operation::START:
      return "start";
    case Operation::UPDATE:
      return "update";
    default:
      return "heartbeat";
  }
}

Operation RESTProtocol::stringToOperation(const std::string str) {
  if (str == "heartbeat") {
    return Operation::HEARTBEAT;
  } else if (str == "update") {
    return Operation::UPDATE;
  } else if (str == "clear") {
    return Operation::CLEAR;
  } else if (str == "stop") {
    return Operation::STOP;
  } else if (str == "start") {
    return Operation::START;
  }
  return Operation::HEARTBEAT;
}

} /* namespace c2 */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
