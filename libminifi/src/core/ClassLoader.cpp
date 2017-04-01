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


#include "core/ClassLoader.h"


namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {


ClassLoader &ClassLoader::getDefaultClassLoader()
{
    static ClassLoader ret;
    // populate ret
    return ret;
}
short ClassLoader::registerResource(const std::string &resource) {
  void* resource_ptr = dlopen(resource.c_str(), RTLD_LAZY);
  if (!resource_ptr) {
    logger_->log_error("Cannot load library: %s", dlerror());
    return RESOURCE_FAILURE;
  } else {
    std::lock_guard<std::mutex> lock(internal_mutex_);
    dl_handles_.push_back(resource_ptr);
  }

  // reset errors
  dlerror();

  // load the symbols
  createFactory* create_factory_func = (createFactory*) dlsym(
      resource_ptr, "createFactory");
  const char* dlsym_error = dlerror();
  if (dlsym_error) {
    logger_->log_error("Cannot load library: %s", dlsym_error);
    return RESOURCE_FAILURE;
  }

  ProcessorFactory *factory = create_factory_func();

  std::lock_guard<std::mutex> lock(internal_mutex_);

  loaded_factories_[factory->getClassName()] = 
      std::unique_ptr<ProcessorFactory>(factory);

  return RESOURCE_SUCCESS;

}

short ClassLoader::registerResource(io::DataStream &stream) {
  // TODO: copy stream to fs so we can read 
return 0;

}

} /* namespace processor */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */