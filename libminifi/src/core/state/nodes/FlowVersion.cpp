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

#include "core/state/nodes/FlowVersion.h"
#include <memory>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {
namespace response {

FlowVersion::FlowVersion()
    : DeviceInformation("FlowVersion") {
  setFlowVersion("", "", getUUIDStr());
}

FlowVersion::FlowVersion(const std::string &registry_url, const std::string &bucket_id, const std::string &flow_id)
    : DeviceInformation("FlowVersion") {
  setFlowVersion(registry_url, bucket_id, flow_id.empty() ? getUUIDStr() : flow_id);
}

std::string FlowVersion::getName() const {
  return "FlowVersion";
}

std::shared_ptr<state::FlowIdentifier> FlowVersion::getFlowIdentifier() const {
  std::lock_guard<std::mutex> lock(guard);
  return identifier;
}
/**
 * In most cases the lock guard isn't necessary for these getters; however,
 * we don't want to cause issues if the FlowVersion object is ever used in a way
 * that breaks the current paradigm.
 */
std::string FlowVersion::getRegistryUrl() const {
  std::lock_guard<std::mutex> lock(guard);
  return identifier->getRegistryUrl();
}

std::string FlowVersion::getBucketId() const {
  std::lock_guard<std::mutex> lock(guard);
  return identifier->getBucketId();
}

std::string FlowVersion::getFlowId() const {
  std::lock_guard<std::mutex> lock(guard);
  return identifier->getFlowId();
}

void FlowVersion::setFlowVersion(const std::string &url, const std::string &bucket_id, const std::string &flow_id) {
  std::lock_guard<std::mutex> lock(guard);
  identifier = std::make_shared<FlowIdentifier>(url, bucket_id, flow_id);
}

std::vector<SerializedResponseNode> FlowVersion::serialize() {
  std::lock_guard<std::mutex> lock(guard);
  std::vector<SerializedResponseNode> serialized;
  SerializedResponseNode ru;
  ru.name = "registryUrl";
  ru.value = identifier->getRegistryUrl();

  SerializedResponseNode bucketid;
  bucketid.name = "bucketId";
  bucketid.value = identifier->getBucketId();

  SerializedResponseNode flowId;
  flowId.name = "flowId";
  flowId.value = identifier->getFlowId();

  serialized.push_back(ru);
  serialized.push_back(bucketid);
  serialized.push_back(flowId);
  return serialized;
}

} /* namespace response */
} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
