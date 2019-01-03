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
#ifndef EXTENSIONS_JVMLOADER_H
#define EXTENSIONS_JVMLOADER_H

#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include "JavaClass.h"
#include <jni.h>
#include <dlfcn.h>
#include "Core.h"

typedef jint (*registerNatives_t)(JNIEnv* env, jclass clazz);

jfieldID getPtrField(JNIEnv *env, jobject obj);

template<typename T>
T *getPtr(JNIEnv *env, jobject obj);

template<typename T>
void setPtr(JNIEnv *env, jobject obj, T *t);

class JVMLoader {

 public:

  bool initialized() {
    return initialized_;
  }

  JavaClass load_class(const std::string &name) {
    auto finder = objects_.find(name);
    if (finder != objects_.end()) {
      return finder->second;
    }
    JNIEnv* jenv;
    jint ret = jvm_->GetEnv((void**) &jenv, JNI_VERSION_1_8);
    if (ret == JNI_EDETACHED) {
      ret = jvm_->AttachCurrentThread((void**) &jenv, NULL);
      if (ret != JNI_OK || jenv == NULL) {
        throw std::runtime_error("Could not find class");
      } else {
        std::cout << "got env for " << name << std::endl;
      }
    } else if (ret == JNI_OK) {
      std::cout << "got env for " << name << std::endl;
    }
    jclass classref;
    classref = jenv->FindClass(name.c_str());
    if (classref == NULL) {

      std::cout << name << " not found " << std::endl;
      if (jenv->ExceptionOccurred()) {
        std::cout << "Exception occurred" << std::endl;
        jenv->ExceptionDescribe();
        jenv->ExceptionClear();
      }
    }
    JavaClass clazz(name, classref, jenv);
    objects_.insert(std::make_pair(name, clazz));
    return clazz;
  }

  static JVMLoader *getInstance(const std::vector<std::string> &pathVector, const std::vector<std::string> &otherOptions = std::vector<std::string>()) {
    static JVMLoader jvm;
    if (!jvm.initialized()) {
      std::stringstream str;
      std::vector<std::string> options;
      for (const auto &path : pathVector) {
        if (str.str().length() > 0) {
#ifdef WIN32
          str << ";" << path;
#else
          str << ":" << path;
#endif
        } else
          str << path;
      }
      options.insert(options.end(), otherOptions.begin(), otherOptions.end());
      std::string classpath = "-Djava.class.path=" + str.str();
      options.push_back(classpath);
      jvm.initialize(options);
    }
    return &jvm;
  }

  template<typename T>
  void setReference(jobject obj, T *t) {
    setPtr(env_, obj, t);
  }

  static jfieldID getPtrField(JNIEnv *env, jobject obj) {
    jclass c = env->GetObjectClass(obj);
    // J is the type signature for long:
    return env->GetFieldID(c, "nativePtr", "J");
  }

  template<typename T>
  static T *getPtr(JNIEnv *env, jobject obj) {
    jlong handle = env->GetLongField(obj, getPtrField(env, obj));
    return reinterpret_cast<T *>(handle);
  }

  template<typename T>
  static void setPtr(JNIEnv *env, jobject obj, T *t) {
    jlong handle = reinterpret_cast<jlong>(t);
    env->SetLongField(obj, getPtrField(env, obj), handle);
  }

 protected:

  void initialize(const std::vector<std::string> &opts) {
    string_options_ = opts;
    // added for testing
       //string_options_.push_back("-verbose:jni");
//   string_options_.push_back("-Djava.compiler=NONE");
    jvm_options_ = new JavaVMOption[opts.size()];
    int i = 0;
    for (const auto &opt : string_options_) {
      std::cout << "Adding option " << opt << std::endl;
      jvm_options_[i++].optionString = const_cast<char*>(opt.c_str());
    }

    JavaVMInitArgs vm_args;

    vm_args.version = JNI_VERSION_1_8;
    vm_args.nOptions = opts.size();
    vm_args.options = jvm_options_;
    vm_args.ignoreUnrecognized = JNI_FALSE;
// load and initialize a Java VM, return a JNI interface
// pointer in env
    JNI_CreateJavaVM(&jvm_, (void**) &env_, &vm_args);

        initialized_ = true;
  }

 private:

  std::atomic<bool> initialized_;

  std::map<std::string, JavaClass> objects_;

  std::vector<std::string> string_options_;

  JavaVMOption* jvm_options_;

  JavaVM *jvm_;
  JNIEnv *env_;

  JVMLoader()
      : jvm_options_(nullptr),
        jvm_(nullptr),
        env_(nullptr) {
    initialized_ = false;
  }

  ~JVMLoader() {
    if (jvm_options_) {
      delete[] jvm_options_;
    }
  }
};

#endif /* EXTENSIONS_JVMLOADER_H */
