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
#include "controllers/UpdatePolicyControllerService.h"
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

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace controllers {

core::Property UpdatePolicyControllerService::EnableAllProperties("Network Controllers", "Network controllers in order of priority for this prioritizer");
core::Property UpdatePolicyControllerService::AllowedProperties("Max Throughput", "Max throughput for these network controllers");

void UpdatePolicyControllerService::initialize() {
  std::set<core::Property> supportedProperties;

  setSupportedProperties(supportedProperties);
}

void UpdatePolicyControllerService::yield() {
}

bool UpdatePolicyControllerService::isRunning() {
  return getState() == core::controller::ControllerServiceState::ENABLED;
}

bool UpdatePolicyControllerService::isWorkAvailable() {
  return false;
}

void UpdatePolicyControllerService::onEnable() {


}
} /* namespace controllers */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
