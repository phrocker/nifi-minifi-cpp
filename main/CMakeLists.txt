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

IF(POLICY CMP0048)
  CMAKE_POLICY(SET CMP0048 OLD)
ENDIF(POLICY CMP0048)

include_directories(../libminifi/include ../thirdparty/cron ../thirdparty/spdlog-20170710/include ../thirdparty/concurrentqueue ../thirdparty/yaml-cpp-yaml-cpp-20171024/include ../thirdparty/rapidjson-1.1.0/include ../thirdparty/)
include_directories(${JEMALLOC_INCLUDE_DIRS})

if(WIN32)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	include_directories(../libminifi/opsys/win)
else()
	include_directories(../libminifi/opsys/posix)
endif()


include(CheckCXXCompilerFlag)
if (WIN32)
  if ((MSVC_VERSION GREATER "1900") OR (MSVC_VERSION EQUAL "1900"))
	    CHECK_CXX_COMPILER_FLAG("/std:c++14" _cpp_latest_flag_supported)
	    if (_cpp_latest_flag_supported)
	        add_compile_options("/std:c++14")
	    endif()
	endif()
else()
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
 message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

endif()



  
add_executable(minifiexe MiNiFiMain.cpp)

if (NOT USE_SHARED_LIBS)
	if (LIBC_STATIC)
		set_target_properties(minifiexe PROPERTIES LINK_SEARCH_START_STATIC 1)
	endif(LIBC_STATIC)
endif(NOT USE_SHARED_LIBS)
#if(THREADS_HAVE_PTHREAD_ARG)
#  target_compile_options(PUBLIC minifiexe "-pthread")
#endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(minifiexe "${CMAKE_THREAD_LIBS_INIT}")
endif()

# Include UUID

set (WIN32_ARCHIVES "")



# Link against minifi, yaml-cpp, civetweb-cpp, uuid, openssl, and rocksdb
#target_link_libraries(minifiexe core-minifi)

if (APPLE)
	target_link_libraries (minifiexe -Wl,-all_load core-minifi)
elseif(NOT WIN32)
	target_link_libraries (minifiexe -Wl,--whole-archive core-minifi -Wl,--no-whole-archive)
else()
	#target_link_libraries (minifiexe core-minifi)
	set(WIN32_ARCHIVES "${WIN32_ARCHIVES} /WHOLEARCHIVE:core-minifi")
#	set_target_properties(minifiexe PROPERTIES LINK_FLAGS "/WHOLEARCHIVE:core-minifi")
endif ()

# Include OpenSSL
if (OPENSSL_FOUND)
	if (NOT USE_SYSTEM_OPENSSL)
		if (APPLE)
			target_link_libraries(minifiexe -Wl,-all_load ${OPENSSL_LIBRARIES})
		elseif(NOT WIN32)
			target_link_libraries(minifiexe -Wl,--whole-archive ${OPENSSL_LIBRARIES} -Wl,--no-whole-archive)
		else()
		target_link_libraries(minifiexe ${OPENSSL_LIBRARIES})
		endif()
	else()
		target_link_libraries(minifiexe ${OPENSSL_LIBRARIES})
	endif()
	include_directories(${OPENSSL_INCLUDE_DIR})
endif(OPENSSL_FOUND)

add_dependencies(minifiexe minifi)


set(BASE_DIR_ZIPPER "${CMAKE_CURRENT_BINARY_DIR}/thirdparty/zipper")
set(DIR_ZIPPER "${BASE_DIR_ZIPPER}/thirdparty/zipper-src")
set(BYPRODUCT_ZIPPER "${DIR_ZIPPER}/libZipper-static.a")

set(BASE_DIR_TINYXML "${CMAKE_CURRENT_BINARY_DIR}/thirdparty/tinyxml")
set(DIR_TINYXML "${BASE_DIR_TINYXML}/thirdparty/tinyxml-src")
set(BYPRODUCT_TINYXML "${DIR_TINYXML}/libtinyxml2.a")

ExternalProject_Add(
	    tinyxml-external
	    GIT_REPOSITORY "https://github.com/leethomason/tinyxml2.git"
	    GIT_TAG "7.0.1" 
	    PREFIX "${BASE_DIR_TINYXML}/thirdparty/tinyxml"
	    BUILD_IN_SOURCE true
	    SOURCE_DIR "${DIR_TINYXML}"
	    BUILD_BYPRODUCTS ${BYPRODUCT_TINYXML} 
	     CMAKE_ARGS ${PASSTHROUGH_CMAKE_ARGS}
	     	   "-DBUILD_SHARED_LIBS=OFF"
	     	   "-DBUILD_STATIC_LIBS=ON"
               "-DCMAKE_INSTALL_PREFIX=${DIR_TINYXML}"
	  )
	  
  include_directories("${DIR_TINYXML}")
	  
  add_library(tinyxml STATIC IMPORTED)


ExternalProject_Add(
	    zipper-external
	    GIT_REPOSITORY "https://github.com/sebastiandev/zipper.git"
	    GIT_TAG "3b0ca4e18caa04d61434c23716d410e581ffd1b5" 
	    PREFIX "${BASE_DIR_ZIPPER}/thirdparty/zipper"
	    BUILD_IN_SOURCE true
	    SOURCE_DIR "${DIR_ZIPPER}"
	    BUILD_BYPRODUCTS ${BYPRODUCT_ZIPPER} 
	     CMAKE_ARGS ${PASSTHROUGH_CMAKE_ARGS}
	     	   "-DMZ_WZAES=OFF"
	     	   "-DMZ_PKCRYPT=OFF"
	     	   "-DMZ_BZIP2=OFF"
	     	   "-DMZ_LZMA=OFF"
	     	   "-DZLIB_FOUND=${ZLIB_FOUND}"
			   "-DZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}"
			   "-DZLIB_INCLUDE_DIRS=${ZLIB_INCLUDE_DIRS}"
			   "-DZLIB_LIBRARIES=${ZLIB_LIBRARIES}"
			   "-DZLIB_LIBRARY=${ZLIB_LIBRARY}"
			   "-DZLIB_LIBRARY_RELEASE=${ZLIB_LIBRARY_RELEASE}"
               "-DCMAKE_INSTALL_PREFIX=${DIR_ZIPPER}"
	  )
	  
  include_directories("${DIR_ZIPPER}/include")
	  
  add_library(zipper STATIC IMPORTED)

