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
include_directories(controllerservices)

#find_package(AWSSDK REQUIRED COMPONENTS transfer s3)
#include_directories(${AWSSDK_INCLUDE_DIR})

file(GLOB SOURCES "*.cpp" "s3/*.cpp" "controllerservices/*.cpp" )

set(BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/extensions/aws")
set(BYPRODUCT "${BASE_DIR}/thirdparty/aws-sdk-cpp/.libs/libcoap-2.a")
set(DIR "${BASE_DIR}/thirdparty/aws-sdk-cpp")

# AWS SDK 1.7.89
ExternalProject_Add(
        awssdk-external
        GIT_REPOSITORY "https://github.com/aws/aws-sdk-cpp.git"
        GIT_TAG "09b65deba03cfbef9a1e5d5986aa4de71bc03cd8"
        PREFIX "${BASE_DIR}/thirdparty/aws-sdk-cpp"
        BUILD_IN_SOURCE true
        SOURCE_DIR "${DIR}"
        BUILD_COMMAND make
        CMAKE_COMMAND "cmake .. -D BUILD_ONLY=s3"
        UPDATE_COMMAND ""
        INSTALL_COMMAND ${CMAKE_COMMAND}  -E echo "Skipping install step."
        CONFIGURE_COMMAND ""
        PATCH_COMMAND ""
        STEP_TARGETS build
        EXCLUDE_FROM_ALL TRUE
)

add_library(awssdklib STATIC IMPORTED)
set_target_properties(awssdklib PROPERTIES IMPORTED_LOCATION "${BYPRODUCT}")
add_dependencies(awssdklib awssdk-external)
set(AWSSDK_FOUND "YES" CACHE STRING "" FORCE)
set(AWSSDK_INCLUDE_DIRS "${DIR}/include" CACHE STRING "" FORCE)
set(AWSSDK_LIBRARIES awssdklib CACHE STRING "" FORCE)
set(AWSSDK_LIBRARY awssdklib CACHE STRING "" FORCE)

add_library(minifi-aws STATIC ${SOURCES})
target_link_libraries (minifi-aws ${AWSSDK_LIBRARIES})

SET (AWS-EXTENSION minifi-aws PARENT_SCOPE)
register_extension(minifi-aws)
