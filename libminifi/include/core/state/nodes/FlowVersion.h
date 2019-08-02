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
#ifndef LIBMINIFI_INCLUDE_CORE_STATE_NODES_FLOWVERSION_H_
#define LIBMINIFI_INCLUDE_CORE_STATE_NODES_FLOWVERSION_H_

#include "core/ConfigurableComponent.h"
#include "core/Processor.h"
#include "core/Resource.h"
#include <functional>
#if ( defined(__APPLE__) || defined(__MACH__) || defined(BSD)) 
#include <net/if_dl.h>
#include <net/if_types.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <map>
#include "../nodes/MetricsBase.h"
#include "Connection.h"
#include "io/ClientSocket.h"
#include "../nodes/StateMonitor.h"
#include "../FlowIdentifier.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {
namespace response {

class FlowVersion : public DeviceInformation {
 public:

  FlowVersion();

  explicit FlowVersion(const std::string &registry_url, const std::string &bucket_id, const std::string &flow_id);

  std::string getName() const ;

  virtual std::shared_ptr<state::FlowIdentifier> getFlowIdentifier() const ;
  /**
   * In most cases the lock guard isn't necessary for these getters; however,
   * we don't want to cause issues if the FlowVersion object is ever used in a way
   * that breaks the current paradigm.
   */
  std::string getRegistryUrl() const ;

  std::string getBucketId() const ;

  std::string getFlowId() const ;

  void setFlowVersion(const std::string &url, const std::string &bucket_id, const std::string &flow_id);

  std::vector<SerializedResponseNode> serialize();

  //FlowVersion &operator=(const FlowVersion &&fv);

 protected:

  mutable std::mutex guard;

  std::shared_ptr<FlowIdentifier> identifier;
};


} /* namespace response */
} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_STATE_NODES_FLOWVERSION_H_ */
