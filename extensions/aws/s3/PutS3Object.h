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
#ifndef __PUT_S3_H__
#define __PUT_S3_H__

#include "FlowFileRecord.h"
#include "core/Processor.h"
#include "core/ProcessSession.h"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/PutObjectRequest.h>
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

class PutS3Object : public core::Processor {
 public:
  //! Constructor
  /*!
   * Create a new processor
   */
  PutS3Object(std::string name, utils::Identifier uuid = utils::Identifier())
      : core::Processor(name, uuid), logger_(logging::LoggerFactory<PutS3Object>::getLogger()) {
  }
  //! Destructor
  virtual ~PutS3Object() {
  }
  //! Processor Name
  static const std::string ProcessorName;

  static std::string DefaultContentType;

  //! Supported Relationships
  static core::Relationship Success;
  static core::Relationship Failure;

  static core::Property ObjectKey;
  static core::Property Bucket;
  static core::Property ContentType;
  static core::Property AccessKey;
  static core::Property SecretKey;
  //static core::Property AWSControllerService;
  static core::Property Region;

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

  Aws::Client::ClientConfiguration client_config_;

  // Logger
  std::shared_ptr<logging::Logger> logger_;
};

REGISTER_RESOURCE(PutS3Object, "Puts FlowFiles to an Amazon S3 Bucket The upload uses either the PutS3Object method or PutS3MultipartUpload methods. The PutS3Object method send the file in a single synchronous call, but it has a 5GB size limit. Larger files are sent using the multipart upload methods that initiate, transfer the parts, and complete an upload. This multipart process saves state after each step so that a large upload can be resumed with minimal loss if the processor or cluster is stopped and restarted. A multipart upload consists of three steps 1) initiate upload, 2) upload the parts, and 3) complete the upload. For multipart uploads, the processor saves state locally tracking the upload ID and parts uploaded, which must both be provided to complete the upload. The AWS libraries select an endpoint URL based on the AWS region, but this can be overridden with the 'Endpoint Override URL' property for use with other S3-compatible endpoints. The S3 API specifies that the maximum file size for a PutS3Object upload is 5GB. It also requires that parts in a multipart upload must be at least 5MB in size, except for the last part. These limits are establish the bounds for the Multipart Upload Threshold and Part Size properties.");

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
