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

#find_package(OpenCV REQUIRED)
#
#include_directories(${LIBOPENCV_INCLUDE_DIR})

file(GLOB SOURCES  "*.cpp")

set(BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/opencv")
set(BYPRODUCT "${CMAKE_CURRENT_BINARY_DIR}/opencv-install/")

#set_property(TARGET minifi-opencv PROPERTY POSITION_INDEPENDENT_CODE ON)
#if(THREADS_HAVE_PTHREAD_ARG)
#    target_compile_options(PUBLIC minifi-opencv "-pthread")
#endif()
#if(CMAKE_THREAD_LIBS_INIT)
#    target_link_libraries(minifi-opencv "${CMAKE_THREAD_LIBS_INIT}")
#endif()
#
#target_link_libraries( minifi-opencv ${LIBMINIFI} ${OpenCV_LIBRARIES} )


# OpenCV 4.1.0
ExternalProject_Add(
        opencv-external
        GIT_REPOSITORY "https://github.com/opencv/opencv.git"
        GIT_TAG "371bba8f54560b374fbcd47e7e02f015ac4969ad"
        EXCLUDE_FROM_ALL TRUE
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
)

set(OPENCV_FOUND "YES" CACHE STRING "" FORCE)
set(OPENCV_INCLUDE_DIRS "${BYPRODUCT}/include" CACHE STRING "" FORCE)
set(OPENCV_LIBRARIES awssdklib CACHE STRING "" FORCE)
set(OPENCV_LIBRARY awssdklib CACHE STRING "" FORCE)

include_directories(${OPENCV_INCLUDE_DIRS})
add_library(minifi-opencv STATIC ${SOURCES})

#if (APPLE)
#    target_link_libraries (minifi-aws ${BYPRODUCT}lib/libaws-c-event-stream.dylib)
#else ()
#    target_link_libraries (minifi-aws ${BYPRODUCT}lib/libaws-c-event-stream.so)
#endif ()

add_dependencies(minifi-opencv opencv-external)

SET (OPENCV-EXTENSION minifi-opencv PARENT_SCOPE)
register_extension(minifi-opencv)