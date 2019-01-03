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
#ifndef EXTENSIONS_JNIPROCESSCONTEXT_H
#define EXTENSIONS_JNIPROCESSCONTEXT_H

#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <jni.h>
#include "core/Processor.h"
#include "core/ProcessSession.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_apache_nifi_processor_JniProcessContext_initialise(JNIEnv *env, jobject obj);

JNIEXPORT jstring JNICALL Java_org_apache_nifi_processor_JniProcessContext_getPropertyValue(JNIEnv *env, jobject obj, jstring propertyName);

JNIEXPORT jobject JNICALL Java_org_apache_nifi_processor_JniProcessContext_getAllProperties(JNIEnv *env, jobject obj, jstring propertyName);

#ifdef __cplusplus
}
#endif

class JniProcessContext {
 public:
  JniProcessContext(const std::shared_ptr<core::ProcessContext> &ctx)
      : ctx_(ctx) {

  }
 private:
  std::shared_ptr<core::ProcessContext> ctx_;
};

/*

 private native String getPropertyValue(final String propertyName);

 @Override
 public PropertyValue newPropertyValue(final String rawValue) {
 return new StandardPropertyValue(rawValue, this);
 }

 @Override
 public native void yield();

 @Override
 public ControllerService getControllerService(final String serviceIdentifier) {
 return null;
 }

 @Override
 public native int getMaxConcurrentTasks();

 @Override
 public String getAnnotationData() {
 // TODO IMPLEMENT
 return "";
 }

 @Override
 public Map<PropertyDescriptor, String> getProperties() {
 Map<String,String> properties = getAllProperties();
 Map<PropertyDescriptor, String> conversionMap = new HashMap<>();
 return conversionMap;
 }

 @Override
 public native Map<String, String> getAllProperties();

 @Override
 public String encrypt(final String unencrypted) {
 throw new UnsupportedOperationException();
 }

 @Override
 public String decrypt(final String encrypted) {
 throw new UnsupportedOperationException();
 }

 @Override
 public Set<String> getControllerServiceIdentifiers(final Class<? extends ControllerService> serviceType) {
 throw new UnsupportedOperationException();
 }

 @Override
 public boolean isControllerServiceEnabled(final ControllerService service) {
 return false;
 }

 @Override
 public boolean isControllerServiceEnabled(final String serviceIdentifier) {
 return false;
 }

 @Override
 public boolean isControllerServiceEnabling(final String serviceIdentifier) {
 return false;
 }

 @Override
 public ControllerServiceLookup getControllerServiceLookup() {
 return this;
 }

 private native Map<String,String> getRelationships();

 @Override
 public Set<Relationship> getAvailableRelationships() {

 Map<String,String> relationships = getRelationships();


 final Set<Relationship> set = new HashSet<>();

 return set;
 }

 @Override
 public String getControllerServiceName(final String serviceIdentifier) {
 return "";
 }

 @Override
 public native boolean hasIncomingConnection();

 @Override
 public boolean hasNonLoopConnection() {
 return true;
 }

 @Override
 public boolean hasConnection(final Relationship relationship) {
 return true;
 }

 @Override
 public boolean isExpressionLanguagePresent(final PropertyDescriptor property) {
 return false;
 }

 @Override
 public StateManager getStateManager() {
 return null;
 }

 @Override
 public native String getName();



 */
#endif /* EXTENSIONS_JNIPROCESSCONTEXT_H */
