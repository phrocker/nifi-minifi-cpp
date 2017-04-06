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
#ifndef LIBMINIFI_INCLUDE_CORE_CONTROLLER_CONTROLLERSERVICEPROVIDER_H_
#define LIBMINIFI_INCLUDE_CORE_CONTROLLER_CONTROLLERSERVICEPROVIDER_H_

#include <vector>
#include "core/core.h"
#include "core/ConfigurableComponent.h"
#include "ControllerServiceNode.h"
#include "SchedulingAgent.h"
#include "core/ClassLoader.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {
namespace controller {

class ControllerServiceProvider : public CoreComponent,
    public ConfigurableComponent {
 public:

  ControllerServiceProvider(minifi::SchedulingAgent &agent,
                            ClassLoader &loader =
                                ClassLoader::getDefaultClassLoader())
      : CoreComponent(core::getClassName<ControllerServiceProvider>()),
        agent_(agent),
        extension_loader_(loader) {

  }

  /**
   * Creates a new Controller Service of the specified type and assigns it the
   * given id. If <code>firstTimeadded</code> is true, calls any methods that
   * are annotated with {@link OnAdded}
   *
   * @param type of service
   * @param id of service
   * @param firstTimeAdded for service
   * @return the service node
   */
  std::unique_ptr<ControllerServiceNode> createControllerService(
      const std::string &type, const std::string &id,
      bool firstTimeAdded) {

    std::shared_ptr<ControllerService> new_controller_service =
        extension_loader_.instantiate<ControllerService>(type, id);

    if (nullptr == new_controller_service) {
      return nullptr;
    }

    ControllerServiceNode *newNode = new ControllerServiceNode(
        new_controller_service, id);

    return std::unique_ptr<ControllerServiceNode>(newNode);

  }

  /**
   * @param id of the service
   * @return the controller service node for the specified identifier. Returns
   * <code>null</code> if the identifier does not match a known service
   */
  ControllerServiceNode &getControllerServiceNode(const std::string &id) {
  }

  /**
   * Removes the given Controller Service from the flow. This will call all
   * appropriate methods that have the @OnRemoved annotation.
   *
   * @param serviceNode the controller service to remove
   *
   * @throws IllegalStateException if the controller service is not disabled
   * or is not a part of this flow
   */
  void removeControllerService(ControllerServiceNode &serviceNode) {
  }

  /**
   * Enables the given controller service that it can be used by other
   * components
   *
   * @param serviceNode the service node
   */
  void enableControllerService(ControllerServiceNode &serviceNode) {
  }

  /**
   * Enables the collection of services. If a service in this collection
   * depends on another service, the service being depended on must either
   * already be enabled or must be in the collection as well.
   *
   * @param serviceNodes the nodes
   */
  void enableControllerServices(std:vector<core::controller::ControllerServiceNode> serviceNodes) {}

  /**
   * Disables the given controller service so that it cannot be used by other
   * components. This allows configuration to be updated or allows service to
   * be removed.
   *
   * @param serviceNode the node
   */
  void disableControllerService(ControllerServiceNode &serviceNode) {
  }

  /**
   * @return a Set of all Controller Services that exist for this service
   * provider
   */
  std::vector<ControllerServiceNode> getAllControllerServices() {
  }

  /**
   * Verifies that all running Processors and Reporting Tasks referencing the
   * Controller Service (or a service that depends on the provided service)
   * can be stopped.
   *
   * @param serviceNode the node
   *
   * @throws IllegalStateException if any referencing component cannot be
   * stopped
   */
  void verifyCanStopReferencingComponents(ControllerServiceNode &serviceNode) {
  }

  /**
   * Recursively unschedules all schedulable components (Processors and
   * Reporting Tasks) that reference the given Controller Service. For any
   * Controller services that reference this one, its schedulable referencing
   * components will also be unscheduled.
   *
   * @param serviceNode the node
   */
  std::vector<core::ConfigurableComponent> unscheduleReferencingComponents(
      ControllerServiceNode &serviceNode) {
  }

  /**
   * Verifies that all Controller Services referencing the provided Controller
   * Service can be disabled.
   *
   * @param serviceNode the node
   *
   * @throws IllegalStateException if any referencing service cannot be
   * disabled
   */
  void verifyCanDisableReferencingServices(ControllerServiceNode &serviceNode) {
  }

  /**
   * Disables any Controller Service that references the provided Controller
   * Service. This action is performed recursively so that if service A
   * references B and B references C, disabling references for C will first
   * disable A, then B.
   *
   * @param serviceNode the node
   */
  std::vector<core::ConfigurableComponent> disableReferencingServices(
      ControllerServiceNode &serviceNode) {
  }

  /**
   * Verifies that all Controller Services referencing the provided
   * ControllerService can be enabled.
   *
   * @param serviceNode the node
   *
   * @throws IllegalStateException if any referencing component cannot be
   * enabled
   */
  void verifyCanEnableReferencingServices(ControllerServiceNode &serviceNode) {
  }

  /**
   * Enables all Controller Services that are referencing the given service.
   * If Service A references Service B and Service B references serviceNode,
   * Service A and B will both be enabled.
   *
   * @param serviceNode the node
   *
   * @return the set of all components that were updated as a result of this action
   */
  std::vector<core::ConfigurableComponent> enableReferencingServices(
      ControllerServiceNode &serviceNode) {
  }

  /**
   * Verifies that all enabled Processors referencing the ControllerService
   * (or a service that depends on the provided service) can be scheduled to
   * run.
   *
   * @param serviceNode the node
   *
   * @throws IllegalStateException if any referencing component cannot be
   * scheduled
   */
  void verifyCanScheduleReferencingComponents(
      ControllerServiceNode &serviceNode) {
  }

  /**
   * Schedules any schedulable component (Processor, ReportingTask) that is
   * referencing the given Controller Service to run. This is performed
   * recursively, so if a Processor is referencing Service A, which is
   * referencing serviceNode, then the Processor will also be started.
   *
   * @param serviceNode the node
   */
  std::vector<core::ConfigurableComponent> scheduleReferencingComponents(
      ControllerServiceNode &serviceNode) {
  }

  /**
   *
   * @param serviceType type of service to get identifiers for
   * @param groupId the ID of the Process Group to look in for Controller Services
   *
   * @return the set of all Controller Service Identifiers whose Controller
   *         Service is of the given type.
   * @throws IllegalArgumentException if the given class is not an interface
   */
  //std::vector<std::string> getControllerServiceIdentifiers(Class<? extends ControllerService> serviceType, const std::string &groupId) throws IllegalArgumentException;
  /**
   * @param serviceIdentifier the identifier of the controller service
   * @param componentId the identifier of the component that is referencing the service.
   * @return the Controller Service that is registered with the given identifier or <code>null</code> if that
   *         identifier does not exist for any controller service or if the controller service with that identifier is
   *         not accessible from the component with the given componentId, or if no component exists with the given
   *         identifier
   */
  ControllerService &getControllerServiceForComponent(
      const std::string &serviceIdentifier, const std::string &componentId) {
  }
 protected:

  minifi::SchedulingAgent &agent_;

  ClassLoader &extension_loader_;
};

} /* namespace controller */
} /* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_CONTROLLER_CONTROLLERSERVICEPROVIDER_H_ */
