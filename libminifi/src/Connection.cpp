/**
 * @file Connection.cpp
 * Connection class implementation
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
#include <set>
#include <sys/time.h>
#include <time.h>
#include <chrono>
#include <thread>
#include <iostream>

#include "Connection.h"
#include "core/Processor.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {

Connection::Connection(std::string name, uuid_t uuid, uuid_t srcUUID,
                       uuid_t destUUID)
    : core::Connectable(name, uuid) {

  if (srcUUID)
    uuid_copy(_srcUUID, srcUUID);
  if (destUUID)
    uuid_copy(_destUUID, destUUID);

  _srcProcessor = NULL;
  _destProcessor = NULL;
  _maxQueueSize = 0;
  _maxQueueDataSize = 0;
  _expiredDuration = 0;
  _queuedDataSize = 0;

  logger_ = logging::Logger::getLogger();

  logger_->log_info("Connection %s created", name_.c_str());
}

bool Connection::isEmpty() {
  std::lock_guard<std::mutex> lock(_mtx);

  return queue_.empty();
}

bool Connection::isFull() {
  std::lock_guard<std::mutex> lock(_mtx);

  if (_maxQueueSize <= 0 && _maxQueueDataSize <= 0)
    // No back pressure setting
    return false;

  if (_maxQueueSize > 0 && queue_.size() >= _maxQueueSize)
    return true;

  if (_maxQueueDataSize > 0 && _queuedDataSize >= _maxQueueDataSize)
    return true;

  return false;
}

void Connection::put(std::shared_ptr<core::Record> flow) {
  {
    std::lock_guard<std::mutex> lock(_mtx);

    queue_.push(flow);

    _queuedDataSize += flow->getSize();

    logger_->log_debug("Enqueue flow file UUID %s to connection %s",
                       flow->getUUIDStr().c_str(), name_.c_str());
  }

  // Notify receiving processor that work may be available
  if (_destProcessor) {
    _destProcessor->notifyWork();
  }
}

std::shared_ptr<core::Record> Connection::poll(
    std::set<std::shared_ptr<core::Record>> &expiredFlowRecords) {
  std::lock_guard<std::mutex> lock(_mtx);

  while (!queue_.empty()) {
    std::shared_ptr<core::Record> item = queue_.front();
    queue_.pop();
    _queuedDataSize -= item->getSize();

    if (_expiredDuration > 0) {
      // We need to check for flow expiration
      if (getTimeMillis() > (item->getEntryDate() + _expiredDuration)) {
        // Flow record expired
        expiredFlowRecords.insert(item);
      } else {
        // Flow record not expired
        if (item->isPenalized()) {
          // Flow record was penalized
          queue_.push(item);
          _queuedDataSize += item->getSize();
          break;
        }
        std::shared_ptr<Connectable> connectable = std::static_pointer_cast<
            Connectable>(shared_from_this());
        item->setOriginalConnection(connectable);
        logger_->log_debug("Dequeue flow file UUID %s from connection %s",
                           item->getUUIDStr().c_str(), name_.c_str());
        return item;
      }
    } else {
      // Flow record not expired
      if (item->isPenalized()) {
        // Flow record was penalized
        queue_.push(item);
        _queuedDataSize += item->getSize();
        break;
      }
      std::shared_ptr<Connectable> connectable = std::static_pointer_cast<
          Connectable>(shared_from_this());
      item->setOriginalConnection(connectable);
      logger_->log_debug("Dequeue flow file UUID %s from connection %s",
                         item->getUUIDStr().c_str(), name_.c_str());
      return item;
    }
  }

  return NULL;
}

void Connection::drain() {
  std::lock_guard<std::mutex> lock(_mtx);

  while (!queue_.empty()) {
    auto &&item = queue_.front();
    queue_.pop();
  }

  logger_->log_debug("Drain connection %s", name_.c_str());
}

} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
