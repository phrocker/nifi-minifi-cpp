/*
 * RepositoryFactory.h
 *
 *  Created on: Mar 24, 2017
 *      Author: mparisi
 */

#ifndef LIBMINIFI_INCLUDE_CORE_REPOSITORYFACTORY_H_
#define LIBMINIFI_INCLUDE_CORE_REPOSITORYFACTORY_H_

#include <type_traits>
#include "core/Repository.h"
#include "core.h"
#ifdef LEVELDB_SUPPORT
#include "core/repository/FlowFileRepository.h"
#include "provenance/ProvenanceRepository.h"
#endif

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace provenance {
class ProvenanceRepository;
}
namespace core {

namespace repository {

class FlowFileRepository;
}

template<typename T>
typename std::enable_if<!class_operations<T>::value, std::shared_ptr<T>>::type instantiate() {
  throw std::runtime_error("Cannot instantiate class");
}

template<typename T>
typename std::enable_if<class_operations<T>::value, std::shared_ptr<T>>::type instantiate() {
  return nullptr;
}

/**
 * Configuration factory is used to create a new FlowConfiguration
 * object.
 */
class RepositoryFactory {
 public:

  /**
   * static factory function to create a new FlowConfiguration Instance.
   */
  static std::shared_ptr<core::Repository> create(
      const std::string configuration_class_name, bool fail_safe = false) {

    std::string class_name_lc = configuration_class_name;
    std::transform(class_name_lc.begin(), class_name_lc.end(),
                   class_name_lc.begin(), ::tolower);
    try {
      std::shared_ptr<core::Repository> return_obj = nullptr;
      if (class_name_lc == "flowfilerepository") {

        return_obj = instantiate<core::repository::FlowFileRepository>();
      } else if (class_name_lc == "provenancerepository") {

          return_obj = std::make_shared<provenance::ProvenanceRepository>();

      }
      if (return_obj)
        return return_obj;
      if (fail_safe) {
        return std::make_shared<core::Repository>("fail_safe", "fail_safe", 1,
                                                  1, 1);
      } else {
        throw std::runtime_error(
            "Support for the provided configuration class could not be found");
      }
    } catch (const std::runtime_error &r) {
      if (fail_safe) {
        return std::make_shared<core::Repository>("fail_safe", "fail_safe", 1,
                                                  1, 1);
      }
    }

    throw std::runtime_error(
        "Support for the provided configuration class could not be found");
  }
  RepositoryFactory(const RepositoryFactory &parent) = delete;
  RepositoryFactory &operator=(const RepositoryFactory &parent) = delete;

};

} /* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_REPOSITORYFACTORY_H_ */
