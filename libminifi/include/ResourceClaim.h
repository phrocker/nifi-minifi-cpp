/**
 * @file ResourceClaim.h
 * Resource Claim class declaration
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
#ifndef __RESOURCE_CLAIM_H__
#define __RESOURCE_CLAIM_H__

#include <string>
#include <uuid/uuid.h>
#include <vector>
#include <queue>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>

#include "core/StreamManager.h"
#include "properties/Configure.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {

// Default content directory
#define DEFAULT_CONTENT_DIRECTORY "./content_repository"

// ResourceClaim Class
class ResourceClaim : public std::enable_shared_from_this<ResourceClaim> {

 public:

  static char *default_directory_path;
  // Constructor
  /*!
   * Create a new resource claim
   */
  ResourceClaim(std::shared_ptr<core::StreamManager<ResourceClaim>> claim_manager, const std::string contentDirectory = default_directory_path);

  ResourceClaim(const std::string path, uint64_t id, uuid_t uuid)
      : claim_manager_(nullptr),
        deleted_(false)
  {
    _contentFullPath = path;
    id_ = id;
    uuid_copy(uuid_, uuid);
  }

  ResourceClaim(const std::string path, std::shared_ptr<core::StreamManager<ResourceClaim>> claim_manager, bool deleted = false)
      : claim_manager_(claim_manager),
        deleted_(deleted)
  {
    _contentFullPath = path;
    id_ = 0;
    uuid_generate(uuid_);
  }

  // Destructor
  virtual ~ResourceClaim() {
  }
  // increaseFlowFileRecordOwnedCount
  void increaseFlowFileRecordOwnedCount() {
    ++_flowFileRecordOwnedCount;
  }
  // decreaseFlowFileRecordOwenedCount
  void decreaseFlowFileRecordOwnedCount() {

    if (_flowFileRecordOwnedCount > 0) {
      _flowFileRecordOwnedCount--;
    }

  }
  // getFlowFileRecordOwenedCount
  uint64_t getFlowFileRecordOwnedCount() {
    return _flowFileRecordOwnedCount;
  }
  // Get the content full path
  std::string getContentFullPath() {
    return _contentFullPath;
  }
  // Set the content full path
  void setContentFullPath(std::string path) {
    _contentFullPath = path;
  }

  void deleteClaim() {
    if (!deleted_)
    {
      deleted_ = true;
    }

  }

 protected:

  std::atomic<bool> deleted_;
  // A global unique identifier
  uuid_t uuid_;
  // A local unique identifier
  uint64_t id_;
  // Full path to the content
  std::string _contentFullPath;

  // How many FlowFileRecord Own this cliam
  std::atomic<uint64_t> _flowFileRecordOwnedCount;

  std::shared_ptr<core::StreamManager<ResourceClaim>> claim_manager_;

 private:

  // Logger
  std::shared_ptr<logging::Logger> logger_;
  // Prevent default copy constructor and assignment operation
  // Only support pass by reference or pointer
  ResourceClaim(const ResourceClaim &parent);
  ResourceClaim &operator=(const ResourceClaim &parent);

  // Local resource claim number
  static std::atomic<uint64_t> _localResourceClaimNumber;
};

} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
#endif
