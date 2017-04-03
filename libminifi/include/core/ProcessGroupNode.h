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
#ifndef LIBMINIFI_INCLUDE_CORE_CONTROLLER_PROCESSGROUPNODE_H_
#define LIBMINIFI_INCLUDE_CORE_CONTROLLER_PROCESSGROUPNODE_H_

#include "core/ProcessGroup.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {

class ProcessGroupNode {
 public:
  /**
   * @return the Process Group that this Controller Service belongs to, or <code>null</code> if the Controller Service
   *         does not belong to any Process Group
   */
  std::shared_ptr<ProcessGroup> &getProcessGroup();

  /**
   * Sets the Process Group for this Controller Service
   *
   * @param group the group that the service belongs to
   */
  void setProcessGroup(std::shared_ptr<ProcessGroup> &processGroup);

 protected:

  std::mutex mutex_;

  std::shared_ptr<ProcessGroup> process_group_;
};

std::shared_ptr<ProcessGroup> &ProcessGroupNode::getProcessGroup() {
  std::lock_guard<std::mutex> lock(mutex_);
  return process_group_;
}

void ProcessGroupNode::setProcessGroup(
    std::shared_ptr<ProcessGroup> &processGroup) {
  std::lock_guard<std::mutex> lock(mutex_);
  process_group_ = processGroup;
}

} /* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_CONTROLLER_PROCESSGROUPNODE_H_ */
