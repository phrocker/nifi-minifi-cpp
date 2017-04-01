/**
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

#ifndef LIBMINIFI_INCLUDE_CORE_CLASSLOADER_H_
#define LIBMINIFI_INCLUDE_CORE_CLASSLOADER_H_

#include <mutex>
#include <vector>
#include <map>
#include "Connectable.h"
#include "utils/StringUtils.h"
#include <dlfcn.h>
#include "core/core.h"
#include "core/logging/Logger.h"
#include "io/DataStream.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {

#define RESOURCE_FAILURE -1

#define RESOURCE_SUCCESS 1

/**
 * Factory that is used as an interface for
 * creating processors from shared objects.
 */
class ProcessorFactory {

 public:
  /**
   * Virtual destructor.
   */
  virtual ~ProcessorFactory() {

  }

  /**
   * Create a shared pointer to a new processor.
   */
  virtual std::shared_ptr<Connectable> create(const std::string &name) {
    return nullptr;
  }
  
   /**
   * Create a shared pointer to a new processor.
   */
  virtual std::shared_ptr<Connectable> create(const std::string &name, uuid_t uuid) {
    return nullptr;
  }

  /**
   * Gets the name of the object.
   * @return class name of processor
   */
  virtual std::string getName() = 0;

  /**
   * Gets the class name for the object
   * @return class name for the processor.
   */
  virtual std::string getClassName() = 0;

};

/**
 * Factory that is used as an interface for
 * creating processors from shared objects.
 */
template<class T>
class DefaultProcessorFactory : public ProcessorFactory {

 public:

  DefaultProcessorFactory() {
    className = core::getClassName<T>();
  }
  /**
   * Virtual destructor.
   */
  virtual ~DefaultProcessorFactory() {

  }

  /**
   * Create a shared pointer to a new processor.
   */
  virtual std::shared_ptr<Connectable> create(const std::string &name) {
    std::shared_ptr<T> ptr = std::make_shared<T>(name);
    return std::static_pointer_cast<Connectable>(ptr);
  }
  
    /**
   * Create a shared pointer to a new processor.
   */
  virtual std::shared_ptr<Connectable> create(const std::string &name, uuid_t uuid) {
    std::shared_ptr<T> ptr = std::make_shared<T>(name,uuid);
    return std::static_pointer_cast<Connectable>(ptr);
  }

  /**
   * Gets the name of the object.
   * @return class name of processor
   */
  virtual std::string getName() {
    return className;
  }

  /**
   * Gets the class name for the object
   * @return class name for the processor.
   */
  virtual std::string getClassName() {
    return className;
  }

 protected:
  std::string className;

};

/**
 * Function that is used to create the
 * processor factory from the shared object.
 */
typedef ProcessorFactory* createFactory();

/**
 * Processor class loader that accepts
 * a variety of mechanisms to load in shared
 * objects.
 */
class ClassLoader {

 public:

   static ClassLoader &getDefaultClassLoader();
  
  
  /**
   * Constructor.
   */
  ClassLoader()
      : logger_(logging::Logger::getLogger()) {

  }

  ~ClassLoader() {
    loaded_factories_.clear();
    for (auto ptr : dl_handles_) {
      dlclose(ptr);
    }
  }

  short registerResource(const std::string &resource);

  short registerResource(io::DataStream &stream);

  void registerClass(const std::string &name,
                     std::unique_ptr<ProcessorFactory> factory) {
    if (loaded_factories_.find(name) != loaded_factories_.end())
      return;
    
    std::lock_guard<std::mutex> lock(internal_mutex_);
    std::cout << "registering " << name << std::endl;
    
    loaded_factories_.insert(std::make_pair(name,std::move(factory)));
  }

  template<typename ... Targs>
  std::shared_ptr<Connectable> instantiate(const std::string &class_name, const std::string &name) {

    std::lock_guard<std::mutex> lock(internal_mutex_);
    auto factory_entry = loaded_factories_.find(class_name);
    if (factory_entry != loaded_factories_.end()) {
      return factory_entry->second->create(name);
    } else {
      std::cout<<"could not find " << class_name << std::endl;
      return nullptr;
    }
  }
  
   template<typename ... Targs>
  std::shared_ptr<Connectable> instantiate(const std::string &class_name, uuid_t uuid) {

    std::lock_guard<std::mutex> lock(internal_mutex_);
    auto factory_entry = loaded_factories_.find(class_name);
    if (factory_entry != loaded_factories_.end()) {
      return factory_entry->second->create(class_name,uuid);
    } else {
      std::cout<<"could not find " << class_name << std::endl;
      return nullptr;
    }
  }

 protected:

  // logger shared ptr
  std::shared_ptr<org::apache::nifi::minifi::core::logging::Logger> logger_;

  std::map<std::string, std::unique_ptr<ProcessorFactory>> loaded_factories_;

  std::mutex internal_mutex_;

  std::vector<void *> dl_handles_;

};


}/* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_CLASSLOADER_H_ */
