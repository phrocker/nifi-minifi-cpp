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

#include "utils/ClassUtils.h"
#include "utils/StringUtils.h"
#include <iostream>
#include <string>
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace utils {

void ClassUtils::shortenClassName(const std::string &class_name, std::string &out) {
  std::string class_delim = "::";
  auto class_split = utils::StringUtils::split(class_name, class_delim);
  // support . and ::
  if (class_split.size() <= 1 && class_name.find(".") != std::string::npos) {
    class_delim = ".";
    class_split = utils::StringUtils::split(class_name, class_delim);
  }
  for (auto &elem : class_split) {
    if (&elem != &class_split.back() && elem.size() > 1) {
      elem = elem.substr(0, 1);
    }
  }

  out = utils::StringUtils::join(class_delim, class_split);
}

} /* namespace utils */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
