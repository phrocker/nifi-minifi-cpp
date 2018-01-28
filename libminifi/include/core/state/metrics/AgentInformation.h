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
#ifndef LIBMINIFI_INCLUDE_CORE_STATE_METRICS_AGENTINFORMATION_H_
#define LIBMINIFI_INCLUDE_CORE_STATE_METRICS_AGENTINFORMATION_H_

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
#include "../../../agent/agent_version.h"
#include "../../../agent/build_description.h"
#include "core/ClassLoader.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {
namespace metrics {

/**
 * Justification and Purpose: Provides available extensions for the agent information block.
 */
class AvailableFeatures : public DeviceMetric {
 public:

  AvailableFeatures(std::string name, uuid_t uuid)
      : DeviceMetric(name, uuid) {
    setArray(true);
  }

  AvailableFeatures(const std::string &name)
      : DeviceMetric(name, 0) {
    setArray(true);
  }

  std::string getName() const {
    return "Extensions";
  }

  std::vector<MetricResponse> serialize() {
    std::vector<MetricResponse> serialized;

    for (auto group : AgentBuild::getExtensions()) {
      MetricResponse feat;
      feat.name = group;
      serialized.push_back(feat);
    }

    return serialized;
  }
};

/**
 * Purpose and Justification: Prints classes along with their properties for the current agent.
 */
class AvailableClasses : public DeviceMetric {
 public:

  AvailableClasses(std::string name, uuid_t uuid)
      : DeviceMetric(name, uuid) {
    setArray(true);
  }

  AvailableClasses(const std::string &name)
      : DeviceMetric(name, 0) {
    setArray(true);
  }

  std::string getName() const {
    return "AvailableClasses";
  }

  std::vector<MetricResponse> serialize() {
    std::vector<MetricResponse> serialized;

    for (ClassDescription group : BuildDescription::getClassDescriptions()) {

      MetricResponse desc;

      desc.name = "Class";

      MetricResponse feat;
      feat.name = "class_name";
      feat.value = group.class_name_;
      desc.children.push_back(feat);

      if (!group.class_properties_.empty()) {
        MetricResponse props;
        props.name = "properties";
        for (auto && prop : group.class_properties_) {
          MetricResponse child;
          child.name = prop;
          props.children.push_back(child);
        }

        desc.children.push_back(props);
      }

      //serialized.push_back(props);


      MetricResponse dyn_prop;
      dyn_prop.name = "supportsDynamicProperties";
      dyn_prop.value = group.support_dynamic_ ? "true" : "false";

      //serialized.push_back(dyn_prop);

      desc.children.push_back(dyn_prop);

      serialized.push_back(desc);

    }

    return serialized;
  }
};

REGISTER_RESOURCE(AvailableFeatures);
REGISTER_RESOURCE(AvailableClasses);

} /* namespace metrics */
} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_STATE_METRICS_AGENTINFORMATION_H_ */
