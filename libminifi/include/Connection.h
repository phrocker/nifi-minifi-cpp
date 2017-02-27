/**
 * @file Connection.h
 * Connection class declaration
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
#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <uuid/uuid.h>
#include <vector>
#include <queue>
#include <map>
#include <mutex>
#include <atomic>
#include <algorithm>

#include "core/Connectable.h"
#include "core/Record.h"
#include "core/logging/Logger.h"
#include "core/Relationship.h"
#include "core/Connectable.h"


namespace org {
namespace apache {
namespace nifi {
namespace minifi {
//! Connection Class
class Connection : public core::Connectable,
    public std::enable_shared_from_this<Connection> {
 public:
  //! Constructor
  /*!
   * Create a new processor
   */
  explicit Connection(std::string name, uuid_t uuid = NULL, uuid_t srcUUID =
  NULL,
                      uuid_t destUUID = NULL);
  //! Destructor
  virtual ~Connection() {
  }

  //! Set Source Processor UUID
  void setSourceUUID(uuid_t uuid) {
    uuid_copy(_srcUUID, uuid);
  }
  //! Set Destination Processor UUID
  void setDestinationUUID(uuid_t uuid) {
    uuid_copy(_destUUID, uuid);
  }
  //! Get Source Processor UUID
  void getSourceUUID(uuid_t uuid) {
    uuid_copy(uuid, _srcUUID);
  }
  //! Get Destination Processor UUID
  void getDestinationUUID(uuid_t uuid) {
    uuid_copy(uuid, _destUUID);
  }

  //! Set Connection Source Processor
  void setSource(
      std::shared_ptr<core::Connectable> source) {
    _srcProcessor = source;
  }
  // ! Get Connection Source Processor
  std::shared_ptr<core::Connectable> getSource() {
    return _srcProcessor;
  }
  //! Set Connection Destination Processor
  void setDestination(
      std::shared_ptr<core::Connectable> dest) {
    _destProcessor = dest;
  }
  // ! Get Connection Destination Processor
  std::shared_ptr<core::Connectable> getDestination() {
    return _destProcessor;
  }
  //! Set Connection relationship
  void setRelationship(
      core::Relationship relationship) {
    relationship_ = relationship;
  }
  // ! Get Connection relationship
  core::Relationship getRelationship() {
    return relationship_;
  }
  //! Set Max Queue Size
  void setMaxQueueSize(uint64_t size) {
    _maxQueueSize = size;
  }
  //! Get Max Queue Size
  uint64_t getMaxQueueSize() {
    return _maxQueueSize;
  }
  //! Set Max Queue Data Size
  void setMaxQueueDataSize(uint64_t size) {
    _maxQueueDataSize = size;
  }
  //! Get Max Queue Data Size
  uint64_t getMaxQueueDataSize() {
    return _maxQueueDataSize;
  }
  //! Set Flow expiration duration in millisecond
  void setFlowExpirationDuration(uint64_t duration) {
    _expiredDuration = duration;
  }
  //! Get Flow expiration duration in millisecond
  uint64_t getFlowExpirationDuration() {
    return _expiredDuration;
  }
  //! Check whether the queue is empty
  bool isEmpty();
  //! Check whether the queue is full to apply back pressure
  bool isFull();
  //! Get queue size
  uint64_t getQueueSize() {
    std::lock_guard<std::mutex> lock(_mtx);
    return queue_.size();
  }
  //! Get queue data size
  uint64_t getQueueDataSize() {
    return _maxQueueDataSize;
  }
  //! Put the flow file into queue
  void put(std::shared_ptr<core::Record> flow);
  //! Poll the flow file from queue, the expired flow file record also being returned
  std::shared_ptr<core::Record> poll(
      std::set<std::shared_ptr<core::Record>> &expiredFlowRecords);
  //! Drain the flow records
  void drain();

  void yield() {

  }

  bool isWorkAvailable() {
    return !isEmpty();
  }

  bool isRunning() {
    return true;
  }

 protected:
  //! Source Processor UUID
  uuid_t _srcUUID;
  //! Destination Processor UUID
  uuid_t _destUUID;
  //! Relationship for this connection
  core::Relationship relationship_;
  //! Source Processor (ProcessNode/Port)
  std::shared_ptr<core::Connectable> _srcProcessor;
  //! Destination Processor (ProcessNode/Port)
  std::shared_ptr<core::Connectable> _destProcessor;
  //! Max queue size to apply back pressure
  std::atomic<uint64_t> _maxQueueSize;
  //! Max queue data size to apply back pressure
  std::atomic<uint64_t> _maxQueueDataSize;
  //! Flow File Expiration Duration in= MilliSeconds
  std::atomic<uint64_t> _expiredDuration;

 private:
  //! Mutex for protection
  std::mutex _mtx;
  //! Queued data size
  std::atomic<uint64_t> _queuedDataSize;
  //! Queue for the Flow File
  std::queue<std::shared_ptr<core::Record>> queue_;
  //! Logger
  std::shared_ptr<logging::Logger> logger_;
  // Prevent default copy constructor and assignment operation
  // Only support pass by reference or pointer
  Connection(const Connection &parent);
  Connection &operator=(const Connection &parent);

};
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
#endif
