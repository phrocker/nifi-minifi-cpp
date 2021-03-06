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

if(NOT DISABLE_CURL)
    file(GLOB CIVETWEB_INTEGRATION_TESTS "*.cpp")
    SET(CIVETWEB-EXTENSIONS_TEST_COUNT 0)
    FOREACH(testfile ${CIVETWEB_INTEGRATION_TESTS})
        get_filename_component(testfilename "${testfile}" NAME_WE)
        add_executable("${testfilename}" "${testfile}")
        target_include_directories(${testfilename} PRIVATE BEFORE "${CMAKE_SOURCE_DIR}/libminifi/test/")
        target_include_directories(${testfilename} PRIVATE BEFORE "${CMAKE_SOURCE_DIR}/extensions/civetweb")
        target_include_directories(${testfilename} PRIVATE BEFORE "${CMAKE_SOURCE_DIR}/extensions/standard-processors")
        target_include_directories(${testfilename} PRIVATE BEFORE "${CMAKE_SOURCE_DIR}/extensions/http-curl")
        target_include_directories(${testfilename} PRIVATE BEFORE "${CMAKE_SOURCE_DIR}/thirdparty/civetweb-1.10/include")
        target_include_directories(${testfilename} PRIVATE BEFORE ${CURL_INCLUDE_DIRS})

        if (APPLE)
            target_link_libraries (${testfilename} -Wl,-all_load ${ZLIB_LIBRARIES} ${OPENSSL_LIBRARIES} minifi-civet-extensions minifi-http-curl minifi-standard-processors)
        elseif (WIN32)
            target_link_libraries (${testfilename} ${ZLIB_LIBRARY} ${OPENSSL_LIBRARIES} minifi-civet-extensions minifi-http-curl minifi-standard-processors)
            set_target_properties (${testfilename} PROPERTIES LINK_FLAGS "${LINK_FLAGS} /WHOLEARCHIVE:minifi-civet-extensions /WHOLEARCHIVE:minifi-http-curl /WHOLEARCHIVE:minifi-standard-processors")
        else ()
            target_link_libraries (${testfilename} -Wl,--whole-archive ${ZLIB_LIBRARIES} ${OPENSSL_LIBRARIES} minifi-civet-extensions minifi-http-curl minifi-standard-processors -Wl,--no-whole-archive)
        endif ()

        createTests("${testfilename}")
        target_link_libraries(${testfilename} ${CATCH_MAIN_LIB})
        MATH(EXPR CIVETWEB-EXTENSIONS_TEST_COUNT "${CIVETWEB-EXTENSIONS_TEST_COUNT}+1")
        add_test(NAME "${testfilename}" COMMAND "${testfilename}" WORKING_DIRECTORY ${TEST_DIR})
        # Copy test resources
                add_custom_command(
                        TARGET "${testfilename}"
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory
                                "${CMAKE_SOURCE_DIR}/extensions/civetweb/tests/resources"
                                "$<TARGET_FILE_DIR:${testfilename}>/resources"
                        )
    ENDFOREACH()
    message("-- Finished building ${CIVETWEB-EXTENSIONS_TEST_COUNT} civetweb related test file(s)...")
endif()
