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
#ifndef LIBMINIFI_INCLUDE_CONTROLLERS_FailurePolicyControllerService_H_
#define LIBMINIFI_INCLUDE_CONTROLLERS_FailurePolicyControllerService_H_

#include <iostream>
#include <memory>
#include <limits>
#include "core/Resource.h"
#include "utils/StringUtils.h"
#include "io/validation.h"
#include "core/controller/ControllerService.h"
#include "core/logging/LoggerConfiguration.h"
#include "core/state/FailurePolicy.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace controllers {

/**
 * Purpose: FailurePolicyControllerService allows a flow specific policy on allowing or disallowing updates.
 * Since the flow dictates the purpose of a device it will also be used to dictate updates to specific components.
 */
class FailurePolicyControllerService : public core::controller::ControllerService, public std::enable_shared_from_this<FailurePolicyControllerService> {
 public:
  explicit FailurePolicyControllerService(const std::string &name, const std::string &id)
      : ControllerService(name, id),
        policy_(state::FAILURE_POLICY::FAIL),
        logger_(logging::LoggerFactory<FailurePolicyControllerService>::getLogger()) {
  }

  explicit FailurePolicyControllerService(const std::string &name, uuid_t uuid = 0)
      : ControllerService(name, uuid),
        policy_(state::FAILURE_POLICY::FAIL),
        logger_(logging::LoggerFactory<FailurePolicyControllerService>::getLogger()) {
  }

  explicit FailurePolicyControllerService(const std::string &name, const std::shared_ptr<Configure> &configuration)
      : FailurePolicyControllerService(name, nullptr) {
    setConfiguration(configuration);
    initialize();
  }

  static core::Property FailurePolicy;
  static core::Property Components;

  void initialize();

  virtual void onEnable();

  state::FAILURE_POLICY getPolicy() const {
    return policy_.load();
  }

  void update(const state::FAILURE_POLICY policy) {
    policy_ = policy;
  }

  void update(const std::string policyStr) {
    policy_ = state::from_string(policyStr);
  }

  void yield();

   bool isRunning();

   bool isWorkAvailable();


 private:

  std::atomic<state::FAILURE_POLICY> policy_;

  std::vector<std::string> components_;

  std::shared_ptr<logging::Logger> logger_;
};

REGISTER_RESOURCE(FailurePolicyControllerService);

} /* namespace controllers */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CONTROLLERS_FailurePolicyControllerService_H_ */
