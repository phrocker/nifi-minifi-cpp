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

#include "core/state/nodes/FlowInformation.h"
#include <memory>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {
namespace response {

FlowInformation::FlowInformation(const std::string &name, utils::Identifier &uuid)
     : FlowMonitor(name, uuid) {
 }

FlowInformation::FlowInformation(const std::string &name)
     : FlowMonitor(name) {
 }

 std::string FlowInformation::getName() const {
   return "flowInfo";
 }

 std::vector<SerializedResponseNode> FlowInformation::serialize() {
   std::vector<SerializedResponseNode> serialized;

       SerializedResponseNode fv;
       fv.name = "flowId";
       fv.value = flow_version_->getFlowId();

       SerializedResponseNode uri;
       uri.name = "versionedFlowSnapshotURI";
       for (auto &entry : flow_version_->serialize()) {
         uri.children.push_back(entry);
       }

       serialized.push_back(fv);
       serialized.push_back(uri);

       if (!connections_.empty()) {
         SerializedResponseNode queues(false);
         queues.name = "queues";

         for (auto &queue : connections_) {
           SerializedResponseNode repoNode(false);
           repoNode.name = queue.second->getUUIDStr();

           SerializedResponseNode queueUUIDNode;
           queueUUIDNode.name = "name";
           queueUUIDNode.value = queue.second->getName();

           SerializedResponseNode queuesize;
           queuesize.name = "size";
           queuesize.value = queue.second->getQueueSize();

           SerializedResponseNode queuesizemax;
           queuesizemax.name = "sizeMax";
           queuesizemax.value = queue.second->getMaxQueueSize();

           SerializedResponseNode datasize;
           datasize.name = "dataSize";
           datasize.value = queue.second->getQueueDataSize();
           SerializedResponseNode datasizemax;

           datasizemax.name = "dataSizeMax";
           datasizemax.value = queue.second->getMaxQueueDataSize();

           repoNode.children.push_back(queuesize);
           repoNode.children.push_back(queuesizemax);
           repoNode.children.push_back(datasize);
           repoNode.children.push_back(datasizemax);
           repoNode.children.push_back(queueUUIDNode);

           queues.children.push_back(repoNode);

         }
         serialized.push_back(queues);
       }

       if (nullptr != monitor_) {
         auto components = monitor_->getAllComponents();
         SerializedResponseNode componentsNode(false);
         componentsNode.name = "components";

         for (auto component : components) {
           SerializedResponseNode componentNode(false);
           componentNode.name = component->getComponentName();

           SerializedResponseNode uuidNode;
           uuidNode.name = "uuid";
           uuidNode.value = component->getComponentUUID();

           SerializedResponseNode componentStatusNode;
           componentStatusNode.name = "running";
           componentStatusNode.value = component->isRunning();

           componentNode.children.push_back(componentStatusNode);
           componentNode.children.push_back(uuidNode);
           componentsNode.children.push_back(componentNode);
         }
         serialized.push_back(componentsNode);
       }

       return serialized;
 }

 } /* namespace response */
} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
