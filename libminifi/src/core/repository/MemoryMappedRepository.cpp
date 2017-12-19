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

#include "core/repository/MemoryMappedRepository.h"
#include <memory>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace core {
namespace repository {

bool MemoryMappedRepository::initialize(const std::shared_ptr<minifi::Configure> &configuration) {
  return true;
}
void MemoryMappedRepository::stop() {
}

std::shared_ptr<io::BaseStream> MemoryMappedRepository::write(const std::shared_ptr<minifi::ResourceClaim> &claim) {
  return std::make_shared<io::MmapStream>(claim->getContentFullPath());
}

bool MemoryMappedRepository::exists(const std::shared_ptr<minifi::ResourceClaim> &streamId) {
  std::ifstream file(streamId->getContentFullPath());
  return file.good();
}

std::shared_ptr<io::BaseStream> MemoryMappedRepository::read(const std::shared_ptr<minifi::ResourceClaim> &claim) {
  /*
  std::lock_guard<std::mutex> lock(read_stream_mutex_);

  auto ptr = read_streams_.find(claim->getContentFullPath());

  if (ptr != read_streams_.end()) {
    return ptr->second;
  } else {
  */
    auto mmap_ptr = std::make_shared<io::MmapStream>(claim->getContentFullPath(), 0, false);
    read_streams_[claim->getContentFullPath()] = mmap_ptr;
    return mmap_ptr;
  //}
}

bool MemoryMappedRepository::remove(const std::shared_ptr<minifi::ResourceClaim> &claim) {
  std::remove(claim->getContentFullPath().c_str());
  //std::lock_guard<std::mutex> lock(read_stream_mutex_);
  //read_streams_.erase(claim->getContentFullPath());
  return true;
}

} /* namespace repository */
} /* namespace core */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
