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

#include "JniFlowFile.h"

#include <string>
#include <memory>
#include <algorithm>
#include <iterator>
#include <set>
#include "core/Property.h"
#include "io/validation.h"
#include "core/FlowFile.h"
#include "utils/StringUtils.h"
#include "utils/file/FileUtils.h"
#include "properties/Configure.h"
#include "JVMLoader.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_org_apache_nifi_processor_JniFlowFile_initialise(JNIEnv *env, jobject obj)
{

}

JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getId(JNIEnv *env, jobject obj) {

  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(obj);

    auto ff = ptr->ref_;
  jlong id = 0;
  return id;

}
JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getEntryDate(JNIEnv *env, jobject obj) {
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(obj);

    auto ff = ptr->ref_;
  jlong entryDate = ff->getEntryDate();
  return entryDate;
}
JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getLineageStartDate(JNIEnv *env, jobject obj) {
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(obj);

    auto ff = ptr->ref_;
  jlong val = ff->getlineageStartDate();
  return val;
}
JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getLineageStartIndex(JNIEnv *env, jobject obj) {
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(obj);

    auto ff = ptr->ref_;
  jlong val = ff->getlineageStartDate();
  return val;
}
JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getLastQueueDatePrim(JNIEnv *env, jobject obj) {
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(obj);

    auto ff = ptr->ref_;
  jlong val = 0;
  return val;
}
JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getQueueDateIndex(JNIEnv *env, jobject obj) {
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(obj);

    auto ff = ptr->ref_;
  jlong val = 0;
  return val;
}
JNIEXPORT jboolean JNICALL Java_org_apache_nifi_processor_JniFlowFile_isPenalized(JNIEnv *env, jobject obj) {
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(obj);

    auto ff = ptr->ref_;
  jboolean val = ff->isPenalized();
  return val;
}
JNIEXPORT jstring JNICALL Java_org_apache_nifi_processor_JniFlowFile_getAttribute(JNIEnv *env, jobject obj, jstring key) {
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(obj);

    auto ff = ptr->ref_;
  const char *kstr = env->GetStringUTFChars(key, 0);
  std::string value;
  std::string keystr = kstr;
  ff->getAttribute(keystr,value);
  env->ReleaseStringUTFChars(key, kstr);
  return env->NewStringUTF(value.c_str());
}
JNIEXPORT jlong JNICALL Java_org_apache_nifi_processor_JniFlowFile_getSize(JNIEnv *env, jobject obj) {
  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(obj);
  auto ff = ptr->ref_;
  jlong val = ff->getSize();
  return val;
}
JNIEXPORT jobject JNICALL Java_org_apache_nifi_processor_JniFlowFile_getAttributes(JNIEnv *env, jobject obj) {

  JniFlowFile *ptr = JVMLoader::getInstance()->getReference<JniFlowFile>(obj);

  std::cout << "getattributes" << std::endl;
  auto ff = ptr->ref_;
  jclass mapClass = env->FindClass("java/util/HashMap");
  if (mapClass == NULL) {
    return NULL;
  }

  jsize map_len = ff->getAttributes().size();

  jmethodID init = env->GetMethodID(mapClass, "<init>", "(I)V");
  jobject hashMap = env->NewObject(mapClass, init, map_len);

  jmethodID put = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

  for (auto kf : ff->getAttributes()) {
    env->CallObjectMethod(hashMap, put, env->NewStringUTF(kf.first.c_str()), env->NewStringUTF(kf.second.c_str()));
  }

  return hashMap;
}

#ifdef __cplusplus
}
#endif
