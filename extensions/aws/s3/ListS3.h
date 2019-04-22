/**
 * @file GetGPS.h
 * GetGPS class declaration
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
#ifndef __LIST_S3_H__
#define __LIST_S3_H__

#include "../FlowFileRecord.h"
#include "../core/Processor.h"
#include "../core/ProcessSession.h"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

class ListS3 : public core::Processor {
 public:
  //! Constructor
  /*!
   * Create a new processor
   */
  ListS3(std::string name, utils::Identifier uuid = utils::Identifier())
      : core::Processor(name, uuid), logger_(logging::LoggerFactory<ListS3>::getLogger()) {
  }
  //! Destructor
  virtual ~ListS3() {
  }
  //! Processor Name
  static const std::string ProcessorName;

  //! Supported Relationships
  static core::Relationship Success;

 public:
  /**
   * Function that's executed when the processor is scheduled.
   * @param context process context.
   * @param sessionFactory process session factory that is used when creating
   * ProcessSession objects.
   */
  void onSchedule(const std::shared_ptr<core::ProcessContext> &context,
                  const std::shared_ptr<core::ProcessSessionFactory> &sessionFactory) override;
  //! OnTrigger method, implemented by NiFi GetGPS
  virtual void onTrigger(const std::shared_ptr<core::ProcessContext> &context,
                         const std::shared_ptr<core::ProcessSession> &session) override;
  //! Initialize, over write by NiFi GetGPS
  virtual void initialize(void) override;

 private:
  // Logger
  std::shared_ptr<logging::Logger> logger_;
};

REGISTER_RESOURCE(ListS3, "Lists all of the S3 buckets at a specified level");

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
