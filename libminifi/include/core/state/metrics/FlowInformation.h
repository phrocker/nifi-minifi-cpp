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
#ifndef LIBMINIFI_INCLUDE_CORE_STATE_METRICS_FLOWINFORMATION_H_
#define LIBMINIFI_INCLUDE_CORE_STATE_METRICS_FLOWINFORMATION_H_

#include "core/Resource.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include <sys/ioctl.h>
#if ( defined(__APPLE__) || defined(__MACH__) || defined(BSD)) 
#include <net/if_dl.h>
#include <net/if_types.h>
#endif
#include <ifaddrs.h>
#include <net/if.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <map>
#include "MetricsBase.h"
#include "Connection.h"
#include "io/ClientSocket.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {
namespace metrics {


/**
 * Justification and Purpose: Provides flow version Information
 */
class FlowInformation : public DeviceMetric {
 public:

  FlowInformation(std::string name, uuid_t uuid)
      : DeviceMetric(name, uuid) {
  }

  FlowInformation(const std::string &name)
      : DeviceMetric(name, 0) {
  }

  std::string getName() const{
    return "FlowInformation";
  }

  std::vector<MetricResponse> serialize() {
    std::vector<MetricResponse> serialized;

    MetricResponse fv;
    fv.name = "FlowInformation";
    fv.value = flow_version_;


    serialized.push_back(fv);

    return serialized;
  }

 protected:

  std::string flow_version_;
};

REGISTER_RESOURCE(FlowInformation);

} /* namespace metrics */
} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_STATE_METRICS_FLOWINFORMATION_H_ */
