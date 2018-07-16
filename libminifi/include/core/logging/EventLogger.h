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
#ifndef __EVENT_LOGGER_H__
#define __EVENT_LOGGER_H__

#include <mutex>
#include <memory>
#include <sstream>
#include <iostream>

#include "Logger.h"
#include "core/repository/EventRepository.h"
#include "spdlog/spdlog.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {
namespace logging {


/**
 * Event logging framework that supports Event repositories to store and handle events.
 *
 */
class EventLogger : public Logger {

 public:

 protected:
  EventLogger(const std::string &name, std::shared_ptr<spdlog::logger> delegate, std::shared_ptr<LoggerControl> controller)
      : Logger(name, delegate, controller), repo_(nullptr) {
    repo_ = core::repository::EventRepositoryFactory::getRepository(LOG_EVENTS);
  }

  EventLogger(std::shared_ptr<spdlog::logger> delegate)
      : Logger(delegate, nullptr) {
  }
 private:
  inline char const* conditional_conversion(const core::Event &evt) {
    // put the event into the repo
    repo_->put(evt, true);
    return evt.to_string().c_str();
  }

  std::shared_ptr<core::repository::EventRepository> repo_;

};

} /* namespace logging */
} /* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
