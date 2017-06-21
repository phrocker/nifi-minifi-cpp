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
#ifndef LIBMINIFI_INCLUDE_C2_RESTPROTOCOL_H_
#define LIBMINIFI_INCLUDE_C2_RESTPROTOCOL_H_

#include "json/json.h"
#include "json/writer.h"
#include <string>
#include "../C2Protocol.h"
#include "controllers/SSLContextService.h"
#include "utils/ByteInputCallBack.h"
#include "utils/HTTPUtils.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace c2 {

/**
 *
 *  Currently transaction_id isn't needed.
 *
 * {operation : "heartbeat",
 *  transaction_id : id
 *  content_name : content_value
 * }
 * response
 * {operation : "heartbeat",
 *  transaction_id : id
 *  requests: [  {
 *                  operation : update
 *                  name : config
 *                  update_details : flowconfig
 *                 }
 *
 *   ]
 *
 * }
 *
 * example:
 *
 * {operation : "heartbeat",
 *  transaction_id : id
 *  requests: [  {
 *                  operation : clear
 *                  name: connection
 *                  update_details : connection_X
 *                 }
 *
 *   ]
 *
 * }
 */
class RESTProtocol : public C2Protocol {
 public:
  RESTProtocol(const std::shared_ptr<core::controller::ControllerServiceProvider> &controller, const std::shared_ptr<Configure> &configure);

  virtual C2Payload send(const std::string &url, const C2Payload &payload, Direction direction, bool async);

  virtual C2Payload send(const C2Payload &payload, Direction direction, bool async);

 protected:

  Json::Value serializeJsonPayload(Json::Value &json_root, const C2Payload &payload);

  const C2Payload parseJsonResponse(const C2Payload &payload, const std::string response);

  const C2Payload sendPayload(const std::string url, const Direction direction, const C2Payload &payload, const std::string outputConfig);

  std::string getOperation(const C2Payload &payload);

  Operation stringToOperation(const std::string str);

 private:
  std::string rest_uri_;
  std::shared_ptr<minifi::controllers::SSLContextService> ssl_context_service_;
};

} /* namesapce c2 */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_C2_RESTPROTOCOL_H_ */
