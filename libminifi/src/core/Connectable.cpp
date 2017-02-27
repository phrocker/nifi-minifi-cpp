/*
 * Connectable.cpp
 *
 *  Created on: Feb 27, 2017
 *      Author: mparisi
 */

#include "../../include/core/Connectable.h"

#include <uuid/uuid.h>
#include "core/logging/Logger.h"
#include "core/Relationship.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {

Connectable::Connectable(std::string name, uuid_t uuid)
    : logger_(logging::Logger::getLogger()),
      name_(name),
      max_concurrent_tasks_(1) {
  if (!uuid)
    // Generate the global UUID for the flow record
    uuid_generate(uuid_);
  else
    uuid_copy(uuid_, uuid);

  char uuidStr[37];
  uuid_unparse_lower(uuid_, uuidStr);
  uuidStr_ = uuidStr;
}

Connectable::Connectable(const Connectable &&other)
    : name_(std::move(other.name_)),
      logger_(logging::Logger::getLogger()),
      max_concurrent_tasks_(std::move(other.max_concurrent_tasks_)) {
  uuid_copy(uuid_, other.uuid_);
  has_work_ = other.has_work_.load();
  strategy_ = other.strategy_.load();
}

Connectable::~Connectable() {

}

// Set UUID
void Connectable::setUUID(uuid_t uuid) {
  uuid_copy(uuid_, uuid);
  char uuidStr[37];
  uuid_unparse_lower(uuid_, uuidStr);
  uuidStr_ = uuidStr;
}
// Get UUID
bool Connectable::getUUID(uuid_t uuid) const {
  if (uuid) {
    uuid_copy(uuid, uuid_);
    return true;
  } else {
    return false;
  }
}

// Get UUID
unsigned const char *Connectable::getUUID() const {
  return uuid_;
}

// Set Processor Name
void Connectable::setName(const std::string name) {
  name_ = name;
}
// Get Process Name
std::string Connectable::getName() const {
  return name_;
}

bool Connectable::setSupportedRelationships(
    std::set<core::Relationship> relationships) {
  if (isRunning()) {
    logger_->log_info(
        "Can not set processor supported relationship while the process %s is running",
        name_.c_str());
    return false;
  }

  std::lock_guard<std::mutex> lock(relationship_mutex_);

  relationships_.clear();
  for (auto item : relationships) {
    relationships_[item.getName()] = item;
    logger_->log_info("Processor %s supported relationship name %s",
                      name_.c_str(), item.getName().c_str());
  }

  return true;
}

// Whether the relationship is supported
bool Connectable::isSupportedRelationship(
    core::Relationship relationship) {
  const bool requiresLock = isRunning();

  const auto conditionalLock =
      !requiresLock ?
          std::unique_lock<std::mutex>() :
          std::unique_lock<std::mutex>(relationship_mutex_);

  const auto &it = relationships_.find(relationship.getName());
  if (it != relationships_.end()) {
    return true;
  } else {
    return false;
  }
}

bool Connectable::setAutoTerminatedRelationships(
    std::set<Relationship> relationships) {
  if (isRunning()) {
    logger_->log_info(
        "Can not set processor auto terminated relationship while the process %s is running",
        name_.c_str());
    return false;
  }

  std::lock_guard<std::mutex> lock(relationship_mutex_);

  auto_terminated_relationships_.clear();
  for (auto item : relationships) {
    auto_terminated_relationships_[item.getName()] = item;
    logger_->log_info("Processor %s auto terminated relationship name %s",
                      name_.c_str(), item.getName().c_str());
  }

  return true;
}

// Check whether the relationship is auto terminated
bool Connectable::isAutoTerminated(
    core::Relationship relationship) {
  const bool requiresLock = isRunning();

  const auto conditionalLock =
      !requiresLock ?
          std::unique_lock<std::mutex>() :
          std::unique_lock<std::mutex>(relationship_mutex_);

  const auto &it = auto_terminated_relationships_.find(relationship.getName());
  if (it != auto_terminated_relationships_.end()) {
    return true;
  } else {
    return false;
  }
}

void Connectable::waitForWork(uint64_t timeoutMs) {
  has_work_.store(isWorkAvailable());

  if (!has_work_.load()) {
    std::unique_lock<std::mutex> lock(work_available_mutex_);
    work_condition_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                             [&] {return has_work_.load();});
  }

}

void Connectable::notifyWork() {
  // Do nothing if we are not event-driven
  if (strategy_ != EVENT_DRIVEN) {
    return;
  }

  {
    has_work_.store(isWorkAvailable());

    if (has_work_.load()) {
      work_condition_.notify_one();
    }
  }

}


std::set<std::shared_ptr<Connectable>> Connectable::getOutGoingConnections(
    std::string relationship) {
  std::set<std::shared_ptr<Connectable>> empty;

  auto &&it = _outGoingConnections.find(relationship);
  if (it != _outGoingConnections.end()) {
    return _outGoingConnections[relationship];
  } else {
    return empty;
  }
}


std::shared_ptr<Connectable>Connectable::getNextIncomingConnection() {
  std::lock_guard<std::mutex> lock(relationship_mutex_);

  if (_incomingConnections.size() == 0)
    return NULL;

  if (incoming_connections_Iter == _incomingConnections.end())
    incoming_connections_Iter = _incomingConnections.begin();

  std::shared_ptr<Connectable>ret = *incoming_connections_Iter;
  incoming_connections_Iter++;

  if (incoming_connections_Iter == _incomingConnections.end())
    incoming_connections_Iter = _incomingConnections.begin();

  return ret;
}

} /* namespace components */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
