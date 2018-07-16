/**
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
#ifndef LIBMINIFI_INCLUDE_CORE_EVENT_H_
#define LIBMINIFI_INCLUDE_CORE_EVENT_H_

#include <chrono>
#include <sstream>
#include "state/Value.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {

class EventBuilder;

enum EventType {
  FAILURE,
  WARN,
  INFO
};

class Event {
 public:
  Event() = delete;

  /**
   * Increments the internal countr.
   */
  void incrementCount() {
    count_++;
  }

  /**
   * Returns the event name as a constant string
   * @return event name.
   */
  std::string getName() const {
    return event_name_;
  }

  Event &operator=(const Event &other) {
    event_name_ = other.event_name_;
    event_information_ = other.event_information_;
    count_ = other.count_;
    last_time_ = other.last_time_;
    return *this;
  }

  std::string to_string() const {
    std::stringstream str;
    str << event_name_ << "::" << event_information_.to_string() << "::" << count_ << "::" << last_time_;
    return str.str();
  }

  EventType getType() const {
    return event_type_;
  }

  size_t getSize() const {
    return size_;
  }

 protected:

  Event(const std::string &name, state::response::ValueNode &value, uint64_t last_time, int64_t count)
      : event_name_(name),
        event_information_(value),
        count_(count),
        last_time_(last_time),
        event_type_(INFO) {
    size_ = name.size() + value.getSize();
  }

  Event(const std::string &name, state::response::ValueNode &value, uint64_t last_time, int64_t count, EventType type)
      : event_name_(name),
        event_information_(value),
        count_(count),
        last_time_(last_time),
        event_type_(type) {
    size_ = name.size() + value.getSize();
  }

  /**
   * Sets the count within this Event.
   */
  void setCount(const int64_t &count) {
    count_ = count;
  }

  /**
   * Sets the event type.
   *
   */
  void setEventType(EventType event_type) {
    event_type_ = event_type;
  }

  std::string event_name_;
  state::response::ValueNode event_information_;
  int64_t count_;
  uint64_t last_time_;
  EventType event_type_;
  size_t size_;
  friend class EventBuilder;
};

class EventBuilder {

 public:
  static EventBuilder newEvent(const std::string name, const state::response::ValueNode &node) {
    EventBuilder new_builder(name, node);
    return new_builder;
  }

  static EventBuilder newEvent(const std::string name, const state::response::ValueNode &node, EventType type) {
    EventBuilder new_builder(name, node);
    new_builder.event_type_ = type;
    return new_builder;
  }

  EventBuilder withLastTime(uint64_t time) {
    last_time_ = time;
    return *this;
  }

  EventBuilder withCount(int64_t count) {
    count_ = count;
    return *this;
  }

  Event build() {
    return Event(event_name_, value_, last_time_, count_, event_type_);
  }

  EventBuilder &operator=(const EventBuilder &other) {
    event_name_ = other.event_name_;
    value_ = other.value_;
    count_ = other.count_;
    last_time_ = other.last_time_;
    event_type_ = other.event_type_;
    return *this;
  }

 private:
  EventBuilder(const std::string &name, const state::response::ValueNode &value)
      : event_name_(name),
        value_(value),
        count_(1),
        last_time_((uint64_t) (std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1))),
        event_type_(INFO) {
  }

  std::string event_name_;
  state::response::ValueNode value_;
  int64_t count_;
  uint64_t last_time_;
  EventType event_type_;
};

} /* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_EVENT_H_ */
