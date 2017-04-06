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
#ifndef LIBMINIFI_INCLUDE_CORE_CONTROLLER_STANDARDCONTROLLERSERVICENODE_H_
#define LIBMINIFI_INCLUDE_CORE_CONTROLLER_STANDARDCONTROLLERSERVICENODE_H_

#include "core/core.h"
#include "ControllerServiceNode.h"
#include "core/logging/Logger.h"
#include "core/ProcessGroup.h"
#include "ControllerService.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {
namespace controller {

class StandardControllerServiceNode : public ControllerServiceNode {

  StandardControllerServiceNode(std::shared_ptr<ControllerService> service,
                                const std::string &id)
      : ControllerServiceNode(service, id) {
  }
  std::shared_ptr<core::ProcessGroup> &getProcessGroup();

  void setProcessGroup(std::shared_ptr<ProcessGroup> &processGroup);

  StandardControllerServiceNode(const StandardControllerServiceNode &other) = delete;
  StandardControllerServiceNode &operator=(
      const StandardControllerServiceNode &parent) = delete;

 protected:

  // process group.
  std::shared_ptr<core::ProcessGroup> process_group_;

};

std::shared_ptr<core::ProcessGroup> &StandardControllerServiceNode::getProcessGroup() {
  std::lock_guard<std::mutex> lock(mutex_);
  return process_group_;
}

void StandardControllerServiceNode::setProcessGroup(
    std::shared_ptr<ProcessGroup> &processGroup) {
  std::lock_guard<std::mutex> lock(mutex_);
  process_group_ = processGroup;
}

} /* namespace controller */
} /* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_CONTROLLER_STANDARDCONTROLLERSERVICENODE_H_ */
