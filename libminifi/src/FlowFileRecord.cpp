/**
 * @file FlowFileRecord.cpp
 * Flow file record class implementation 
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
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <cstdio>

#include "FlowFileRecord.h"

#include "core/logging/Logger.h"
#include "core/Relationship.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {

std::atomic<uint64_t> FlowFileRecord::_localFlowSeqNumber(0);

FlowFileRecord::FlowFileRecord(std::map<std::string, std::string> attributes,
                               std::shared_ptr<ResourceClaim> claim)
    : Record()

{

  id_ = _localFlowSeqNumber.load();
  claim_ = claim;
  // Increase the local ID for the flow record
  ++_localFlowSeqNumber;
  // Populate the default attributes
  addKeyedAttribute(FILENAME, std::to_string(getTimeNano()));
  addKeyedAttribute(PATH, DEFAULT_FLOWFILE_PATH);
  addKeyedAttribute(UUID, getUUIDStr());
  // Populate the attributes from the input
  std::map<std::string, std::string>::iterator it;
  for (it = attributes.begin(); it != attributes.end(); it++) {
    Record::addAttribute(it->first, it->second);
  }

  snapshot_ = false;

  if (claim_ != nullptr)
    // Increase the flow file record owned count for the resource claim
    claim_->increaseFlowFileRecordOwnedCount();
  logger_ = logging::Logger::getLogger();
}

FlowFileRecord::~FlowFileRecord() {
  if (!snapshot_)
    logger_->log_debug("Delete FlowFile UUID %s", uuid_str_.c_str());
  else
    logger_->log_debug("Delete SnapShot FlowFile UUID %s", uuid_str_.c_str());
  if (claim_) {
    // Decrease the flow file record owned count for the resource claim
    claim_->decreaseFlowFileRecordOwnedCount();
    if (claim_->getFlowFileRecordOwnedCount() <= 0) {
      logger_->log_debug("Delete Resource Claim %s",
                         claim_->getContentFullPath().c_str());
      std::remove(claim_->getContentFullPath().c_str());
    }
  }
}

bool FlowFileRecord::addKeyedAttribute(FlowAttribute key, std::string value) {
  const char *keyStr = FlowAttributeKey(key);
  if (keyStr) {
    const std::string keyString = keyStr;
    return Record::addAttribute(keyString, value);
  } else {
    return false;
  }
}

bool FlowFileRecord::removeKeyedAttribute(FlowAttribute key) {
  const char *keyStr = FlowAttributeKey(key);
  if (keyStr) {
    std::string keyString = keyStr;
    return Record::removeAttribute(keyString);
  } else {
    return false;
  }
}

bool FlowFileRecord::updateKeyedAttribute(FlowAttribute key,
                                          std::string value) {
  const char *keyStr = FlowAttributeKey(key);
  if (keyStr) {
    std::string keyString = keyStr;
    return Record::updateAttribute(keyString, value);
  } else {
    return false;
  }
}

bool FlowFileRecord::getKeyedAttribute(FlowAttribute key, std::string &value) {
  const char *keyStr = FlowAttributeKey(key);
  if (keyStr) {
    std::string keyString = keyStr;
    return Record::getAttribute(keyString, value);
  } else {
    return false;
  }
}

FlowFileRecord &FlowFileRecord::operator=(const FlowFileRecord &other) {
  core::Record::operator=(other);
  return *this;
}
/*
 void FlowFileRecord::duplicate(std::shared_ptr<FlowFileRecord> original)
 {
 uuid_copy(this->uuid_, original->uuid_);
 this->attributes_ = original->attributes_;
 this->entry_date_ = original->entry_date_;
 this->id_ = original->id_;
 this->last_queue_date_ = original->last_queue_date_;
 this->lineage_start_date_ = original->lineage_start_date_;
 this->offset_ = original->offset_;
 this->penaltyExpiration_ms_ = original->penaltyExpiration_ms_;
 this->size_ = original->size_;
 this->lineage_Identifiers_ = original->lineage_Identifiers_;
 this->_orginalConnection = original->_orginalConnection;
 this->uuid_str_ = original->uuid_str_;
 this->_connection = original->_connection;
 this->marked_delete_ = original->marked_delete_;

 this->claim_ = original->claim_;
 if (this->claim_)
 this->claim_->increaseFlowFileRecordOwnedCount();

 this->_snapshot = true;
 }

 */


} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
