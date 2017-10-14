/**
 * @file AccumuloScanner.h
 * AccumuloScanner class declaration
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
#ifndef __AABULK_IMPORT__
#define __AABULK_IMPORT__

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

// AccumuloScanner Class
class __attribute__((visibility("default"))) AccumuloScanner : public core::Processor {
 public:
  // Constructor
  /*!
   * Create a new processor
   */
  AccumuloScanner(std::string name, uuid_t uuid = NULL)
      : Processor(name, uuid),
        logger_(logging::LoggerFactory<AccumuloScanner>::getLogger()) {
  }
  // Destructor
  virtual ~AccumuloScanner() {
  }
  // Processor Name
  static constexpr char const* ProcessorName = "AccumuloScanner";
  // Supported Properties
  static core::Property Zookeepers;
  static core::Property InstanceId;
  static core::Property Username;
  static core::Property Password;
  static core::Property Table;
  // Supported Relationships
  static core::Relationship Success;

  class ValueCallback : public OutputStreamCallback {
   public:
    ValueCallback(uint8_t *message, size_t size)
        : message_(message),
          size_(size) {
    }

    virtual ~ValueCallback() {

    }

    virtual int64_t process(std::shared_ptr<io::BaseStream> stream) {
      return stream->write(message_, size_);
    }

   private:
    uint8_t *message_;
    size_t size_;
  };

 public:
  // OnTrigger method, implemented by NiFi AccumuloScanner
  virtual void onTrigger(core::ProcessContext *context, core::ProcessSession *session);
  void onSchedule(std::shared_ptr<core::ProcessContext> context, std::shared_ptr<core::ProcessSessionFactory> sessionFactory);
  // Initialize, over write by NiFi AccumuloScanner
  virtual void initialize(void);

 protected:

 private:

  std::string table_;
  std::string zookeepers_;
  std::string username_;
  std::string password_;
  std::string instanceid_;


  std::mutex scanner_mutex_;
  std::unique_ptr<cclient::data::zookeeper::ZookeeperInstance> instance;

    std::unique_ptr<interconnect::MasterConnect> master;

    std::unique_ptr<interconnect::AccumuloTableOperations> ops;
  // Logger
  std::vector<cclient::data::KeyValue*> keyValues;
  std::shared_ptr<logging::Logger> logger_;
};

REGISTER_RESOURCE(AccumuloScanner);

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