include_directories(../thirdparty/cxxopts/include)

add_executable(bootstrap Bootstrap.cpp)
set_target_properties(bootstrap PROPERTIES LINK_SEARCH_START_STATIC 1)


find_package(ZLIB REQUIRED)


set_target_properties(zipper PROPERTIES IMPORTED_LOCATION "${BYPRODUCT_ZIPPER}")
set_target_properties(tinyxml PROPERTIES IMPORTED_LOCATION "${BYPRODUCT_TINYXML}")
add_dependencies(bootstrap tinyxml-external)
  add_dependencies(zipper zipper-external)
  add_dependencies(zipper zlib-external)
target_link_libraries(bootstrap zipper tinyxml minifi ${ZLIB_LIBRARY})

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")


if (APPLE)
	target_link_libraries(bootstrap -static-libstdc++)
elseif(NOT WIN32)
	target_link_libraries(bootstrap -static-libxgcc -static-libstdc++)
endif()
set_target_properties(bootstrap PROPERTIES LINK_SEARCH_END_STATIC 1)

if(WIN32)
	set_target_properties(bootstrap PROPERTIES LINK_FLAGS "${LINK_FLAGS} ${WIN32_ARCHIVES}")
endif()
set_target_properties(bootstrap
        PROPERTIES OUTPUT_NAME bootstrap)

add_dependencies(bootstrap minifiexe)


if (APPLE)
	target_link_libraries (minifiexe -Wl,-all_load minifi)
elseif(NOT WIN32)
	target_link_libraries (minifiexe -Wl,--whole-archive minifi -Wl,--no-whole-archive)
else()
	target_link_libraries (minifiexe minifi)
	set(WIN32_ARCHIVES "${WIN32_ARCHIVES} /WHOLEARCHIVE:minifi")
	#set_target_properties(minifiexe PROPERTIES LINK_FLAGS "${LINK_FLAGS} /WHOLEARCHIVE:minifi")
endif ()

target_link_libraries(minifiexe zipper ${ZLIB_LIBRARY})
target_link_libraries(minifiexe yaml-cpp ${UUID_LIBRARIES} ${JEMALLOC_LIBRARIES}) #

if (WIN32)
	include_directories("../thirdparty/Simple-Windows-Posix-Semaphore")
  	target_link_libraries(minifiexe semaphore)
endif()

if (NOT USE_SHARED_LIBS)
	if (LIBC_STATIC)
		if (APPLE)
		target_link_libraries(minifiexe -static-libstdc++)
		elseif(NOT WIN32)
		target_link_libraries(minifiexe -static-libxgcc -static-libstdc++)
		endif()
		set_target_properties(minifiexe PROPERTIES LINK_SEARCH_END_STATIC 1)
	endif(LIBC_STATIC)
endif(NOT USE_SHARED_LIBS)

if (APPLE)
	get_property(extensions GLOBAL PROPERTY EXTENSION-OPTIONS)
	foreach(EXTENSION ${extensions})
		message("Linking MiNiFiMain against ${EXTENSION}")
		target_link_libraries (minifiexe -Wl,-all_load ${EXTENSION})
		target_link_libraries (bootstrap -Wl,-all_load ${EXTENSION})
		add_dependencies(minifiexe ${EXTENSION})
		add_dependencies(bootstrap ${EXTENSION})
	endforeach()    
else ()
	get_property(extensions GLOBAL PROPERTY EXTENSION-OPTIONS)
	foreach(EXTENSION ${extensions})
	if (WIN32)
	target_link_libraries (minifiexe ${EXTENSION})
	target_link_libraries (bootstrap ${EXTENSION})
	  set(WIN32_ARCHIVES "${WIN32_ARCHIVES} /WHOLEARCHIVE:${EXTENSION}")
	  add_dependencies(minifiexe ${EXTENSION})
	  add_dependencies(bootstrap ${EXTENSION})
    else()
	  target_link_libraries (minifiexe -Wl,--whole-archive ${EXTENSION} -Wl,--no-whole-archive)
	  target_link_libraries (bootstrap -Wl,--whole-archive ${EXTENSION} -Wl,--no-whole-archive)
	  add_dependencies(minifiexe ${EXTENSION})
	  add_dependencies(bootstrap ${EXTENSION})
	  endif()
	endforeach()
endif ()

if(WIN32)
	set_target_properties(minifiexe PROPERTIES LINK_FLAGS "${LINK_FLAGS} ${WIN32_ARCHIVES}")
	set_target_properties(bootstrap PROPERTIES LINK_FLAGS "${LINK_FLAGS} ${WIN32_ARCHIVES}")
endif()
set_target_properties(minifiexe
        PROPERTIES OUTPUT_NAME minifi)

               

if (NOT WIN32)
add_custom_command(TARGET minifiexe POST_BUILD
           COMMAND cat ${CMAKE_BINARY_DIR}/all.log)
else()
#add_custom_command(TARGET minifiexe POST_BUILD
#           COMMAND type ${CMAKE_BINARY_DIR}/all.log)
endif()
