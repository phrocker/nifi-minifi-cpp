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
#ifndef LIBMINIFI_INCLUDE_CORE_STATE_FAILUREPOLICY_H_
#define LIBMINIFI_INCLUDE_CORE_STATE_FAILUREPOLICY_H_

#include "utils/StringUtils.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {

/**
 * Failure Policies can have different meanings for different components.
 * AS a result, they must adhere to the base meaning.
 *
 *  FAILURE means that that the component fails
 *  RECOVER means that the component attempts recovery and fails if another failure occurs
 *  RETRY_RECOVERY means that the component continually attempts recovery.
 *
 */
enum FAILURE_POLICY {
  FAIL,
  RECOVER,
  RETRY,
  RETRY_RECOVERY
};

FAILURE_POLICY from_string(const std::string &policy);

} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_STATE_FAILUREPOLICY_H_ */
