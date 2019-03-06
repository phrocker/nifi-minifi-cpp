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
#ifndef EXTENSIONS_JNIPROVENANCEREPORTER_H
#define EXTENSIONS_JNIPROVENANCEREPORTER_H

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <jni.h>
#include "io/BaseStream.h"
#include "FlowFileRecord.h"
#include "core/ProcessSession.h"
#include "core/ProcessSessionFactory.h"
#include "core/WeakReference.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace jni {

} /* namespace jni */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProvenanceReporter_receive(JNIEnv *env, jobject obj, jobject ff, jstring transitUri, jstring sourceSystemFlowIdentifier, jstring details, jlong transmissionMillis);

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProvenanceReporter_fetch(JNIEnv *env, jobject obj, jobject ff, jstring transitUri, jstring details, jlong transmissionMillis);

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProvenanceReporter_send(JNIEnv *env, jobject obj, jobject ff, jstring transitUri, jstring details, jlong transmissionMillis, jboolean force);

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProvenanceReporter_fork(JNIEnv *env, jobject obj, jobject parent,jobject children, jstring details, jlong joinDuration);

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProvenanceReporter_join(JNIEnv *env, jobject obj, jobject parents,jobject child, jstring details, jlong joinDuration);

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProvenanceReporter_join(JNIEnv *env, jobject obj, jobject parent,jobject child);

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProvenanceReporter_modifyContent(JNIEnv *env, jobject obj, jobject ff,jstring details, jlong processingMillis);

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProvenanceReporter_route(JNIEnv *env, jobject obj, jobject ff,jobject relationship, jstring details, jlong processingDuration);

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProvenanceReporter_create(JNIEnv *env, jobject obj, jobject ff,jstring details);


#ifdef __cplusplus
}
#endif

#endif /* EXTENSIONS_JNIPROVENANCEREPORTER_H */
