/**
 * @file FlowFileRecord.h
 * Flow file record class declaration
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
#ifndef __FLOW_FILE_RECORD_H__
#define __FLOW_FILE_RECORD_H__

#include <uuid/uuid.h>
#include <vector>
#include <queue>
#include <map>
#include <mutex>
#include <atomic>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>

#include "utils/TimeUtil.h"
#include "core/logging/Logger.h"
#include "ResourceClaim.h"
#include "Connection.h"
#include "core/Record.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {


#define DEFAULT_FLOWFILE_PATH "."

//! FlowFile Attribute
enum FlowAttribute {
  //! The flowfile's path indicates the relative directory to which a FlowFile belongs and does not contain the filename
  PATH = 0,
  //! The flowfile's absolute path indicates the absolute directory to which a FlowFile belongs and does not contain the filename
  ABSOLUTE_PATH,
  //! The filename of the FlowFile. The filename should not contain any directory structure.
  FILENAME,
  //! A unique UUID assigned to this FlowFile.
  UUID,
  //! A numeric value indicating the FlowFile priority
  priority,
  //! The MIME Type of this FlowFile
  MIME_TYPE,
  //! Specifies the reason that a FlowFile is being discarded
  DISCARD_REASON,
  //! Indicates an identifier other than the FlowFile's UUID that is known to refer to this FlowFile.
  ALTERNATE_IDENTIFIER,
  MAX_FLOW_ATTRIBUTES
};

//! FlowFile Attribute Key
static const char *FlowAttributeKeyArray[MAX_FLOW_ATTRIBUTES] = { "path",
    "absolute.path", "filename", "uuid", "priority", "mime.type",
    "discard.reason", "alternate.identifier" };

//! FlowFile Attribute Enum to Key
inline const char *FlowAttributeKey(FlowAttribute attribute) {
  if (attribute < MAX_FLOW_ATTRIBUTES)
    return FlowAttributeKeyArray[attribute];
  else
    return NULL;
}

//! FlowFile IO Callback functions for input and output
//! throw exception for error
class InputStreamCallback {
 public:
  virtual void process(std::ifstream *stream) = 0;
};
class OutputStreamCallback {
 public:
  virtual void process(std::ofstream *stream) = 0;
};

//! FlowFile Record Class
class FlowFileRecord : public core::Record {
 public:
  //! Constructor
  /*!
   * Create a new flow record
   */
  explicit FlowFileRecord(std::map<std::string, std::string> attributes,
                          std::shared_ptr<ResourceClaim> claim = nullptr);
  //! Destructor
  virtual ~FlowFileRecord();
  //! addAttribute key is enum
  bool addKeyedAttribute(FlowAttribute key, std::string value);
  //! removeAttribute key is enum
  bool removeKeyedAttribute(FlowAttribute key);
  //! updateAttribute key is enum
  bool updateKeyedAttribute(FlowAttribute key, std::string value);
  //! getAttribute key is enum
  bool getKeyedAttribute(FlowAttribute key, std::string &value);
 

  void setSnapShot(bool snapshot) {
    snapshot_ = snapshot;
  }

  FlowFileRecord &operator=(const FlowFileRecord &);

  FlowFileRecord(const FlowFileRecord &parent) = delete;

 protected:
  /*
   //! Date at which the flow file entered the flow
   uint64_t _entryDate;
   //! Date at which the origin of this flow file entered the flow
   uint64_t _lineageStartDate;
   //! Date at which the flow file was queued
   uint64_t _lastQueueDate;
   //! Size in bytes of the data corresponding to this flow file
   uint64_t _size;
   //! A global unique identifier
   uuid_t _uuid;
   //! A local unique identifier
   uint64_t _id;
   //! Offset to the content
   uint64_t _offset;
   //! Penalty expiration
   uint64_t _penaltyExpirationMs;
   //! Attributes key/values pairs for the flow record
   std::map<std::string, std::string> _attributes;
   //! Pointer to the associated content resource claim
   ResourceClaim *_claim;
   //! UUID string
   std::string _uuidStr;
   //! UUID string for all parents
   std::set<std::string> _lineageIdentifiers;
   */

 private:

  //! Local flow sequence ID
  static std::atomic<uint64_t> _localFlowSeqNumber;

  //! Logger
  std::shared_ptr<logging::Logger> logger_;
  //! Snapshot flow record for session rollback
  bool snapshot_;
  // Prevent default copy constructor and assignment operation
  // Only support pass by reference or pointer

};

} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
