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

#ifndef LIBMINIFI_INCLUDE_CORE_CONFIGURATIONFACTORY_H_
#define LIBMINIFI_INCLUDE_CORE_CONFIGURATIONFACTORY_H_

#include "FlowConfiguration.h"
#include  <type_traits>
//#ifdef YAML_SUPPORT
#include "yaml/YamlConfiguration.h"
//#endif

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {


/**
 * Configuration factory is used to create a new FlowConfiguration
 * object.
 */
class ConfigurationFactory {
 public:

  /**
   * static factory function to create a new FlowConfiguration Instance.
   */
  static std::unique_ptr<core::FlowConfiguration> create(
      std::shared_ptr<core::Repository> repo,
      std::shared_ptr<core::Repository> flow_file_repo,
      const std::string configuration_class_name, const std::string path = "",
      bool fail_safe = false) {

    std::string class_name_lc = configuration_class_name;
    std::transform(class_name_lc.begin(), class_name_lc.end(),
                   class_name_lc.begin(), ::tolower);
    try {

      if (class_name_lc == "flowconfiguration") {
        return std::unique_ptr<core::FlowConfiguration>(
            new core::FlowConfiguration(repo, flow_file_repo, path));

      } else if (class_name_lc == "yamlconfiguration") {
        return std::unique_ptr<core::FlowConfiguration>(
            new core::YamlConfiguration(repo, flow_file_repo, path));

      } else {
        if (fail_safe) {
          return std::unique_ptr<core::FlowConfiguration>(
              new core::FlowConfiguration(repo, flow_file_repo, path));
        } else {
          throw std::runtime_error(
              "Support for the provided configuration class could not be found");
        }
      }
    } catch (const std::runtime_error &r) {
      if (fail_safe) {
        return std::unique_ptr<core::FlowConfiguration>(
            new core::FlowConfiguration(repo, flow_file_repo, path));
      }
    }

    throw std::runtime_error(
        "Support for the provided configuration class could not be found");
  }

  ConfigurationFactory(const ConfigurationFactory &parent) = delete;
  ConfigurationFactory &operator=(const ConfigurationFactory &parent) = delete;
};

} /* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_CONFIGURATIONFACTORY_H_ */
