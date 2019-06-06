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

#include "core/state/nodes/AgentStatus.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {
namespace response {

AgentStatus::AgentStatus(std::string name, utils::Identifier & uuid)
    : StateMonitorNode(name, uuid) {
}

AgentStatus::AgentStatus(const std::string &name)
    : StateMonitorNode(name) {
}

std::string AgentStatus::getName() const {
  return "status";
}

void AgentStatus::setRepositories(const std::map<std::string, std::shared_ptr<core::Repository>> &repositories) {
  repositories_ = repositories;
}

std::vector<SerializedResponseNode> AgentStatus::serialize() {
  std::vector<SerializedResponseNode> serialized;

  SerializedResponseNode uptime;

  uptime.name = "uptime";
  if (nullptr != monitor_) {
    uptime.value = monitor_->getUptime();
  } else {
    uptime.value = "0";
  }

  if (!repositories_.empty()) {
    SerializedResponseNode repositories;

    repositories.name = "repositories";

    for (auto &repoMapping : repositories_) {
      SerializedResponseNode repoNode;
      repoNode.collapsible = false;
      repoNode.name = repoMapping.first;

      auto repo = repoMapping.second;

      SerializedResponseNode datasize;
      datasize.name = "running";
      datasize.value = repo->isRunning();

      SerializedResponseNode datasizemax;
      datasizemax.name = "full";
      datasizemax.value = repo->isFull();

      SerializedResponseNode queuesize;
      queuesize.name = "size";
      queuesize.value = repo->getRepoSize();

      SerializedResponseNode maxSize;
      maxSize.name = "maxSize";
      maxSize.value = repo->getInitialMaxSize();

      SerializedResponseNode averageTimeInRepo;
      averageTimeInRepo.name = "averageMillisInRepo";
      averageTimeInRepo.value = repo->getAverageTimeInRepo();

      SerializedResponseNode throughput;
      throughput.name = "throughputPerSecond";
      throughput.value = repo->getThroughput();

      repoNode.children.emplace_back(datasize);
      repoNode.children.emplace_back(datasizemax);
      repoNode.children.emplace_back(queuesize);
      repoNode.children.emplace_back(maxSize);
      repoNode.children.emplace_back(averageTimeInRepo);
      repoNode.children.emplace_back(throughput);

      repositories.children.push_back(repoNode);
    }
    serialized.push_back(repositories);
  }

  serialized.push_back(uptime);

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
