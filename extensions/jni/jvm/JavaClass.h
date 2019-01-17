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
#ifndef EXTENSIONS_JAVACLASS_H
#define EXTENSIONS_JAVACLASS_H

#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <jni.h>
#include "JniProcessContext.h"
#include "JniFlowFile.h"
#include "JniProcessSession.h"

class JavaClass {

 public:

  JavaClass()
      : class_ref_(nullptr),
        jenv_(nullptr) {

  }

  JavaClass(const JavaClass &ref) = default;

  JavaClass(const std::string &name, jclass classref, JNIEnv* jenv)
      : name_(name),
        jenv_(jenv) {
    class_ref_ = (jclass)jenv_->NewGlobalRef(classref);
    cnstrctr = jenv_->GetMethodID(class_ref_, "<init>", "()V");

  }

  jclass getReference() {
    return class_ref_;
  }

  JNIEnv *getEnv(){
    return jenv_;
  }

  void setEnv(JNIEnv* jenv){
    jenv_ = jenv;
  }

  JavaClass &operator=(const JavaClass &o) = default;

  /**
   * Call empty constructor
   */
  JNIEXPORT
  jobject newInstance(JNIEnv *env=nullptr) {
    std::string instanceName = "(L" + name_ + ";)V";
    JNIEnv *lenv = env;
    if (lenv == nullptr){
      lenv = jenv_;
    }
    if (cnstrctr == nullptr) {
      printf("Find method Failed. %s\n",name_.c_str());
      return nullptr;
    } else {
      printf("Found method. %s\n",name_.c_str());
    }

    return lenv->NewGlobalRef(lenv->NewObject(class_ref_, cnstrctr));
  }

  JNIEXPORT
    jobject newInstance(const std::string &arg) {
      std::string instanceName = "(L" + name_ + ";)V";
      jmethodID cnstrctsString = jenv_->GetMethodID(class_ref_, "<init>", "(Ljava/util/String;)V");
      if (cnstrctsString == nullptr) {
        printf("Find method Failed 5. %s\n",name_.c_str());
        return nullptr;
      } else {
        printf("Found method. %s\n",name_.c_str());
      }

      auto clazz_name = jenv_->NewStringUTF(arg.c_str());

      return jenv_->NewObject(class_ref_, cnstrctsString, clazz_name);
    }

  jmethodID getClassMethod(JNIEnv *env, const std::string &methodName, const std::string &type) {
      jmethodID mid = env->GetMethodID(class_ref_, methodName.c_str(), type.c_str());
      return mid;
    }

  jmethodID getClassMethod(const std::string &methodName, const std::string &type) {
    jmethodID mid = jenv_->GetMethodID(class_ref_, methodName.c_str(), type.c_str());
    return mid;
  }

  void registerMethods(JNINativeMethod *methods, size_t size){

    JNINativeMethod methods2[] = {
               { "getPropertyValue", "(Ljava/lang/String;)Ljava/lang/String;", reinterpret_cast<void*>(&Java_org_apache_nifi_processor_JniProcessContext_getPropertyValue)}

          };


    jenv_->RegisterNatives(class_ref_,methods,size);
    if (jenv_->ExceptionCheck()) {
      std::cout << "error while registering methods " << std::endl;
      jenv_->ExceptionDescribe();
        }
    else{
      std::cout << "registered methods " << std::endl;
    }

  }

  //clazz.callVoidMethod("onScheduled","(Lorg/apache/nifi/processor/ProcessContext;)V", obj);
  template<typename ... Args>
  void callVoidMethod(jobject obj, const std::string &methodName, const std::string &type, Args ... args) {

    jmethodID method = getClassMethod( methodName, type);
    if (method == nullptr) {
      throw std::runtime_error("cannot run method " + methodName);
    }
    jobject objects[] = { static_cast<jobject>(args)... };
    jenv_->CallVoidMethod(obj, method, std::forward<Args>(args)...);
    if (jenv_->ExceptionOccurred()) {
      jenv_->ExceptionDescribe();
      jenv_->ExceptionClear();
    }
  }

  template<typename ... Args>
    void callVoidMethod(JNIEnv* env, jobject obj, const std::string &methodName, const std::string &type, Args ... args) {

      jmethodID method = getClassMethod(env, methodName, type);
      if (method == nullptr) {
        throw std::runtime_error("cannot run method");
      }
      jobject objects[] = { static_cast<jobject>(args)... };
      env->CallVoidMethod(obj, method, std::forward<Args>(args)...);
      if (env->ExceptionOccurred()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
      }
    }

  /**
   * Call empty constructor

   template<typename T>

   JNIEXPORT jobject newInstance(T* ) {
   std::string instanceName = "(L" + name_ + ";)V";
   jmethodID cnstrctr = jenv_->GetMethodID(class_ref_, "<init>", "()V");
   if (cnstrctr == nullptr) {
   printf("Find method Failed.\n");
   return nullptr;
   } else {
   printf("Found method.\n");
   }

   return jenv_->NewObject(class_ref_, cnstrctr);
   }   */

 private:
  jmethodID cnstrctr;
  std::string name_;
  jclass class_ref_;
  JNIEnv* jenv_;
};

#endif /* EXTENSIONS_JAVACLASS_H */
