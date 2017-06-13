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
#ifndef LIBMINIFI_INCLUDE_PROCESSORS_MERGECONTENT_H_
#define LIBMINIFI_INCLUDE_PROCESSORS_MERGECONTENT_H_

#include "FlowFileRecord.h"
#include "core/Processor.h"
#include "core/ProcessSession.h"
#include "core/Core.h"
#include "core/Property.h"
#include "core/Resource.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

// MergeContent Class
class MergeContent : public core::Processor {
 public:

  // Constructor
  /*!
   * Create a new processor
   */
  MergeContent(std::string name, uuid_t uuid = NULL)
      : Processor(name, uuid),
        logger_(logging::LoggerFactory<MergeContent>::getLogger()) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
  }
  // Destructor
  virtual ~MergeContent();
  // Processor Name
  static const char *ProcessorName;
  // Supported Properties
  static core::Property MergeStrategy;
  static core::Property MergeFormat;
  static core::Property AttributeStrategy;
  static core::Property DelimiterStrategy;

  // Supported Relationships
  static core::Relationship Original;
  static core::Relationship Failure;
  static core::Relationship Merged;

  void onTrigger(core::ProcessContext *context, core::ProcessSession *session);
  void initialize();
  void onSchedule(core::ProcessContext *context, core::ProcessSessionFactory *sessionFactory);

 protected:

 private:
  std::shared_ptr<logging::Logger> logger_;
};

REGISTER_RESOURCE(MergeContent)

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */




#endif /* LIBMINIFI_INCLUDE_PROCESSORS_MERGECONTENT_H_ */
