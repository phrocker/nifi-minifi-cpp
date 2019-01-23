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

#include "JniProcessSession.h"

#include <string>
#include <memory>
#include <algorithm>
#include <iterator>
#include <set>
#include "core/Property.h"
#include "io/validation.h"
#include "utils/StringUtils.h"
#include "utils/file/FileUtils.h"
#include "properties/Configure.h"
#include "JVMLoader.h"

#include "core/Processor.h"
#include "core/ProcessSession.h"
//#include "JavaControllerService.h"
#include "JniFlowFile.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_org_apache_nifi_processor_JniProcessSession_initialise(JNIEnv *env, jobject obj)
{

}

JNIEXPORT jobject JNICALL Java_org_apache_nifi_processor_JniProcessSession_create(JNIEnv *env, jobject obj) {
  core::ProcessSession *session = JVMLoader::getPtr<core::ProcessSession>(env, obj);

  auto ff = JVMLoader::getInstance()->load_class("org/apache/nifi/processor/JniFlowFile", env);
  /*
   JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getId(JNIEnv *env, jobject obj);
   JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getEntryDate(JNIEnv *env, jobject obj);
   JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getLineageStartDate(JNIEnv *env, jobject obj);
   JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getLineageStartIndex(JNIEnv *env, jobject obj);
   JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getLastQueueDatePrim(JNIEnv *env, jobject obj);
   JNIEXPORT jboolean JNICALL Java_org_apache_nifi_processor_JniFlowFile_isPenalized(JNIEnv *env, jobject obj);
   JNIEXPORT jstring JNICALL Java_org_apache_nifi_processor_JniFlowFile_getAttribute(JNIEnv *env, jobject obj, jstring key);
   JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getSize(JNIEnv *env, jobject obj);
   */

  static bool initializedMethods = false;
  if (!initializedMethods) {
    // org.apache.nifi.processor.JniProcessSession.writeOrAppend(Lorg/apache/nifi/flowfile/FlowFile;[B)Z
    static JNINativeMethod methods[] = { { "getAttributes", "()Ljava/util/Map;", reinterpret_cast<void*>(&Java_org_apache_nifi_processor_JniFlowFile_getAttributes) }, { "getAttribute",
        "(Ljava/lang/String;)Ljava/lang/String;", reinterpret_cast<void*>(&Java_org_apache_nifi_processor_JniFlowFile_getAttribute) }, { "getSize", "()J",
        reinterpret_cast<void*>(&Java_org_apache_nifi_processor_JniFlowFile_getSize) }, { "getEntryDate", "()J", reinterpret_cast<void*>(&Java_org_apache_nifi_processor_JniFlowFile_getEntryDate) }, {
        "getLineageStartDate", "()J", reinterpret_cast<void*>(&Java_org_apache_nifi_processor_JniFlowFile_getLineageStartDate) }, { "getLastQueueDatePrim", "()J",
        reinterpret_cast<void*>(&Java_org_apache_nifi_processor_JniFlowFile_getLastQueueDatePrim) }, { "getQueueDateIndex", "()J",
        reinterpret_cast<void*>(&Java_org_apache_nifi_processor_JniFlowFile_getQueueDateIndex) },
               { "getId", "()J", reinterpret_cast<void*>(&Java_org_apache_nifi_processor_JniFlowFile_getId) } };

    ff.registerMethods(methods, sizeof(methods) / sizeof(methods[0]));
    initializedMethods = true;
  }
  auto ff_instance = ff.newInstance();

  if (ff_instance == nullptr) {
    JVMLoader::getInstance()->remove_class("org/apache/nifi/processor/JniFlowFile");
    if (env->ExceptionOccurred()) {
      std::cout << "Exception occurred" << std::endl;
      env->ExceptionDescribe();
      env->ExceptionClear();
    }
    ff = JVMLoader::getInstance()->load_class("org/apache/nifi/processor/JniFlowFile");
    ff_instance = ff.newInstance();
    if (ff_instance == nullptr) {
      std::cout << "Could not create instnace" << std::endl;
      return nullptr;
    }
  }

  auto flow_file = session->create();

  auto flow = new JniFlowFile(flow_file);

  JVMLoader::getInstance()->setReference(ff_instance, ff.getEnv(), flow);

  std::cout << "Created " << (ff_instance == nullptr) << std::endl;
  return ff_instance;
}

