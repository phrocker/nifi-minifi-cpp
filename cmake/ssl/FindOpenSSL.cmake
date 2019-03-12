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

# Dummy OpenSSL find for when we use bundled version

set(OPENSSL_FOUND "YES" CACHE STRING "" FORCE)
set(OPENSSL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libressl/include" CACHE STRING "" FORCE)
set(OPENSSL_LIBRARIES "${CMAKE_BINARY_DIR}/thirdparty/libressl-install/lib/libtls.a" "${CMAKE_BINARY_DIR}/thirdparty/libressl-install/lib/libssl.a" "${CMAKE_BINARY_DIR}/thirdparty/libressl-install/lib/libcrypto.a" CACHE STRING "" FORCE)