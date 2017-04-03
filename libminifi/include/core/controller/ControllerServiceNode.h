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
#ifndef LIBMINIFI_INCLUDE_CORE_CONTROLLER_CONTROLLERSERVICENODE_H_
#define LIBMINIFI_INCLUDE_CORE_CONTROLLER_CONTROLLERSERVICENODE_H_

#include "core/core.h"
#include "core/ConfigurableComponent.h"
#include "core/ProcessGroupNode.h"
#include "ControllerService.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {
namespace controller {

class ControllerServiceNode : public CoreComponent,
    public ConfigurableComponent, public ProcessGroupNode {
 public:

  ControllerServiceNode(std::shared_ptr<ControllerService> service,
                        const std::string &id)
      : CoreComponent(id),
        controller_service_(service) {

  }

  /**
   * <p>
   * Returns the actual implementation of the Controller Service that this ControllerServiceNode
   * encapsulates. This direct implementation should <strong>NEVER</strong> be passed to another
   * pluggable component. This implementation should be addressed only by the framework itself.
   * If providing the controller service to another pluggable component, provide it with the
   * proxied entity obtained via {@link #getProxiedControllerService()}
   * </p>
   *
   * @return the actual implementation of the Controller Service
   */
  std::shared_ptr<ControllerService> getControllerServiceImplementation();

  ControllerServiceNode(const ControllerServiceNode &other) = delete;
  ControllerServiceNode &operator=(const ControllerServiceNode &parent) = delete;
 protected:

  std::mutex mutex_;

  std::shared_ptr<core::ProcessGroup> process_group_;
  std::shared_ptr<ControllerService> controller_service_;
};

} /* namespace controller */
} /* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_CONTROLLER_CONTROLLERSERVICENODE_H_ */
