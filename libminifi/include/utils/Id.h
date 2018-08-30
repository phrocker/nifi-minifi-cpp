/**
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
#ifndef LIBMINIFI_INCLUDE_UTILS_ID_H_
#define LIBMINIFI_INCLUDE_UTILS_ID_H_

#include <cstddef>
#include <atomic>
#include <memory>
#include <string>
#include <uuid/uuid.h>

#include "core/logging/Logger.h"
#include "properties/Properties.h"

#define UNSIGNED_CHAR_MAX 255
#define UUID_TIME_IMPL 0
#define UUID_RANDOM_IMPL 1
#define UUID_DEFAULT_IMPL 2
#define MINIFI_UID_IMPL 3

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace utils {

template<typename T, void (*F)(T, std::string&), bool (*N)(T), void (*C)(T, const T)>
class IdentifierBase {
 public:

  IdentifierBase(T myid) {
    C(id_, myid);
    F(myid, idString);
  }

  IdentifierBase(const IdentifierBase &other) {
    C(id_, other.id_);
    F(id_, idString);
  }

  IdentifierBase() {
  }

  bool operator==(std::nullptr_t nullp) {
    return N(id_);
  }

  bool operator!=(std::nullptr_t nullp) {
    return !N(id_);
  }

  bool operator==(const IdentifierBase &other) {
    return idString == other.idString;
  }

  IdentifierBase &operator=(const IdentifierBase &other) {
    //id_ = other.id_;
    C(id_, other.id_);
    idString = other.idString;
    return *this;
  }

  IdentifierBase &operator=(T o) {
    C(id_, o);
    F(id_, idString);
    return *this;
  }

  void getIdentifier(T other) const {
    C(other, id_);
  }

  std::string to_string() const {
    return idString;
  }

 protected:

  std::string idString;

  T id_;
};

#ifdef WIN32
typedef IdentifierBase<m_uuid> Identifier;
class Identifier : IdentifierBase<m_uuid> {

};
#else

void uuid_to_string(uuid_t u, std::string &id);

bool is_null(uuid_t u);

void copy_ids(uuid_t dst, const uuid_t src);

class Identifier : public IdentifierBase<uuid_t, uuid_to_string, is_null, copy_ids> {
 public:
  Identifier(uuid_t u)
      : IdentifierBase(u) {
  }

  Identifier()
      : IdentifierBase() {
  }

  Identifier &operator=(const IdentifierBase<uuid_t, uuid_to_string, is_null, copy_ids> &other) {
    IdentifierBase::operator =(other);
    return *this;
  }

  Identifier &operator=(const Identifier &other) {
    IdentifierBase::operator =(other);
    return *this;
  }

  Identifier &operator=(uuid_t o) {
    IdentifierBase::operator=(o);
    return *this;
  }

  Identifier &operator=(const std::string &id) {
    uuid_parse(id.c_str(), id_);
    uuid_to_string(id_,idString);
    return *this;
  }

  bool operator==(const Identifier &other) {
    return idString == other.idString;
  }

  unsigned char *toArray(){
    return id_;
  }

};

#endif

class IdGenerator {
 public:
  void generate(Identifier &output);
  Identifier generate();
  void initialize(const std::shared_ptr<Properties> & properties);

  static std::shared_ptr<IdGenerator> getIdGenerator() {
    static std::shared_ptr<IdGenerator> generator = std::shared_ptr<IdGenerator>(new IdGenerator());
    return generator;
  }
 protected:
  uint64_t getDeviceSegmentFromString(const std::string & str, int numBits);
  uint64_t getRandomDeviceSegment(int numBits);
 private:
  IdGenerator();
  int implementation_;
  std::shared_ptr<minifi::core::logging::Logger> logger_;
  unsigned char deterministic_prefix_[8];
  std::atomic<uint64_t> incrementor_;
};

class NonRepeatingStringGenerator {
 public:
  NonRepeatingStringGenerator();
  std::string generate() {
    return prefix_ + std::to_string(incrementor_++);
  }
 private:
  std::atomic<uint64_t> incrementor_;
  std::string prefix_;
};

} /* namespace utils */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_UTILS_ID_H_ */
