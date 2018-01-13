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
#ifndef LIBMINIFI_INCLUDE_CONTROLLERS_THREADMANAGEMENTSERVICE_H_
#define LIBMINIFI_INCLUDE_CONTROLLERS_THREADMANAGEMENTSERVICE_H_
#include <iostream>
#include <memory>
#include "core/Resource.h"
#include "utils/StringUtils.h"
#include "io/validation.h"
#include "core/controller/ControllerService.h"
#include "core/logging/LoggerConfiguration.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace controllers {

class ThreadManagementService : public core::controller::ControllerService {
 public:
  explicit ThreadManagementService(const std::string &name, const std::string &id)
      : ControllerService(name, id),
        logger_(logging::LoggerFactory<ThreadManagementService>::getLogger()) {
  }

  explicit ThreadManagementService(const std::string &name, uuid_t uuid = 0)
      : ControllerService(name, uuid),
        logger_(logging::LoggerFactory<ThreadManagementService>::getLogger()) {
  }

  explicit ThreadManagementService(const std::string &name, const std::shared_ptr<Configure> &configuration)
      : ControllerService(name, nullptr),
        logger_(logging::LoggerFactory<ThreadManagementService>::getLogger()) {

  }

  virtual bool isAboveMax(int current_tasks) = 0;

  virtual uint16_t getMaxThreads() = 0;

  virtual bool shouldReduce() = 0;

  virtual void reduce() = 0;

  virtual void registerThreadCount(int threads){
    thread_count_ += threads;
  }

  virtual bool canIncrease() = 0;

  virtual void initialize() {
    ControllerService::initialize();
  }

  void yield() {

  }

  bool isRunning() {
    return getState() == core::controller::ControllerServiceState::ENABLED;
  }

  bool isWorkAvailable() {
    return false;
  }

  virtual void onEnable() {

  }

 protected:

  std::atomic<int> thread_count_;

 private:
  std::shared_ptr<logging::Logger> logger_;
};


} /* namespace controllers */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CONTROLLERS_THREADMANAGEMENTSERVICE_H_ */
