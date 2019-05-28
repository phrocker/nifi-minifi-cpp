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
#ifndef __SPLIT_TEXT_H__
#define __SPLIT_TEXT_H__

#include "FlowFileRecord.h"
#include "core/Processor.h"
#include "core/ProcessSession.h"
#include "core/Core.h"
#include "core/Resource.h"
#include "core/logging/LoggerConfiguration.h"
#include "core/state/nodes/MetricsBase.h"
#include "FileMetrics.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

struct Split {
  uint64_t start;
  uint64_t length;
  uint64_t trimmedLength;  // for the case where we removed the newline(s)
  uint64_t lineCount;
};

class SplitText : public core::Processor, public state::response::MetricsNodeSource {
 public:
  // Constructor
  /*!
   * Create a new processor
   */
  explicit SplitText(std::string name, utils::Identifier uuid = utils::Identifier())
      : core::Processor(name, uuid),
        removeNewLines(false),
        maxSplitSize(0),
        lineCount(0),
        headerLineCount(0),
        logger_(logging::LoggerFactory<SplitText>::getLogger()) {
    metrics_ = std::make_shared<metrics::FileMetrics>("GetFileMetrics");
  }
  // Destructor
  virtual ~SplitText() {
  }
  // Processor Name
  static constexpr char const* ProcessorName = "SplitText";
  // Supported Properties
  static core::Property SplitLineCount;
  static core::Property FragmentMaxSize;
  static core::Property RemoveTrailingNewLines;
  static core::Property HeaderLineCount;
  static core::Property HeaderLineMarker;
  // Supported Relationships
  static core::Relationship Original;
  static core::Relationship Splits;
  static core::Relationship Failure;

 public:
  /**
   * Function that's executed when the processor is scheduled.
   * @param context process context.
   * @param sessionFactory process session factory that is used when creating
   * ProcessSession objects.
   */
  virtual void onSchedule(std::shared_ptr<core::ProcessContext> context, std::shared_ptr<core::ProcessSessionFactory> sessionFactory) override;
  // OnTrigger method, implemented by NiFi SplitText
  virtual virtual void onTrigger(std::shared_ptr<core::ProcessContext> context, std::shared_ptr<core::ProcessSession> session) override;
  // Initialize, over write by NiFi SplitText
  virtual void initialize(void) override;

  /**
   * Populates the metrics. In retrospect these should be weak ptrs to internal metrics, but the lifetime should be short lived.
   */
  virtual int16_t getMetricNodes(std::vector<std::shared_ptr<state::response::ResponseNode>> &metric_vector) override {
    metric_vector.push_back(metrics_);
    return 0;
  }

 protected:

  class SplitLocator : public InputStreamCallback {
   public:
    SplitLocator(const bool &removeNewLines, const uint32_t &maxSplitSize, const int &lineCount, const int &headerLineCount, const std::string &headerMarker);
    ~SplitLocator();
    virtual int64_t process(std::shared_ptr<io::BaseStream> stream) override;

    std::vector<Split> getSplits() const {
      return splits_;
    }

   protected:
    std::vector<Split> splits_;

   private:
    bool removeNewLines;
    uint32_t maxSplitSize;
    int lineCount;
    int headerLineCount;
    std::string headerMarker;
    std::shared_ptr<logging::Logger> logger_;
  };

 private:

  std::shared_ptr<metrics::FileMetrics> metrics_;

  bool removeNewLines;
  uint32_t maxSplitSize;
  int lineCount;
  int headerLineCount;
  std::string headerMarker;

  std::shared_ptr<logging::Logger> logger_;
};

REGISTER_RESOURCE(SplitText, "Splits a text file into multiple smaller text files on line boundaries limited by maximum number of lines "
                  "or total size of fragment. Each output split file will contain no more than the configured number of lines or bytes. "
                  "If both Line Split Count and Maximum Fragment Size are specified, the split occurs at whichever limit is reached first. "
                  "If the first line of a fragment exceeds the Maximum Fragment Size, that line will be output in a single split file which "
                  "exceeds the configured maximum size limit. This component also allows one to specify that each split should include a header "
                  "lines. Header lines can be computed by either specifying the amount of lines that should constitute a header or by using header "
                  "marker to match against the read lines. If such match happens then the corresponding line will be treated as header. Keep in mind "
                  "that upon the first failure of header marker match, no more matches will be performed and the rest of the data will be parsed as "
                  "regular lines for a given split. If after computation of the header there are no more data, the resulting split will consists "
                  "of only header lines.");

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
