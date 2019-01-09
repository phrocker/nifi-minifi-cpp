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
#ifndef WIN32
#include <dlfcn.h>
#endif
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

  JNIEnv *attach(const std::string &name = "") {
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

    return jenv;
  }

  jobject getClassLoader() {
    return gClassLoader;
  }

  JavaClass load_class(const std::string &name) {
    auto finder = objects_.find(name);
    if (finder != objects_.end()) {
      return finder->second;
    }
    auto env = attach(name);
    std::cout << "call " << (gClassLoader == nullptr) << std::endl;

    std::string modifiedName = name;
    modifiedName = utils::StringUtils::replaceAll(modifiedName, "/", ".");

    auto obj = env->CallObjectMethod(gClassLoader, gFindClassMethod, env->NewStringUTF(modifiedName.c_str()));
    std::cout << " res nu ? " << (obj == nullptr) << std::endl;
    auto clazzobj = static_cast<jclass>(obj);

    if (clazzobj == nullptr) {
      std::cout << "nully " << std::endl;
      if (env->ExceptionOccurred()) {
        std::cout << "Exception occurred" << std::endl;
        env->ExceptionDescribe();
        env->ExceptionClear();
      }
    }
    std::cout << "called" << (gClassLoader == nullptr) << std::endl;
    JavaClass clazz(name, clazzobj, env);
    objects_.insert(std::make_pair(name, clazz));
    return clazz;
  }

  /*

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
   */
  JavaClass getObjectClass(const std::string &name, jobject jobj) {
    auto env = attach();
    auto jcls = env->GetObjectClass(jobj);
    return JavaClass(name, jcls, env);
  }

  static JVMLoader *getInstance() {
    static JVMLoader jvm;
    return &jvm;
  }

  JNIEnv *getEnv() {
    return env_;
  }

  static JVMLoader *getInstance(const std::vector<std::string> &pathVector, const std::vector<std::string> &otherOptions = std::vector<std::string>()) {
    JVMLoader *jvm = getInstance();
    if (!jvm->initialized()) {
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
      jvm->initialize(options);
    }
    return jvm;
  }

  template<typename T>
  void setReference(jobject obj, T *t) {
    setPtr(env_, obj, t);
  }

  template<typename T>
  T *getReference(jobject obj) {
    return getPtr<T>(env_, obj);
  }

  template<typename T>
    void setReference(jobject obj,JNIEnv *env, T *t) {
      setPtr(env, obj, t);
    }

    template<typename T>
    T *getReference(JNIEnv *env,jobject obj) {
      return getPtr<T>(env, obj);
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

#ifdef WIN32

  // base_object doesn't have a handle
  std::map< HMODULE, std::string > resource_mapping_;
  std::mutex internal_mutex_;

  std::string error_str_;
  std::string current_error_;

  void store_error() {
    auto error = GetLastError();

    if (error == 0) {
      error_str_ = "";
      return;
    }

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    current_error_ = std::string(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);
  }

  void *dlsym(void *handle, const char *name)
  {
    FARPROC symbol;
    HMODULE hModule;

    symbol = GetProcAddress((HMODULE)handle, name);

    if (symbol == nullptr) {
      store_error();

      for (auto hndl : resource_mapping_)
      {
        symbol = GetProcAddress((HMODULE)hndl.first, name);
        if (symbol != nullptr) {
          break;
        }
      }
    }

#ifdef _MSC_VER
#pragma warning( suppress: 4054 )
#endif
    return (void*)symbol;
  }

  const char *dlerror(void)
  {
    std::lock_guard<std::mutex> lock(internal_mutex_);

    error_str_ = current_error_;

    current_error_ = "";

    return error_str_.c_str();
  }

  void *dlopen(const char *file, int mode) {
    std::lock_guard<std::mutex> lock(internal_mutex_);
    HMODULE object;
    char * current_error = NULL;
    uint32_t uMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    if (nullptr == file)
    {
      HMODULE allModules[1024];
      HANDLE current_process_id = GetCurrentProcess();
      DWORD cbNeeded;
      object = GetModuleHandle(NULL);

      if (!object)
      store_error();
      if (EnumProcessModules(current_process_id, allModules,
              sizeof(allModules), &cbNeeded) != 0)
      {

        for (uint32_t i = 0; i < cbNeeded / sizeof(HMODULE); i++)
        {
          TCHAR szModName[MAX_PATH];

          // Get the full path to the module's file.
          resource_mapping_.insert(std::make_pair(allModules[i], "minifi-system"));
        }
      }
    }
    else
    {
      char lpFileName[MAX_PATH];
      int i;

      for (i = 0; i < sizeof(lpFileName) - 1; i++)
      {
        if (!file[i])
        break;
        else if (file[i] == '/')
        lpFileName[i] = '\\';
        else
        lpFileName[i] = file[i];
      }
      lpFileName[i] = '\0';
      object = LoadLibraryEx(lpFileName, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
      if (!object)
      store_error();
      else if ((mode & RTLD_GLOBAL))
      resource_mapping_.insert(std::make_pair(object, lpFileName));
    }

    /* Return to previous state of the error-mode bit flags. */
    SetErrorMode(uMode);

    return (void *)object;

  }

  int dlclose(void *handle)
  {
    std::lock_guard<std::mutex> lock(internal_mutex_);

    HMODULE object = (HMODULE)handle;
    BOOL ret;

    current_error_ = "";
    ret = FreeLibrary(object);

    resource_mapping_.erase(object);

    ret = !ret;

    return (int)ret;
  }

#endif

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

    auto randomClass = env_->FindClass("org/apache/nifi/processor/ProcessContext");
    jclass classClass = env_->GetObjectClass(randomClass);
    auto classLoaderClass = env_->FindClass("java/lang/ClassLoader");
    auto getClassLoaderMethod = env_->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    gClassLoader = env_->CallObjectMethod(randomClass, getClassLoaderMethod);
    gFindClassMethod = env_->GetMethodID(classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    std::cout << "gClassLoader " << (gClassLoader == nullptr) << std::endl;
    std::cout << "gFindClassMethod " << (gFindClassMethod == nullptr) << std::endl;
    if (env_->ExceptionOccurred()) {
      std::cout << "Exception occurred" << std::endl;
      env_->ExceptionDescribe();
      env_->ExceptionClear();
    }
    initialized_ = true;
  }

 private:

  std::atomic<bool> initialized_;

  std::map<std::string, JavaClass> objects_;

  std::vector<std::string> string_options_;

  JavaVMOption* jvm_options_;

  JavaVM *jvm_;
  JNIEnv *env_;

  jobject gClassLoader;
  jmethodID gFindClassMethod;

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
