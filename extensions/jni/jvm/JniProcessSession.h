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
#ifndef EXTENSIONS_JNIPROCESSSESSION_H
#define EXTENSIONS_JNIPROCESSSESSION_H

#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <jni.h>
#include "core/Processor.h"
#include "core/ProcessSession.h"
#include "JavaControllerService.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProcessSession_initialise(JNIEnv *env, jobject obj);

JNIEXPORT jobject JNICALL Java_org_apache_nifi_processor_JniProcessSession_create(JNIEnv *env, jobject obj);

JNIEXPORT jobject JNICALL Java_org_apache_nifi_processor_JniProcessSession_transfer(JNIEnv *env, jobject obj, jobject ff, jobject relationship);

#ifdef __cplusplus
}
#endif

class JniProcessSession {
 public:
  JniProcessSession(const std::shared_ptr<core::ProcessContext> &ctx, std::shared_ptr<minifi::controllers::JavaControllerService> java_servicer)
      : ctx_(ctx),
        java_servicer_(java_servicer) {

  }
 private:
  std::shared_ptr<core::ProcessContext> ctx_;
  std::shared_ptr<minifi::controllers::JavaControllerService> java_servicer_;
};

#endif /* EXTENSIONS_JNIPROCESSSESSION_H */
