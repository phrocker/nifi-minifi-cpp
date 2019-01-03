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
#ifndef LIBMINIFI_INCLUDE_CONTROLLERS_JAVACONTROLLERSERVICE_H_
#define LIBMINIFI_INCLUDE_CONTROLLERS_JAVACONTROLLERSERVICE_H_

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "../jvm/JVMLoader.h"
#include "core/Resource.h"
#include "utils/StringUtils.h"
#include "io/validation.h"
#include "core/controller/ControllerService.h"
#include "core/logging/LoggerConfiguration.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace controllers {

/**
 * Currently intended to be a singleton
 *
 */
class JavaControllerService : public core::controller::ControllerService {
 public:
  explicit JavaControllerService(const std::string &name, const std::string &id)
      : ControllerService(name, id),
        loader(nullptr),
        logger_(logging::LoggerFactory<JavaControllerService>::getLogger()) {
    initialized_ = false;
  }

  explicit JavaControllerService(const std::string &name, utils::Identifier uuid = utils::Identifier())
      : ControllerService(name, uuid),
        loader(nullptr),
        logger_(logging::LoggerFactory<JavaControllerService>::getLogger()) {
    initialized_ = false;
  }

  explicit JavaControllerService(const std::string &name, const std::shared_ptr<Configure> &configuration)
      : ControllerService(name),
        loader(nullptr),
        logger_(logging::LoggerFactory<JavaControllerService>::getLogger()) {
    initialized_ = false;
    setConfiguration(configuration);
    initialize();
  }

  static core::Property Paths;

  virtual void initialize();

  void yield() {
  }

  bool isRunning() {
    return getState() == core::controller::ControllerServiceState::ENABLED;
  }

  bool isWorkAvailable() {
    return false;
  }

  virtual void onEnable();

  std::vector<std::string> getPaths() const {
    return classpaths_;
  }

  JavaClass loadClass(const std::string &class_name_) {
    std::string modifiedName = class_name_;
    modifiedName = utils::StringUtils::replaceAll(modifiedName, ".", "/");
    return loader->load_class(modifiedName);
  }

  template<typename T>
  void setReference(jobject obj, T *t) {
    loader->setReference(obj, t);
  }

 protected:

  void addPath(std::vector<std::string> &jarFiles, const std::string &dir);

 private:

  std::mutex initialization_mutex_;

  std::atomic<bool> initialized_;

  std::vector<std::string> classpaths_;

  JVMLoader *loader;

  std::shared_ptr<logging::Logger> logger_;

};

} /* namespace controllers */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CONTROLLERS_JAVACONTROLLERSERVICE_H_ */
