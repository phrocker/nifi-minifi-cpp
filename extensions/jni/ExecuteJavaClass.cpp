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

#include <regex>
#include <uuid/uuid.h>
#include <memory>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "core/FlowFile.h"
#include "core/logging/Logger.h"
#include "core/ProcessContext.h"
#include "core/Relationship.h"
#include "ExecuteJavaClass.h"
#include "ResourceClaim.h"
#include "utils/StringUtils.h"
#include "utils/ByteArrayCallback.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

core::Property ExecuteJavaClass::JVMControllerService(
    core::PropertyBuilder::createProperty("JVM Controller Service")->withDescription("Name of controller service defined within this flow")->isRequired(true)->withDefaultValue<std::string>("")->build());
core::Property ExecuteJavaClass::NiFiProcessor(
    core::PropertyBuilder::createProperty("NiFi Processor")->withDescription("Name of NiFi processor to load and run")->isRequired(true)->withDefaultValue<std::string>("")->build());

const char *ExecuteJavaClass::ProcessorName = "ExecuteJavaClass";

core::Relationship ExecuteJavaClass::Success("success", "All files are routed to success");
void ExecuteJavaClass::initialize() {
  logger_->log_info("Initializing ExecuteJavaClass");
  // Set the supported properties
  std::set<core::Property> properties;
  properties.insert(JVMControllerService);
  properties.insert(NiFiProcessor);
  setSupportedProperties(properties);
  // Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert(Success);
  setSupportedRelationships(relationships);
}

void ExecuteJavaClass::onSchedule(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSessionFactory> &sessionFactory) {
  std::string controller_service_name;

  if (getProperty(JVMControllerService.getName(), controller_service_name)) {
    auto cs = context->getControllerService(controller_service_name);
    if (cs == nullptr) {
      throw std::runtime_error("Could not load controller service");
    }
    java_servicer_ = std::static_pointer_cast<controllers::JavaControllerService>(cs);

  } else {
    throw std::runtime_error("Could not load controller service");
  }

  if (!getProperty(NiFiProcessor.getName(), class_name_)) {
    throw std::runtime_error("NiFi Processor must be defined");
  }

  auto clazz = java_servicer_->loadClass(class_name_);
  auto clazzInstance = clazz.newInstance();
  // attempt to schedule here
  spn = java_servicer_->loadClass("org/apache/nifi/processor/JniProcessContext");

  init = java_servicer_->loadClass("org/apache/nifi/processor/JniInitializationContext");

  auto obj = spn.newInstance();

  auto initializer = init.newInstance();



  context->setDynamicProperty("Unique FlowFiles","false");

  spn.registerMethods();

  java_servicer_->setReference<core::ProcessContext>(obj,context.get());
  clazz.callVoidMethod(clazzInstance,"init","(Lorg/apache/nifi/processor/ProcessorInitializationContext;)V", initializer);
  std::cout << "calling" << std::endl;
  clazz.callVoidMethod(clazzInstance,"onScheduled","(Lorg/apache/nifi/processor/ProcessContext;)V", obj);
  std::cout << "called" << std::endl;

}

ExecuteJavaClass::~ExecuteJavaClass() {
}

void ExecuteJavaClass::onTrigger(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) {

  auto java_context = spn.newInstance();


}

}
/* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
