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

#include "capi/C2CallbackAgent.h"
#include <unistd.h>
#include <csignal>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "c2/ControllerSocketProtocol.h"
#include "core/state/UpdateController.h"
#include "core/logging/Logger.h"
#include "core/logging/LoggerConfiguration.h"
#include "utils/file/FileUtils.h"
#include "utils/file/FileManager.h"
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace c2 {

C2CallbackAgent::C2CallbackAgent(const std::shared_ptr<core::controller::ControllerServiceProvider> &controller, const std::shared_ptr<state::StateMonitor> &updateSink,
                                 const std::shared_ptr<Configure> &configuration)
    : C2Agent(controller, updateSink, configuration),
      stop(nullptr),
      logger_(logging::LoggerFactory<C2CallbackAgent>::getLogger()) {
}

void C2CallbackAgent::handle_c2_server_response(const C2ContentResponse &resp) {
  switch (resp.op) {
    case Operation::CLEAR:
      // we've been told to clear something
      if (resp.name == "connection") {
        for (auto connection : resp.operation_arguments) {
          logger_->log_debug("Clearing connection %s", connection.second.to_string());
          update_sink_->clearConnection(connection.second.to_string());
        }
        C2Payload response(Operation::ACKNOWLEDGE, resp.ident, false, true);
        enqueue_c2_response(std::move(response));
      } else if (resp.name == "repositories") {
        update_sink_->drainRepositories();
        C2Payload response(Operation::ACKNOWLEDGE, resp.ident, false, true);
        enqueue_c2_response(std::move(response));
      } else {
        logger_->log_debug("Clearing unknown %s", resp.name);
      }

      break;
    case Operation::UPDATE: {
      handle_update(resp);
      C2Payload response(Operation::ACKNOWLEDGE, resp.ident, false, true);
      enqueue_c2_response(std::move(response));
    }
      break;

    case Operation::DESCRIBE:
      handle_describe(resp);
      break;
    case Operation::RESTART: {
      update_sink_->stop(true);
      C2Payload response(Operation::ACKNOWLEDGE, resp.ident, false, true);
      protocol_.load()->consumePayload(std::move(response));
      exit(1);
    }
      break;
    case Operation::START:
    case Operation::STOP: {
      if (resp.name == "C2" || resp.name == "c2") {
        raise(SIGTERM);
      }

      auto str = resp.name.c_str();

      if (nullptr != stop)
      stop(const_cast<char*>(str));

      break;
    }
      //
      break;
    default:
      break;
      // do nothing
  }
}

} /* namespace c2 */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
