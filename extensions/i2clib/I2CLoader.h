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
#ifndef EXTENSIONS_HTTPCURLLOADER_H_
#define EXTENSIONS_HTTPCURLLOADER_H_

#include "../i2clib/monitoring/C2Controller.h"
#include "../i2clib/sunfounder/SmartCar.h"
#include "../i2clib/sensehat/SenseHat.h"

class __attribute__((visibility("default"))) PICarFactory : public core::ObjectFactory {
 public:
  PICarFactory() {

  }

  /**
   * Gets the name of the object.
   * @return class name of processor
   */
  virtual std::string getName() override{
    return "PICarFactory";
  }

  virtual std::string getClassName() override{
    return "PICarFactory";
  }
  /**
   * Gets the class name for the object
   * @return class name for the processor.
   */
  virtual std::vector<std::string> getClassNames() override{
    std::vector<std::string> class_names;
    class_names.push_back("SenseHAT");
    return class_names;
  }

  virtual std::unique_ptr<ObjectFactory> assign(const std::string &class_name) override{
    if (utils::StringUtils::equalsIgnoreCase(class_name, "C2Controller")) {
      return std::unique_ptr<ObjectFactory>(new core::DefautObjectFactory<minifi::c2::C2Controller>());
    }else if (utils::StringUtils::equalsIgnoreCase(class_name, "SenseHAT")) {
      return std::unique_ptr<ObjectFactory>(new core::DefautObjectFactory<minifi::processors::SenseHAT>());
    } else {
      return nullptr;
    }
  }

  static bool added;

};

extern "C" {
void *createpicarfactory(void);
}
#endif /* EXTENSIONS_HTTPCURLLOADER_H_ */
