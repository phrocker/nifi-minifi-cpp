/**
 * @file BulkImport.h
 * BulkImport class declaration
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
#ifndef __BULK_IMPORT__
#define __BULK_IMPORT__

#include <data/constructs/client/zookeeperinstance.h>
#include <interconnect/Master.h>

#include "FlowFileRecord.h"
#include "core/Processor.h"
#include "core/ProcessSession.h"
#include "core/Core.h"
#include "core/Resource.h"
#include "core/logging/LoggerConfiguration.h"
#include <data/constructs/KeyValue.h>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

// BulkImport Class
class __attribute__((visibility("default"))) BulkImport : public core::Processor {
 public:
  // Constructor
  /*!
   * Create a new processor
   */
  BulkImport(std::string name, uuid_t uuid = NULL)
      : Processor(name, uuid),
        logger_(logging::LoggerFactory<BulkImport>::getLogger()) {
  }
  // Destructor
  virtual ~BulkImport() {
  }
  // Processor Name
  static constexpr char const* ProcessorName = "BulkImport";
  // Supported Properties
  static core::Property Zookeepers;
  static core::Property InstanceId;
  static core::Property Username;
  static core::Property Password;
  static core::Property Table;
  // Supported Relationships
  static core::Relationship Success;

  // Nest Callback Class for read stream
  class ReadCallback : public InputStreamCallback {
   public:
    ReadCallback(uint64_t size)
        : read_size_(0) {
      buffer_size_ = size;
      buffer_ = new uint8_t[buffer_size_];
    }
    ~ReadCallback() {
      if (buffer_)
        delete[] buffer_;
    }
    int64_t process(std::shared_ptr<io::BaseStream> stream) {
      int64_t ret = 0;
      ret = stream->read(buffer_, buffer_size_);
      if (!stream)
        read_size_ = stream->getSize();
      else
        read_size_ = buffer_size_;
      return ret;
    }
    uint8_t *buffer_;
    uint64_t buffer_size_;
    uint64_t read_size_;
  };

 public:
  // OnTrigger method, implemented by NiFi BulkImport
  virtual void onTrigger(core::ProcessContext *context, core::ProcessSession *session);
  void onSchedule(std::shared_ptr<core::ProcessContext> context, std::shared_ptr<core::ProcessSessionFactory> sessionFactory);
  // Initialize, over write by NiFi BulkImport
  virtual void initialize(void);

 protected:

 private:

  std::string table_;
  std::string zookeepers_;
  std::string username_;
  std::string password_;
  std::string instanceid_;


  // Logger
  std::vector<cclient::data::KeyValue*> keyValues;
  std::shared_ptr<logging::Logger> logger_;
};

REGISTER_RESOURCE(BulkImport);

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif