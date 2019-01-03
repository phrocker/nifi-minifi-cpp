#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#

include(${CMAKE_SOURCE_DIR}/extensions/ExtensionHeader.txt) 

find_package(JNI REQUIRED)


include_directories(${JNI_INCLUDE_DIRS})

file(GLOB SOURCES  "jvm/*.cpp" "*.cpp")

add_library(minifi-jni STATIC ${SOURCES})
set_property(TARGET minifi-jni PROPERTY POSITION_INDEPENDENT_CODE ON)

if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(minifi-jni "${CMAKE_THREAD_LIBS_INIT}")
endif()

target_link_libraries(minifi-jni ${LIBMINIFI} ${JAVA_JVM_LIBRARY})

SET (JNI-EXTENSION minifi-jni PARENT_SCOPE)
register_extension(minifi-jni)

