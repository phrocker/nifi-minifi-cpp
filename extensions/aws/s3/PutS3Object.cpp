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

#include <string>

#include "PutS3Object.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"
#include "core/PropertyValidation.h"
#include "S3.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

/**
 static core::Property ObjectKey;
 static core::Property Bucket;
 static core::Property ContentType;
 static core::Property AccessKey;
 static core::Property SecretKey;
 static core::Property AWSControllerService;
 static core::Property Region;

 */

std::string PutS3Object::DefaultContentType = "application/octet-stream";

core::Property PutS3Object::ObjectKey(
    core::PropertyBuilder::createProperty("Object Key")->withDescription("Key from which we pull the object")->isRequired(true)->supportsExpressionLanguage(true)->withDefaultValue("${filename}")
        ->build());

core::Property PutS3Object::Bucket(core::PropertyBuilder::createProperty("Bucket")->withDescription("Bucket name")->isRequired(true)->supportsExpressionLanguage(true)->build());

core::Property PutS3Object::ContentType(
    core::PropertyBuilder::createProperty("Content Type")->withDescription(
        "Sets the Content-Type HTTP header indicating the type of content stored in the associated object. The value of this header is a standard MIME type. AWS S3 Java client will attempt to determine the correct content type if one hasn't been set yet. Users are responsible for ensuring a suitable content type is set when uploading streams. If no content type is provided and cannot be determined by the filename, the default content type \"application/octet-stream\" will be used.")
        ->isRequired(false)->supportsExpressionLanguage(true)->withDefaultValue(PutS3Object::DefaultContentType)->build());

core::Property PutS3Object::AccessKey(
    core::PropertyBuilder::createProperty("Access Key")->withDescription("Specifies the AWS Access Key.")->isRequired(false)->supportsExpressionLanguage(true)->build());

core::Property PutS3Object::SecretKey(
    core::PropertyBuilder::createProperty("Secret Key")->withDescription("Specifies the AWS Secret Key.")->isRequired(false)->supportsExpressionLanguage(true)->build());

core::Property PutS3Object::Region(
    core::PropertyBuilder::createProperty("Region")->withDescription("Specifies the region on which to put this S3 Object.")->isRequired(false)->supportsExpressionLanguage(false)
        ->withAllowableValues<std::string>( { "us-west-2", "us-east-1", "us-east-2" })->withDefaultValue("us-east-1")->build());

const std::string PutS3Object::ProcessorName("PutS3Object");
core::Relationship PutS3Object::Success("success", "All files are routed to success");
core::Relationship PutS3Object::Failure("failure", "All files are routed to failure");
void PutS3Object::initialize() {
  //! Set the supported properties
  auto init = S3Singleton::get();

  std::set<core::Property> properties;
  properties.insert(ObjectKey);
  properties.insert(Bucket);
  properties.insert(ContentType);
  properties.insert(AccessKey);
  properties.insert(SecretKey);
  properties.insert(Region);
  setSupportedProperties(properties);
  //! Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert(Success);
  relationships.insert(Failure);
  setSupportedRelationships(relationships);
}

void PutS3Object::onSchedule(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSessionFactory> &sessionFactory) {
  logger_->log_trace("PutS3Object scheduled");

  std::string region;
  if (!context->getProperty(Region.getName(), region)) {
    client_config_.region = region.c_str();
    return;
  }
}

void PutS3Object::onTrigger(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) {

  auto flowFile = session->get();
  if (flowFile) {
    std::string objectKey, bucketName, access_key, secret;
    context->getProperty(ObjectKey, objectKey, flowFile);
    context->getProperty(Bucket, bucketName, flowFile);
    context->getProperty(AccessKey, access_key, flowFile);
    context->getProperty(SecretKey, secret, flowFile);

    if (objectKey.empty()) {
      objectKey = "filename";
    }
    std::string object;
    if (!flowFile->getAttribute(objectKey, object)) {
      object = objectKey;
    }

    // this assumes we have a local file system.
    auto path = flowFile->getResourceClaim()->getContentFullPath();
    client_config_.endpointOverride = "https://s3.amazonaws.com";

    std::unique_ptr<Aws::S3::S3Client> s3_client = nullptr;

    Aws::String s3Ack = access_key.c_str();
    Aws::String s3Secret = secret.c_str();

    Aws::Auth::AWSCredentials creds(s3Ack, s3Secret);

    if (!access_key.empty() && !secret.empty()) {
      s3_client = std::unique_ptr<Aws::S3::S3Client>(new Aws::S3::S3Client(creds, client_config_));
    } else {
      s3_client = std::unique_ptr<Aws::S3::S3Client>(new Aws::S3::S3Client(client_config_));
    }
    Aws::S3::Model::PutObjectRequest object_request;

    Aws::String s3Obj = object.c_str();
    Aws::String s3Bucket = bucketName.c_str();
    object_request.SetBucket(s3Bucket);
    object_request.SetKey(s3Obj);

    const std::shared_ptr<Aws::IOStream> input_data = Aws::MakeShared<Aws::FStream>("SampleAllocationTag", path.c_str(), std::ios_base::in | std::ios_base::binary);
    object_request.SetBody(input_data);

    // Put the object
    auto put_object_outcome = s3_client->PutObject(object_request);
    if (!put_object_outcome.IsSuccess()) {
      auto error = put_object_outcome.GetError();
      session->transfer(flowFile, Failure);
    } else {
      session->transfer(flowFile, Success);
    }
  }

}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
