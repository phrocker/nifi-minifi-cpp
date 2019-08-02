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

#include "core/state/nodes/RepositoryMetrics.h"
#include <memory>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {
namespace response {


RepositoryMetrics::RepositoryMetrics(const std::string &name, utils::Identifier &uuid)
    : ResponseNode(name, uuid) {
}

RepositoryMetrics::RepositoryMetrics(const std::string &name)
    : ResponseNode(name) {
}

RepositoryMetrics::RepositoryMetrics()
    : ResponseNode("RepositoryMetrics") {
}

std::string RepositoryMetrics::getName() const {
  return "RepositoryMetrics";
}

void RepositoryMetrics::addRepository(const std::shared_ptr<core::Repository> &repo) {
  if (nullptr != repo) {
    repositories.insert(std::make_pair(repo->getName(), repo));
  }
}

std::vector<SerializedResponseNode> RepositoryMetrics::serialize() {
  std::vector<SerializedResponseNode> serialized;
  for (auto conn : repositories) {
    auto repo = conn.second;
    SerializedResponseNode parent;
    parent.name = repo->getName();
    SerializedResponseNode datasize;
    datasize.name = "running";
    datasize.value = repo->isRunning();

    SerializedResponseNode isfull;
    isfull.name = "full";
    isfull.value = repo->isFull();

    SerializedResponseNode datasizemax;
    datasizemax.name = "sizeMax";
    datasizemax.value = repo->getRepoMaxSize();

    SerializedResponseNode queuesize;
    queuesize.name = "size";
    queuesize.value = repo->getRepoSize();



    parent.children.push_back(datasize);
    parent.children.push_back(datasizemax);
    parent.children.push_back(isfull);
    parent.children.push_back(queuesize);

    serialized.push_back(parent);
  }
  return serialized;
}

} /* namespace response */
} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
