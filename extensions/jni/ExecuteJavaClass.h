/**
 * ExecuteJavaClass class declaration
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
#ifndef __EXECUTE_JAVA_CLASS__
#define __EXECUTE_JAVA_CLASS__

#include <memory>
#include <regex>

#include "FlowFileRecord.h"
#include "core/Processor.h"
#include "core/ProcessSession.h"
#include "core/Core.h"
#include "core/Property.h"
#include "core/Resource.h"
#include "concurrentqueue.h"
#include "core/logging/LoggerConfiguration.h"
#include "jvm/JavaControllerService.h"
#include "jvm/JniProcessContext.h"
#include "utils/Id.h"
#include "jvm/NarClassLoader.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

// ExecuteJavaClass Class
class ExecuteJavaClass : public core::Processor {
 public:

  // Constructor
  /*!
   * Create a new processor
   */
  explicit ExecuteJavaClass(std::string name, utils::Identifier uuid = utils::Identifier())
      : Processor(name, uuid),
        logger_(logging::LoggerFactory<ExecuteJavaClass>::getLogger()) {
  }
  // Destructor
  virtual ~ExecuteJavaClass();
  // Processor Name
  static const char *ProcessorName;
  static core::Property JVMControllerService;
  static core::Property NiFiProcessor;
  static core::Property NarDirectory;

  // Supported Relationships
  static core::Relationship Success;

  virtual void onTrigger(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) override;
  virtual void initialize() override;
  virtual void onSchedule(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSessionFactory> &sessionFactory) override;
  virtual bool supportsDynamicProperties() override {
    return true;
  }


 protected:

  virtual void notifyStop() override {
  }

 private:
  std::unique_ptr<NarClassLoader> loader_;

  JavaClass narClassLoaderClazz;

  JavaClass spn;

  JavaClass init;

  JavaClass current_processor_class;

  jobject clazzInstance;

  std::shared_ptr<controllers::JavaControllerService> java_servicer_;

  std::string class_name_;

  std::shared_ptr<logging::Logger> logger_;
};

REGISTER_RESOURCE(ExecuteJavaClass, "ExecuteJavaClass runs NiFi processors given a provided system path ")

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
