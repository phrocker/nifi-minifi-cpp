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
#include "CoapServer.h"
#include <coap2/utlist.h>
#include <coap2/coap.h>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace coap {



void hnd_get_time(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request, coap_binary_t *token, coap_string_t *query, coap_pdu_t *response) {
    std::cout << "got it " << std::endl;
    response->tid = coap_new_message_id(session);

    coap_add_data(response, 4, (unsigned char *) "done");

    /*std::cout << "get" << std::endl;
    auto fx = functions_.find(resource);
    if (fx != functions_.end()) {
      auto message = create_coap_message(response);
      // call the UDF
      fx->second(CoapQuery(std::string((const char*) query->s, query->length), message));
    }*/
    response->code = COAP_RESPONSE_CODE(205);
    if (coap_send(session, response) == COAP_INVALID_TID) {
      printf("error while returning response");
    }

  }

std::map<coap_resource_t*, std::function<int(CoapQuery)>> CoapServer::functions_;
CoapServer::~CoapServer() {
  running_ = false;
  future.get();
  if(server_){
    free_server(server_);
  }
}


} /* namespace coap */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
