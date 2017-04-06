/**
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef LIBMINIFI_INCLUDE_CONTROLLERS_SSLCONTEXTSERVICE_H_
#define LIBMINIFI_INCLUDE_CONTROLLERS_SSLCONTEXTSERVICE_H_

#include "core/controller/ControllerService.h"
#include "core/core.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace controllers {

/*
const char *Configure::nifi_security_need_ClientAuth = "nifi.security.need.ClientAuth";
const char *Configure::nifi_security_client_certificate = "nifi.security.client.certificate";
const char *Configure::nifi_security_client_private_key = "nifi.security.client.private.key";
const char *Configure::nifi_security_client_pass_phrase = "nifi.security.client.pass.phrase";
const char *Configure::nifi_security_client_ca_certificate = "nifi.security.client.ca.certificate";
*/
class SSLContextService : public core::controller::ControllerService {


  SSL*  *createSSLContext() {
      return 0;
    }


  virtual void initialize(){

  }

};

} /* namespace controllers */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CONTROLLERS_SSLCONTEXTSERVICE_H_ */
