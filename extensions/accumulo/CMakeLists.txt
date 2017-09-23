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

cmake_minimum_required(VERSION 2.6)

find_package(Sharkbite REQUIRED)
find_package(Thrift REQUIRED)
find_package(Zookeeper REQUIRED)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--export-all-symbols")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-symbols")
set(CMAKE_CXX_VISIBILITY_PRESET default)
include_directories("/usr/local/include/thrift")

include_directories(../../libminifi/include ../../libminifi/include/c2  ../../libminifi/include/c2/protocols/  ../../libminifi/include/core/state ./libminifi/include/core/statemanagement/metrics  ../../libminifi/include/core/yaml  ../../libminifi/include/core  ../../thirdparty/spdlog-20170710/include ../../thirdparty/concurrentqueue ../../thirdparty/yaml-cpp-yaml-cpp-0.5.3/include ../../thirdparty/civetweb-1.9.1/include ../../thirdparty/jsoncpp/include ../../thirdparty/leveldb-1.18/include ../../thirdparty/)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB SOURCES  "*.cpp")


add_library(minifi-accumulo STATIC ${SOURCES})
set_property(TARGET minifi-accumulo PROPERTY POSITION_INDEPENDENT_CODE ON)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(PUBLIC minifiexe "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(minifi-accumulo "${CMAKE_THREAD_LIBS_INIT}")
endif()

# Include UUID
find_package(UUID REQUIRED)
#set(LINK_FLAGS ${LINK_FLAGS} "-Wl,-whole-archive")
# Include OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
# yaml-cpp c-library civetweb-cpp ${JSON_CPP_LIB} ${SHARKBITE_LIBRARY} ${UUID_LIBRARIES} ${LEVELDB_LIBRARIES} ${OPENSSL_LIBRARIES}
target_link_libraries(minifi-accumulo ${CMAKE_DL_LIBS} )#minifi c-library civetweb-cpp yaml-cpp ${JSON_CPP_LIB} ${SHARKBITE_LIBRARY} ${UUID_LIBRARIES} ${LEVELDB_LIBRARIES} ${OPENSSL_LIBRARIES})
target_link_libraries(minifi-accumulo ${SHARKBITE_LIBRARY})
target_link_libraries(minifi-accumulo ${THRIFT_STATIC_LIB})
target_link_libraries(minifi-accumulo ${Zookeeper_LIBRARIES})
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries (minifi-accumulo ${ZLIB_LIBRARIES})
if (WIN32)
    set_target_properties(minifi-accumulo PROPERTIES
        LINK_FLAGS "/WHOLEARCHIVE"
    )
elseif (APPLE)
    set_target_properties(minifi-accumulo PROPERTIES
        LINK_FLAGS "-Wl,-all_load"
    )
else ()
    set_target_properties(minifi-accumulo PROPERTIES
        LINK_FLAGS "-Wl,--whole-archive"
    )
endif ()
#add_library(minifi-accumulo-shared SHARED minifi-accumulo)

