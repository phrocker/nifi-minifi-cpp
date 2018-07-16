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

#include "core/state/FailurePolicy.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace state {


FAILURE_POLICY from_string(const std::string &policy) {
  if (utils::StringUtils::equalsIgnoreCase(policy, "FAIL"))
    return FAIL;
  else if (utils::StringUtils::equalsIgnoreCase(policy, "RECOVER"))
    return RECOVER;
  else if (utils::StringUtils::equalsIgnoreCase(policy, "RETRY"))
    return RETRY;
  else if (utils::StringUtils::equalsIgnoreCase(policy, "RETRY_RECOVER"))
    return RETRY_RECOVERY;
  else
    return FAIL;
}

} /* namespace state */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
