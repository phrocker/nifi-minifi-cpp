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
#include "controllers/FailurePolicyControllerService.h"
#include <cstdio>
#include <utility>
#include <limits>
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <set>
#include "utils/StringUtils.h"
#if ( defined(__APPLE__) || defined(__MACH__) || defined(BSD))
#include <net/if_dl.h>
#include <net/if_types.h>
#endif
#include "core/state/FailurePolicy.h"
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace controllers {

core::Property FailurePolicyControllerService::FailurePolicy("Failure Policy", "Defines the failure policy for this controller service instance", "FAIL");
core::Property FailurePolicyControllerService::Components("Components", "Name of components that adhere to this failure policy");

void FailurePolicyControllerService::initialize() {
  std::set<core::Property> supportedProperties;
  supportedProperties.insert(FailurePolicy);
  supportedProperties.insert(Components);
  setSupportedProperties(supportedProperties);
}

void FailurePolicyControllerService::yield() {
}

bool FailurePolicyControllerService::isRunning() {
  return getState() == core::controller::ControllerServiceState::ENABLED;
}

bool FailurePolicyControllerService::isWorkAvailable() {
  return false;
}

void FailurePolicyControllerService::onEnable() {
  std::string policy_type;

  if (getProperty(FailurePolicy.getName(), policy_type)) {
    policy_ = state::from_string(policy_type);
  }

  core::Property comps("Components", "Name of components that adhere to this failure policy");

  if (getProperty(comps.getName(), comps)) {
    components_ = comps.getValues();
  }
}
} /* namespace controllers */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
