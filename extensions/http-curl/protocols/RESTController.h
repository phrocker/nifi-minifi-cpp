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
#ifndef LIBMINIFI_INCLUDE_C2_RESTCONTROLLER_H
#define LIBMINIFI_INCLUDE_C2_RESTCONTROLLER_H

#include "RESTSender.h"
#include "json/json.h"
#include "json/writer.h"
#include <string>
#include <mutex>
#include "core/Resource.h"
#include "RESTProtocol.h"
#include "CivetServer.h"
#include "c2/C2Protocol.h"
#include "controllers/SSLContextService.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace c2 {

int log_message(const struct mg_connection *conn, const char *message);

int ssl_protocol_en(void *ssl_context, void *user_data);

/**
 * Purpose and Justification: Encapsulates the restful protocol that is built upon C2Protocol.
 *
 * The external interfaces rely solely on send, where send includes a Direction. Transmit will perform a POST
 * and RECEIVE will perform a GET. This does not mean we can't receive on a POST; however, since Direction
 * will encompass other protocols the context of its meaning here simply translates into POST and GET respectively.
 *
 */
class RESTController : public RESTProtocol, public HeartBeatReporter {
 public:
  RESTController(std::string name, uuid_t uuid = nullptr);

  virtual void initialize(const std::shared_ptr<core::controller::ControllerServiceProvider> &controller, const std::shared_ptr<state::StateMonitor> &updateSink,
                          const std::shared_ptr<Configure> &configure) override;
  virtual int16_t heartbeat(const C2Payload &heartbeat) override;

 protected:

  class ControllerProtocol : public CivetHandler {

   public:
    ControllerProtocol() {

    }

    bool handleGet(CivetServer *server, struct mg_connection *conn) {
      std::string currentvalue;
      {
        std::lock_guard<std::mutex> lock(reponse_mutex_);
        currentvalue = resp_;
      }

      auto *req_info = mg_get_request_info(conn);

      auto paths_split = utils::StringUtils::split(req_info->request_uri, "/");

      std::cout << req_info->request_uri << std::endl;
      if (paths_split.size() == 2) {
        if (utils::StringUtils::equalsIgnoreCase(paths_split.at(0), "stop")) {
          auto components = update_sink_->getComponents(paths_split.at(1));
          for (auto component : components) {
            component->stop(true, 30000);
          }
        }
        else if (utils::StringUtils::equalsIgnoreCase(paths_split.at(0), "clear")) {

          update_sink_->clearConnection( paths_split.at(1) );
        }

      }

      // base url / operation / command

      std::stringstream output;
      output << "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " << currentvalue.length() << "\r\nConnection: close\r\n\r\n";

      mg_printf(conn, "%s", output.str().c_str());
      mg_printf(conn, "%s", currentvalue.c_str());
      return true;
    }

    void setResponse(std::string response) {
      std::lock_guard<std::mutex> lock(reponse_mutex_);
      resp_ = response;
    }

   protected:
    std::mutex reponse_mutex_;
    std::string resp_;

  };

  std::unique_ptr<CivetServer> start_webserver(const std::string &port, std::string &rooturi, CivetHandler *handler, std::string &ca_cert);

  std::unique_ptr<CivetServer> start_webserver(const std::string &port, std::string &rooturi, CivetHandler *handler);

  std::unique_ptr<CivetServer> listener;
  std::unique_ptr<ControllerProtocol> handler;

 private:
  std::shared_ptr<logging::Logger> logger_;
};

REGISTER_RESOURCE(RESTController);

} /* namesapce c2 */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_C2_RESTCONTROLLER_H */
