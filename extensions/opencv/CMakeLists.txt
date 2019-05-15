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

file(GLOB SOURCES  "*.cpp")

set(BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/opencv")
set(BYPRODUCT "${CMAKE_CURRENT_BINARY_DIR}/opencv-install/")

# OpenCV 4.1.0
ExternalProject_Add(
        opencv-external
        GIT_REPOSITORY "https://github.com/opencv/opencv.git"
        GIT_TAG "371bba8f54560b374fbcd47e7e02f015ac4969ad"
        EXCLUDE_FROM_ALL TRUE
        INSTALL_DIR ${BYPRODUCT}
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
                    -DCMAKE_INSTALL_PREFIX=${BYPRODUCT}
                    -DBUILD_SHARED_LIBS=ON
                    -DBUILD_EXAMPLES=OFF
                    -DBUILD_DOCS=OFF
                    -DBUILD_PERF_TESTS=OFF
                    -DBUILD_TESTS=OFF
                    -DBUILD_opencv_calib3d=OFF
                    -DBUILD_opencv_core=ON
                    -DBUILD_opencv_dnn=OFF
                    -DBUILD_opencv_features2d=OFF
                    -DBUILD_opencv_flann=OFF
                    -DBUILD_opencv_gapi=OFF
                    -DBUILD_opencv_highgui=OFF
                    -DBUILD_opencv_imgcodecs=ON
                    -DBUILD_opencv_ml=OFF
                    -DBUILD_opencv_objdetect=ON
                    -DBUILD_opencv_photo=OFF
                    -DBUILD_opencv_stitching=OFF
                    -DBUILD_opencv_video=OFF
                    -DBUILD_opencv_videoio=OFF

)

add_library(opencvlib STATIC IMPORTED)
set_target_properties(opencvlib PROPERTIES IMPORTED_LOCATION "${BYPRODUCT}")

set(OPENCV_FOUND "YES" CACHE STRING "" FORCE)
set(OPENCV_INCLUDE_DIRS "${BYPRODUCT}/include/opencv4" CACHE STRING "" FORCE)
set(OPENCV_LIBRARIES opencvlib CACHE STRING "" FORCE)
set(OPENCV_LIBRARY opencvlib CACHE STRING "" FORCE)

include_directories(${OPENCV_INCLUDE_DIRS})
add_library(minifi-opencv STATIC ${SOURCES})

if (APPLE)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_calib3d.4.1.0.dylib)
    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_core.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_dnn.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_features2d.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_flann.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_gapi.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_highgui.4.1.0.dylib)
    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_imgcodecs.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_imgproc.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_ml.4.1.0.dylib)
    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_objdetect.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_photo.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_stitching.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_video.4.1.0.dylib)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib/libopencv_videoio.4.1.0.dylib)
else ()
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_calib3d.so.4.1.0)
    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_core.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_dnn.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_features2d.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_flann.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_gapi.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_highgui.so.4.1.0)
    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_imgcodecs.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_imgproc.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_ml.so.4.1.0)
    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_objdetect.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_photo.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_stitching.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_video.so.4.1.0)
#    target_link_libraries (minifi-opencv ${BYPRODUCT}lib64/libopencv_videoio.so.4.1.0)
endif ()

add_dependencies(minifi-opencv opencv-external)

SET (OPENCV-EXTENSION minifi-opencv PARENT_SCOPE)
register_extension(minifi-opencv)