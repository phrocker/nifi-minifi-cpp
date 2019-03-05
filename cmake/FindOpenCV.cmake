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

# Find module for OpenCV library and includes
#  LIBOPENCV_FOUND - if system found LIBOPENCV library
#  LIBOPENCV_INCLUDE_DIRS - The LIBOPENCV include directories
#  LIBOPENCV_LIBRARIES - The libraries needed to use LIBOPENCV
#  LIBOPENCV_DEFINITIONS - Compiler switches required for using LIBOPENCV

# For OS X do not attempt to use the OS X application frameworks or bundles.
set (CMAKE_FIND_FRAMEWORK NEVER)
set (CMAKE_FIND_APPBUNDLE NEVER)

find_path(LIBOPENCV_INCLUDE_DIR
        NAMES opencv2/opencv.hpp
        PATHS /usr/local/Cellar/opencv/4.0.1/include/opencv4
        DOC "LIBOPENCV include header"
        )

find_library(LIBOPENCV_LIBRARY
        NAMES libopencv_core.dylib
        PATHS /usr/local/Cellar/opencv/4.0.1/include/lib
        DOC "LIBOPENCV library"
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBOPENCV DEFAULT_MSG LIBOPENCV_INCLUDE_DIR LIBOPENCV_LIBRARY)

if (LIBOPENCV_FOUND)
    message("?????????????????????????? OPENCV FOUND ???????????????????????????????")
    set(LIBOPENCV_LIBRARIES ${LIBOPENCV_LIBRARY} )
    set(LIBOPENCV_INCLUDE_DIRS ${LIBOPENCV_INCLUDE_DIR} )
    set(LIBOPENCV_DEFINITIONS )
endif()

mark_as_advanced(LIBOPENCV_ROOT_DIR LIBOPENCV_INCLUDE_DIR LIBOPENCV_LIBRARY)