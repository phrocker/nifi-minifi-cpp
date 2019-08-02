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

#include "core/state/nodes/FlowMonitor.h"
#include <memory>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {
namespace response {

FlowMonitor::FlowMonitor(const std::string &name, utils::Identifier &uuid)
    : StateMonitorNode(name, uuid) {
}

FlowMonitor::FlowMonitor(const std::string &name)
    : StateMonitorNode(name) {
}

void FlowMonitor::addConnection(const std::shared_ptr<minifi::Connection> &connection) {
  if (nullptr != connection) {
    connections_.insert(std::make_pair(connection->getUUIDStr(), connection));
  }
}

void FlowMonitor::setFlowVersion(const std::shared_ptr<state::response::FlowVersion> &flow_version) {
  flow_version_ = flow_version;
}

} /* namespace response */
} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
