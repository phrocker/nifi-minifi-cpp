/**
 * @file AccumuloWriter.cpp
 * AccumuloWriter class implementation
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
#ifndef EXTENSIONS_ACCUMULO_ACCUMULOLOADER_H_
#define EXTENSIONS_ACCUMULO_ACCUMULOLOADER_H_

#include "AccumuloWriter.h"
#include "BulkImport.h"
#include "core/ClassLoader.h"

class __attribute__((visibility("default"))) AccumuloObjectFactory : public core::ObjectFactory {
 public:
  AccumuloObjectFactory() {

  }

  /**
   * Gets the name of the object.
   * @return class name of processor
   */
  virtual std::string getName() {
    return "AccumuloObjectFactory";
  }

  virtual std::string getClassName() {
    return "AccumuloObjectFactory";
  }
  /**
   * Gets the class name for the object
   * @return class name for the processor.
   */
  virtual std::vector<std::string> getClassNames() {
    std::vector<std::string> class_names;
    class_names.push_back("AccumuloWriter");
    class_names.push_back("BulkImport");
    return class_names;
  }

  virtual std::unique_ptr<ObjectFactory> assign(const std::string &class_name) {
    if (class_name == "AccumuloWriter") {
      return std::unique_ptr<ObjectFactory>(new core::DefautObjectFactory<minifi::processors::AccumuloWriter>());
    } else if (class_name == "BulkImport") {
      return std::unique_ptr<ObjectFactory>(new core::DefautObjectFactory<minifi::processors::BulkImport>());
    } else {
      return nullptr;
    }
  }

};


extern "C" {
void *createAccumuloFactory(void);
}
#endif /* EXTENSIONS_ACCUMULO_ACCUMULOLOADER_H_ */
