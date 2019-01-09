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
#ifndef EXTENSIONS_NARCLASSLOADER_H
#define EXTENSIONS_NARCLASSLOADER_H

#include "JavaControllerService.h"
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <jni.h>

class NarClassLoader {

 public:

  NarClassLoader(std::shared_ptr<minifi::controllers::JavaControllerService> servicer, JavaClass &clazz, const std::string &dir_name) : java_servicer_(servicer) {
    class_ref_ = clazz;
    class_loader_ = class_ref_.newInstance();
    auto env = class_ref_.getEnv();
    jmethodID mthd = env->GetMethodID(class_ref_.getReference(), "setClassDir", "(Ljava/lang/String;Ljava/lang/ClassLoader;)V");
    if (mthd == nullptr) {
      printf("Find method Failed. 3\n");

    } else {
      printf("Found method.\n");
      auto clazz_name = env->NewStringUTF(dir_name.c_str());

      env->CallVoidMethod(class_loader_, mthd, clazz_name, servicer->getClassLoader());
    }

  }

  jclass getClass(const std::string &requested_name) {
    auto env = class_ref_.getEnv();
    jmethodID mthd = env->GetMethodID(class_ref_.getReference(), "getClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    if (mthd == nullptr) {
      printf("Find method Failed. 3\n");
      return nullptr;
    } else {
      printf("Found method.\n");
    }

    auto clazz_name = env->NewStringUTF(requested_name.c_str());

    auto job = env->CallObjectMethod(class_loader_, mthd, clazz_name);

    return (jclass) job;
  }
  /**
   * Call empty constructor
   */
  JNIEXPORT
  jobject newInstance(const std::string &requested_name) {
    auto env = class_ref_.getEnv();

    java_servicer_->attach();

    jmethodID mthd = env->GetMethodID(class_ref_.getReference(), "createObject", "(Ljava/lang/String;)Ljava/lang/Object;");
    if (mthd == nullptr) {
      printf("Find method Failed 2.\n");
      return nullptr;
    } else {
      printf("Found method.\n");
    }

    auto clazz_name = env->NewStringUTF(requested_name.c_str());

    jobject obj = env->CallObjectMethod(class_loader_, mthd, clazz_name);

    if (env->ExceptionOccurred()) {
          std::cout << "Exception occurred" << std::endl;
          env->ExceptionDescribe();
          env->ExceptionClear();
        }

    return obj;
  }

 private:
  std::shared_ptr<minifi::controllers::JavaControllerService> java_servicer_;
  JavaClass class_ref_;
  jobject class_loader_;
};

#endif /* EXTENSIONS_NARCLASSLOADER_H */
