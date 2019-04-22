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
include_directories(${AWSSDK_INCLUDE_DIR})

file(GLOB SOURCES "*.cpp" "s3/*.cpp" "controllerservices/*.cpp" )

find_package(AWSSDK REQUIRED COMPONENTS transfer s3)

add_library(minifi-aws STATIC ${SOURCES})
#set_property(TARGET minifi-aws PROPERTY POSITION_INDEPENDENT_CODE ON)
message("AWS link libraries@!!!!!!!" ${AWSSDK_LINK_LIBRARIES})
target_link_libraries (minifi-aws ${AWSSDK_LINK_LIBRARIES})

SET (AWS-EXTENSION minifi-aws PARENT_SCOPE)
register_extension(minifi-aws)
