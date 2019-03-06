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

#include "JniProvenanceReporter.h"

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
#include "JNIUtil.h"
#include "JniReferenceObjects.h"

#include "core/Processor.h"
#include "JniFlowFile.h"
#include "../JavaException.h"

#ifdef __cplusplus
extern "C" {
#endif

void Java_org_apache_nifi_processor_JniProvenanceReporter_receive(JNIEnv *env, jobject obj, jobject ff, jstring transitUri, jstring sourceSystemFlowIdentifier, jstring details,
                                                                  jlong transmissionMillis) {
  if (obj == nullptr) {
    return nullptr;
  }

  minifi::jni::JniSession *session = minifi::jni::JVMLoader::getPtr<minifi::jni::JniSession>(env, obj);
  minifi::jni::JniFlowFile *ffPtr = minifi::jni::JVMLoader::getInstance()->getReference<minifi::jni::JniFlowFile>(env, ff);
  auto transitUrlStr = minifi::jni::JniStringToUTF(env, transitUri);
  auto sourceSystemFlowIdentifierStr = minifi::jni::JniStringToUTF(env, sourceSystemFlowIdentifier);
  auto detailsStr = minifi::jni::JniStringToUTF(env, details);
  session->getSession()->getProvenanceReporter()->receive(ffPtr->ref_, transitUrlStr, sourceSystemFlowIdentifierStr, detailsStr, transmissionMillis > 0 ? transmissionMillis : 0);
}

void Java_org_apache_nifi_processor_JniProvenanceReporter_fetch(JNIEnv *env, jobject obj, jobject ff, jstring transitUri, jstring details, jlong transmissionMillis){

}

void Java_org_apache_nifi_processor_JniProvenanceReporter_send(JNIEnv *env, jobject obj, jobject ff, jstring transitUri, jstring details, jlong transmissionMillis, jboolean force);

void Java_org_apache_nifi_processor_JniProvenanceReporter_fork(JNIEnv *env, jobject obj, jobject parent, jobject children, jstring details, jlong joinDuration);

void Java_org_apache_nifi_processor_JniProvenanceReporter_join(JNIEnv *env, jobject obj, jobject parents, jobject child, jstring details, jlong joinDuration);

void Java_org_apache_nifi_processor_JniProvenanceReporter_join(JNIEnv *env, jobject obj, jobject parent, jobject child);

void Java_org_apache_nifi_processor_JniProvenanceReporter_modifyContent(JNIEnv *env, jobject obj, jobject ff, jstring details, jlong processingMillis);

void Java_org_apache_nifi_processor_JniProvenanceReporter_route(JNIEnv *env, jobject obj, jobject ff, jobject relationship, jstring details, jlong processingDuration);

void Java_org_apache_nifi_processor_JniProvenanceReporter_create(JNIEnv *env, jobject obj, jobject ff, jstring details);
#ifdef __cplusplus
}
#endif
