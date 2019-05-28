/**
 * @file ExtractText.h
 * ExtractText class declaration
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
#ifndef EXTENSIONS_STANDARD_PROCESSORS_PROCESSORS_FILEMETRICS_H_
#define EXTENSIONS_STANDARD_PROCESSORS_PROCESSORS_FILEMETRICS_H_

#include "core/state/nodes/MetricsBase.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {
namespace metrics {

class FileMetrics : public state::response::ResponseNode {
 public:
  FileMetrics(const std::string &name)
      : state::response::ResponseNode(name) {
    iterations_ = 0;
    accepted_files_ = 0;
    input_bytes_ = 0;
    produced_files_ = 0;
  }

  FileMetrics(std::string name, utils::Identifier &uuid)
      : state::response::ResponseNode(name, uuid) {
    iterations_ = 0;
    accepted_files_ = 0;
    input_bytes_ = 0;
    produced_files_ = 0;
  }
  virtual ~FileMetrics() {

  }
  virtual std::string getName() const {
    return core::Connectable::getName();
  }

  inline size_t invoke() {
    return ++iterations_;
  }

  inline size_t increment_produced(const size_t produced) {
    return produced_files_ += produced;
  }

  inline size_t accept_file() {
    return ++accepted_files_;
  }

  inline size_t increment_bytes(const size_t bytes) {
    return input_bytes_ += bytes;
  }

  virtual std::vector<state::response::SerializedResponseNode> serialize() {
    std::vector<state::response::SerializedResponseNode> resp;

    state::response::SerializedResponseNode iter;
    iter.name = "OnTriggerInvocations";
    iter.value = (uint32_t) iterations_.load();

    resp.push_back(iter);

    state::response::SerializedResponseNode accepted_files;
    accepted_files.name = "AcceptedFiles";
    accepted_files.value = (uint32_t) accepted_files_.load();

    resp.push_back(accepted_files);

    state::response::SerializedResponseNode input_bytes;
    input_bytes.name = "InputBytes";
    input_bytes.value = (uint32_t) input_bytes_.load();

    resp.push_back(input_bytes);

    state::response::SerializedResponseNode produced_files;
    produced_files.name = "ProducedFiles";
    produced_files.value = (uint32_t) produced_files_.load();

    resp.push_back(input_bytes);

    return resp;
  }

 protected:
  friend class GetFile;

  std::atomic<size_t> iterations_;
  std::atomic<size_t> accepted_files_;
  std::atomic<size_t> input_bytes_;
  std::atomic<size_t> produced_files_;

};

} /* namespace metrics*/
} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* EXTENSIONS_STANDARD_PROCESSORS_PROCESSORS_FILEMETRICS_H_ */
