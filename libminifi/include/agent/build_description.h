#ifndef BUILD_DESCRPTION_H
#define BUILD_DESCRPTION_H

#include <vector>
#include "capi/expect.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {

struct ClassDescription {
  explicit ClassDescription(std::string name)
      : class_name_(name),
        support_dynamic_(false) {
  }
  explicit ClassDescription(std::string name, std::vector<std::string> props, bool dyn)
      : class_name_(name),
        class_properties_(props),
        support_dynamic_(dyn) {

  }
  std::string class_name_;
  std::vector<std::string> class_properties_;
  bool support_dynamic_;
};

class BuildDescription {
 public:

  static std::vector<ClassDescription> getClassDescriptions() {
    static std::vector<ClassDescription> classes;
    if (UNLIKELY(IsNullOrEmpty(classes))) {
      for (auto clazz : core::ClassLoader::getDefaultClassLoader().getClasses()) {

        auto lastOfIdx = clazz.find_last_of("::");
        if (lastOfIdx != std::string::npos) {
          lastOfIdx++;  // if a value is found, increment to move beyond the .
          int nameLength = clazz.length() - lastOfIdx;
          std::string class_name = clazz.substr(lastOfIdx, nameLength);


          auto obj = core::ClassLoader::getDefaultClassLoader().instantiate(class_name, class_name);

          std::shared_ptr<core::ConfigurableComponent> component = std::dynamic_pointer_cast<core::ConfigurableComponent>(obj);

          ClassDescription description(clazz);
          if (nullptr != component) {
            component->initialize();
            description.class_properties_ = component->getProperties();
            description.support_dynamic_ = component->supportsDynamicProperties();
            classes.emplace_back(description);
          }

        }

      }
    }
    return classes;
  }

};

} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* BUILD_DESCRPTION_H */
