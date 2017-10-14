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


# Find module for Sharkbite library and includes
#  SHARKBITE_FOUND - if system found SHARKBITE library
#  SHARKBITE_INCLUDE_DIRS - The SHARKBITE include directories
#  SHARKBITE_LIBRARIES - The libraries needed to use SHARKBITE
#  SHARKBITE_DEFINITIONS - Compiler switches required for using SHARKBITE

# For OS X do not attempt to use the OS X application frameworks or bundles.
set (CMAKE_FIND_FRAMEWORK NEVER)
set (CMAKE_FIND_APPBUNDLE NEVER)

find_path(SHARKBITE_INCLUDE_DIR
    NAMES interconnect/Connector.h
    PATHS /usr/local/include /usr/include
    DOC "Sharkbite include header"
)

find_library(SHARKBITE_LIBRARY 
    NAMES libsharkbite-shared.dylib libsharkbite-shared.so
    PATHS /usr/local/lib /usr/lib/x86_64-linux-gnu
    DOC "Sharkbite library"
)
	
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SHARKBITE DEFAULT_MSG SHARKBITE_INCLUDE_DIR SHARKBITE_LIBRARY)

if (SHARKBITE_FOUND)
    set(SHARKBITE_LIBRARIES ${SHARKBITE_LIBRARY} )
    set(SHARKBITE_INCLUDE_DIRS ${SHARKBITE_INCLUDE_DIR} )
    set(SHARKBITE_DEFINITIONS )
endif()

mark_as_advanced(SHARKBITE_ROOT_DIR SHARKBITE_INCLUDE_DIR SHARKBITE_LIBRARY)
