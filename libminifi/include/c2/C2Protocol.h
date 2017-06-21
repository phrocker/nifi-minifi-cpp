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
#ifndef LIBMINIFI_INCLUDE_C2_C2PROTOCOL_H_
#define LIBMINIFI_INCLUDE_C2_C2PROTOCOL_H_

#include "C2Payload.h"
#include "core/controller/ControllerServiceProvider.h"
#include "properties/Configure.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace c2 {

/**
 * Defines a protocol to perform state management of the minifi agent.
 */
class C2Protocol {
 public:

  C2Protocol(const std::shared_ptr<core::controller::ControllerServiceProvider> &controller,
             const std::shared_ptr<Configure> &configure)
      : controller_(controller),
        configuration_(configure) {

  }
  virtual ~C2Protocol() {

  }

  /**
   * Send a C2 payload to the provided URI. The direction indicates to the protocol whether or not this a transmit or receive operatin.
   * Depending on the protocol this may mean different things.
   *
   * @param url url.
   * @param operation payload to perform and/or send
   * @param direction direction of the C2 operation.
   * @param async whether or not this is an asynchronous operation
   * @return payload from the response or a response to come back in the face of an asynchronous operation.
   */
  virtual C2Payload send(const std::string &url, const C2Payload &operation, Direction direction = TRANSMIT, bool async = false) = 0;

  /**
   * Send a C2 payload . The direction indicates to the protocol whether or not this a transmit or receive operatin.
   * Depending on the protocol this may mean different things.
   *
   * @param operation payload to perform and/or send
   * @param direction direction of the C2 operation.
   * @param async whether or not this is an asynchronous operation
   * @return payload from the response or a response to come back in the face of an asynchronous operation.
   */
  virtual C2Payload send(const C2Payload &operation, Direction direction = TRANSMIT, bool async = false) = 0;

 protected:
  std::shared_ptr<core::controller::ControllerServiceProvider> controller_;

  std::shared_ptr<Configure> configuration_;
};

} /* namesapce c2 */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_C2_C2PROTOCOL_H_ */
