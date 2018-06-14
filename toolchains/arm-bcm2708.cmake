# Licensed to the Apache Software Foundation (ASF) under one
#
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
set(CMAKE_SYSTEM_NAME Linux)

SET(CMAKE_C_COMPILER /home/marc/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER /home/marc/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-g++)
SET(CMAKE_FIND_ROOT_PATH /home/marc/x-tools/arm-unknown-linux-gnueabi/)
#SET(CMAKE_ASM_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)

SET(CMAKE_SYSTEM_PROCESSOR arm)


# arm11
#add_definitions("-mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -marm")

# search for programs in the build host directories
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
#SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
