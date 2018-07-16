/**
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ref_count_hip.
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
#ifndef LIBMINIFI_INCLUDE_CORE_REPOSITORY_EVENTREPOSITORY_H_
#define LIBMINIFI_INCLUDE_CORE_REPOSITORY_EVENTREPOSITORY_H_

#include "core/ClassLoader.h"
#include "core/Core.h"
#include "core/Event.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {
namespace repository {

class EventRepository : public core::CoreComponent {
 public:

  EventRepository(const std::string &name)
      : core::CoreComponent(name) {
  }
  virtual ~EventRepository() {
  }

  /**
   * Put an event into the repository.
   *
   */
  virtual bool put(const Event &event, bool update = true) = 0;

  virtual void initialize(const std::string &directory, const size_t max_size, const size_t max_events) = 0;

 protected:

};

class MappedEventRepository : public EventRepository {
 public:

  MappedEventRepository(const std::string &name)
      : EventRepository(name) {
  }
  virtual ~MappedEventRepository() {
  }

  virtual Event get(const std::string &name) = 0;

  virtual bool remove(const std::string &name) = 0;

 protected:

};

class VolatileEventRepository : public MappedEventRepository {
 public:
  VolatileEventRepository(const std::string &name)
      : MappedEventRepository(name),
        size_(0),
        max_size_(0),
        max_events_(1000) {
  }

  virtual bool put(const Event &event, bool update = true) {
    std::lock_guard<std::mutex> lock(guard_);
    auto current_obj_ = events_.find(event.getName());
    if (current_obj_ != std::end(events_)) {
      if (update) {
        current_obj_->second.incrementCount();
      }
      return true;
    }
    if (max_size_ > 0) {
      if (size_ + event.getSize() > max_size_) {
        return false;
      }
    }

    if (max_events_ > 0) {
      if (events_.size() + 1 > max_events_) {
        return false;
      }
    }
    events_.insert(std::make_pair(event.getName(), event));

    return true;

  }

  virtual void initialize(const std::string &directory, const size_t max_size, const size_t max_events) {
    max_size_ = max_size;
    max_events_ = max_events;
  }

 protected:
  size_t size_;
  size_t max_size_;
  size_t max_events_;
  std::mutex guard_;
  std::map<std::string, Event> events_;
};

class EventRepositoryFactory {
 public:

  static std::shared_ptr<EventRepository> getRepository(const std::string &name) {
    auto ptr = getInstance().findRepository(name);
    return ptr;
  }

  static std::shared_ptr<EventRepository> createRepository(const std::string &name, const std::string &type, const std::string &directory, const size_t max_size, const size_t max_events) {

    auto repo_inst = core::ClassLoader::getDefaultClassLoader().instantiate(type, type);
    if (nullptr != repo_inst) {
      std::shared_ptr<EventRepository> repo = std::dynamic_pointer_cast<EventRepository>(repo_inst);
      if (nullptr != repo) {
        repo->setName(name);
        repo->initialize(directory, max_size, max_events);
        // insert the repository into the repo list
        getInstance().insert(repo);
        return repo;
      }
    }
    return nullptr;

  }
 protected:

  static EventRepositoryFactory getInstance() {
    static EventRepositoryFactory instance;
    return instance;
  }

  void insert(const std::shared_ptr<EventRepository> &repo) {
    if (repo != nullptr) {
      std::lock_guard<std::mutex> guard(mutex_);
      repos.insert(std::make_pair(repo->getName(), repo));
    }
  }

  std::shared_ptr<EventRepository> findRepository(const std::string &name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto finder = repos.find(name);
    if (finder != std::end(repos)) {
      return finder->second;
    }
    return nullptr;
  }

  EventRepositoryFactory(const EventRepositoryFactory &other)
      : repos(other.repos) {
  }

  EventRepositoryFactory() {
  }

  std::mutex mutex_;
  std::map<std::string, std::shared_ptr<EventRepository>> repos;
};
} /* namespace repository */
} /* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_REPOSITORY_EVENTREPOSITORY_H_ */
