/**
 * @file GetGPS.cpp
 * GetGPS class implementation
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

#include <vector>
#include <queue>
#include <map>
#include <set>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>
#include <regex>

#include "ListS3.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"
#include "core/PropertyValidation.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

const std::string ListS3::ProcessorName("ListS3");
core::Relationship ListS3::Success("success", "All files are routed to success");
void ListS3::initialize() {
  //! Set the supported properties
  std::set<core::Property> properties;
  setSupportedProperties(properties);
  //! Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert(Success);
  setSupportedRelationships(relationships);
}

void ListS3::onSchedule(const std::shared_ptr<core::ProcessContext> &context,
                        const std::shared_ptr<core::ProcessSessionFactory> &sessionFactory) {
  logger_->log_trace("ListS3 scheduled");
}

void ListS3::onTrigger(const std::shared_ptr<core::ProcessContext> &context,
                       const std::shared_ptr<core::ProcessSession> &session) {
  logger_->log_info("Invoking ListS3 processor .....!!!!!");
}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