JNIEXPORT jbyteArray JNICALL Java_org_apache_nifi_processor_JniProcessSession_readFlowFile(JNIEnv *env, jobject obj, jobject ff){
  core::ProcessSession *session = JVMLoader::getPtr<core::ProcessSession>(env, obj);
    JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(ff);


    if (ptr->ref_) {

      JniByteInputStream callback(ptr->ref_->getSize());

      session->read(ptr->ref_,&callback);

      std::cout << "create array of " << callback.buffer_size_ << std::endl;
      jbyte *bytes = new jbyte[callback.buffer_size_];
      jbyteArray jbytes=env->NewByteArray(callback.buffer_size_);
      std::cout << "create array of " << callback.buffer_size_ << std::endl;
      memcpy(bytes, callback.buffer_, callback.buffer_size_);
      std::cout << "create array of " << callback.buffer_size_ << std::endl;
      env->SetByteArrayRegion(jbytes,0,callback.buffer_size_,bytes);
      std::cout << "create array of " << callback.buffer_size_ << std::endl;
      return jbytes;
    }

    return nullptr;

}

JNIEXPORT jboolean JNICALL Java_org_apache_nifi_processor_JniProcessSession_write(JNIEnv *env, jobject obj, jobject ff, jbyteArray byteArray) {
  core::ProcessSession *session = JVMLoader::getPtr<core::ProcessSession>(env, obj);
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(ff);

  if (ptr->ref_) {
    jbyte* buffer = env->GetByteArrayElements(byteArray, 0);
    jsize length = env->GetArrayLength(byteArray);

    JniByteOutStream outStream(buffer, (size_t) length);
    session->write(ptr->ref_, &outStream);

    env->ReleaseByteArrayElements(byteArray, buffer, 0);
    // finished

    return true;
  }

  return false;

}

JNIEXPORT jobject JNICALL Java_org_apache_nifi_processor_JniProcessSession_get(JNIEnv *env, jobject obj) {
  core::ProcessSession *session = JVMLoader::getPtr<core::ProcessSession>(env, obj);

  auto ff = JVMLoader::getInstance()->load_class("org/apache/nifi/processor/JniFlowFile");

  auto ff_instance = ff.newInstance(env);

  auto flow_file = session->get();

  auto flow = new JniFlowFile(flow_file);

  JVMLoader::getInstance()->setReference(ff_instance, env, flow);

  return ff_instance;
}

JNIEXPORT jobject JNICALL Java_org_apache_nifi_processor_JniProcessSession_putAttribute(JNIEnv *env, jobject obj, jobject ff, jstring key, jstring value) {

  core::ProcessSession *session = JVMLoader::getPtr<core::ProcessSession>(env, obj);
  std::cout << "transfer!2 " << (ff == nullptr) << std::endl;
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(ff);
  std::cout << "transfer!3" << (ptr == nullptr) << std::endl;
  core::Relationship success("success", "description");

  const char *kstr = env->GetStringUTFChars(key, 0);
  const char *vstr = env->GetStringUTFChars(value, 0);
  std::string valuestr = vstr;
  std::string keystr = kstr;

  ptr->ref_->addAttribute(keystr, valuestr);
  env->ReleaseStringUTFChars(key, kstr);
  env->ReleaseStringUTFChars(value, vstr);
  return ff;

}

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProcessSession_transfer(JNIEnv *env, jobject obj, jobject ff, jstring relationship) {
  std::cout << "transfer!" << std::endl;
  if (ff == nullptr) {
    std::cout << "no flowfile" << std::endl;
    return;
  }
  core::ProcessSession *session = JVMLoader::getPtr<core::ProcessSession>(env, obj);
  std::cout << "transfer!2 " << (ff == nullptr) << std::endl;
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(ff);
  std::cout << "transfer!3" << (ptr == nullptr) << std::endl;
  const char *relstr = env->GetStringUTFChars(relationship, 0);
  std::string relString = relstr;
  core::Relationship success(relString, "description");
  session->transfer(ptr->ref_, success);
  delete ptr;
  env->ReleaseStringUTFChars(relationship, relstr);
  std::cout << "transfer!" << std::endl;
}

JNIEXPORT jstring JNICALL
Java_org_apache_nifi_processor_JniProcessSession_getPropertyValue(JNIEnv *env, jobject obj, jstring propertyName)
{
  core::ProcessContext *context = JVMLoader::getPtr<core::ProcessContext>(env,obj);
  const char *kstr = env->GetStringUTFChars(propertyName, 0);
  std::string value;
  std::string keystr = kstr;
  if (!context->getProperty(keystr,value) ) {
    context->getDynamicProperty(keystr,value);
  }
  env->ReleaseStringUTFChars(propertyName, kstr);
  return env->NewStringUTF(value.c_str());
}
#ifdef __cplusplus
}
#endif
