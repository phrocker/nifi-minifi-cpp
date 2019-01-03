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
#ifndef EXTENSIONS_NIFIPROCESSOR_H
#define EXTENSIONS_NIFIPROCESSOR_H

#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <jni.h>

class NIFIProcessor {

 public:

  NIFIProcessor(const std::string &name, jobject proc_ref_)
      : name_(name),
        class_ref_(classref),
        jenv_(jenv) {
  }

  jclass getReference() {
    return class_ref_;
  }

  /**
   * Call empty constructor
   */
  JNIEXPORT
  jobject newInstance() {
    std::string instanceName = "(L" + name_ + ";)V";
    jmethodID cnstrctr = jenv_->GetMethodID(class_ref_, "<init>", "()V");
    if (cnstrctr == nullptr) {
      printf("Find method Failed.\n");
      return nullptr;
    } else {
      printf("Found method.\n");
    }

    return jenv_->NewObject(class_ref_, cnstrctr);
  }

 private:
  jobject proc_ref_;
};

#endif /* EXTENSIONS_NIFIPROCESSOR_H */
