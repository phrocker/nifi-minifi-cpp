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

#ifndef EXTENSIONS_COAP_SERVER_COAPSERVER_H_
#define EXTENSIONS_COAP_SERVER_COAPSERVER_H_
#include "core/Connectable.h"
#include "coap_server.h"
#include "coap_message.h"
#include <coap2/coap.h>
#include <functional>
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace coap {

enum METHOD {
  GET,
  POST,
  PUT,
  DELETE
};

class CoapQuery {
 public:
  CoapQuery(const std::string &query, CoAPMessage * const message)
      : query_(query),
        message_(message) {

  }
  virtual ~CoapQuery() {
    free_coap_message(message_);
  }
  CoapQuery(const CoapQuery &qry) = delete;
  CoapQuery(CoapQuery &&qry) = default;

 protected:
  std::string query_;
  CoAPMessage * const message_;
};

class CoapServer : public core::Connectable {
 public:
  explicit CoapServer(const std::string &name, const utils::Identifier &uuid)
      : core::Connectable(name, uuid),
        server_(nullptr),
        port_(0) {
    //TODO: this allows this class to be instantiated via the the class loader
    //need to define this capability in the future.
  }
  CoapServer(const std::string &hostname, uint16_t port)
      : core::Connectable(hostname),
        hostname_(hostname),
        server_(nullptr),
        port_(port) {
    auto port_str = std::to_string(port_);
    server_ = create_server(hostname_.c_str(), port_str.c_str());
  }

  virtual ~CoapServer();

  void add_endpoint(const std::string &path, METHOD method, std::function<int(CoapQuery)> functor) {
    unsigned char mthd = 0;
    switch (method) {
      case GET:
        mthd = COAP_REQUEST_GET;
        break;
      case POST:
        mthd = COAP_REQUEST_POST;
        break;
      case PUT:
        mthd = COAP_REQUEST_PUT;
        break;
      case DELETE:
        mthd = COAP_REQUEST_DELETE;
        break;
    }
    CoAPEndpoint * const endpoint = create_endpoint(server_, path.c_str(), mthd, hnd_get_time);
    functions_.insert(std::make_pair(endpoint->resource, functor));
    endpoints_.emplace_back(endpoint);
  }


  /**
   * Determines if we are connected and operating
   */
  virtual bool isRunning() {
    return true;
  }

  /**
   * Block until work is available on any input connection, or the given duration elapses
   * @param timeoutMs timeout in milliseconds
   */
  void waitForWork(uint64_t timeoutMs);

  virtual void yield() {

  }

  /**
   * Determines if work is available by this connectable
   * @return boolean if work is available.
   */
  virtual bool isWorkAvailable() {
    return true;
  }


 protected:

  static void hnd_get_time(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request, coap_binary_t *token, coap_string_t *query, coap_pdu_t *response) {
    std::cout << "get" << std::endl;
    auto fx = functions_.find(resource);
    if (fx != functions_.end()) {
      auto message = create_coap_message(response);
      // call the UDF
      fx->second(CoapQuery(std::string((const char*)query->s, query->length), message));
    }
  }

  std::string hostname_;
  CoAPServer *server_;
  static std::map<coap_resource_t*, std::function<int(CoapQuery)>> functions_;
  std::vector<CoAPEndpoint*> endpoints_;
  uint16_t port_;
};

} /* namespace coap */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* EXTENSIONS_COAP_SERVER_COAPSERVER_H_ */
