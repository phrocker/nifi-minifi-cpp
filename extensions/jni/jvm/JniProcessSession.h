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
#include <memory>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <jni.h>
#include "io/BaseStream.h"
#include "FlowFileRecord.h"

class JniByteOutStream : public minifi::OutputStreamCallback {
 public:
  JniByteOutStream(jbyte *bytes, size_t length) : bytes_(bytes),length_(length) {

  }

  virtual ~JniByteOutStream() {

  }
  virtual int64_t process(std::shared_ptr<minifi::io::BaseStream> stream) {
    return stream->write((uint8_t*)bytes_,length_);
  }
 private:
  jbyte *bytes_;
  size_t length_;
};

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProcessSession_initialise(JNIEnv *env, jobject obj);

JNIEXPORT jobject JNICALL Java_org_apache_nifi_processor_JniProcessSession_get(JNIEnv *env, jobject obj);

JNIEXPORT jobject JNICALL Java_org_apache_nifi_processor_JniProcessSession_create(JNIEnv *env, jobject obj);

JNIEXPORT jboolean JNICALL Java_org_apache_nifi_processor_JniProcessSession_write(JNIEnv *env, jobject obj, jobject ff, jbyteArray byteArray);

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProcessSession_transfer(JNIEnv *env, jobject obj, jobject ff, jstring relationship);

JNIEXPORT jobject JNICALL Java_org_apache_nifi_processor_JniProcessSession_putAtttribute(JNIEnv *env, jobject obj, jobject ff, jstring key, jstring value);

#ifdef __cplusplus
}
#endif

#endif /* EXTENSIONS_JNIPROCESSSESSION_H */
