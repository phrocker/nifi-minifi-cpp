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

#include "JniProcessContext.h"

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

/*#ifdef __cplusplus
extern "C" {
#endif
*/
JNIEXPORT void JNICALL
Java_org_apache_nifi_processor_JniProcessContext_initialise(JNIEnv *env, jobject obj)
{

}

JNIEXPORT jstring JNICALL
Java_org_apache_nifi_processor_JniProcessContext_getPropertyValue(JNIEnv *env, jobject obj, jstring propertyName)
{
  core::ProcessContext *context = JVMLoader::getPtr<core::ProcessContext>(env,obj);
  const char *kstr = env->GetStringUTFChars(propertyName, 0);
  std::string value;
  std::string keystr = kstr;
  if (!context->getProperty(keystr,value) ){
    if (! context->getDynamicProperty(keystr,value) ){
      return nullptr;
    }
  }
  env->ReleaseStringUTFChars(propertyName, kstr);
  return env->NewStringUTF(value.c_str());
}
/*
#ifdef __cplusplus
}
#endif
*/
