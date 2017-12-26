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

find_package(CURL REQUIRED)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--export-all-symbols")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-symbols")

include_directories(../../libminifi/include  ../http-curl/ ../http-curl/protocols/ ../../libminifi/include/core/yaml  ../../libminifi/include/core  ../../thirdparty/spdlog-20170710/include ../../thirdparty/concurrentqueue ../../thirdparty/RTIMULib/RTIMMLib ${CIVET_THIRDPARTY_ROOT}/include ../../thirdparty/jsoncpp/include  ../../thirdparty/)
include_directories(protocols client processors sitetosite)

file(GLOB SOURCES "*.cpp" "gpio/*.cpp" "monitoring/*.cpp" "sunfounder/*.cpp" "sensehat/*.cpp")

add_library(i2clib STATIC ${SOURCES})

target_link_libraries(i2clib RTIMULib )

SET (I2C-EXTENSIONS i2clib PARENT_SCOPE)

register_extension(i2clib)