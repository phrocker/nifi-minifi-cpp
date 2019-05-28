/**
 * @file SplitText.cpp
 * SplitText class implementation
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
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

#include <limits.h>
#ifndef WIN32
#include <dirent.h>
#include <unistd.h>
#endif
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <memory>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include "utils/TimeUtil.h"
#include "utils/StringUtils.h"
#include "SplitText.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"

#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-compare"
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

core::Property SplitText::SplitLineCount(
    core::PropertyBuilder::createProperty("Line Split Count")->withDescription(
        "The number of lines that will be added to each split file, excluding header lines. "
        "A value of zero requires Maximum Fragment Size to be set, and line count will not be considered in determining splits. ")->isRequired(true)->withDefaultValue<uint32_t>(1)->build());

core::Property SplitText::FragmentMaxSize(
    core::PropertyBuilder::createProperty("Maximum Fragment Size")->withDescription("The maximum size of each split file, including header lines. NOTE: in the case where a "
                                                                                    "single line exceeds this property (including headers, if applicable), that line will be output "
                                                                                    "in a split of its own which exceeds this Maximum Fragment Size setting.")->isRequired(false)
        ->withDefaultValue<core::DataSizeValidator>("1 kb")->build());

core::Property SplitText::HeaderLineCount(
    core::PropertyBuilder::createProperty("Header Line Count")->withDescription(
        "The number of lines that should be considered part of the header; the header lines will be duplicated to all split files")->isRequired(true)->withDefaultValue<uint32_t>(0)->build());

core::Property SplitText::HeaderLineMarker(
    core::PropertyBuilder::createProperty("Header Line Marker Characters")->withDescription(
        "The first character(s) on the line of the datafile which signifies a header line. This value is ignored when Header Line Count is non-zero. "
            + "The first line not containing the Header Line Marker Characters and all subsequent lines are considered non-header")->isRequired(false)->build());

core::Property SplitText::RemoveTrailingNewLines(
    core::PropertyBuilder::createProperty("Remove Trailing Newlines")->withDescription(
        "Whether to remove newlines at the end of each split file. This should be false if you intend to merge the split files later. If this is set to "
        "'true' and a FlowFile is generated that contains only 'empty lines' (i.e., consists only of \r and \n characters), the FlowFile will not be emitted. "
        "Note, however, that if header lines are specified, the resultant FlowFile will never be empty as it will consist of the header lines, so "
        "a FlowFile may be emitted that contains only the header lines.")->isRequired(true)->withDefaultValue<bool>("true")->build());

core::Relationship SplitText::Original("original", "The original file will be routed to this destination.");
core::Relationship SplitText::Splits("splits", "Split files will be routed to this destination when a file is successfully split.");
core::Relationship SplitText::Failure("failure", "If a file cannot be split for some reason, the original file will be routed to this destination and nothing will be routed elsewhere");

void SplitText::initialize() {
  // Set the supported properties
  std::set<core::Property> properties;
  properties.insert( { SplitLineCount, FragmentMaxSize, HeaderLineCount, HeaderLineMarker, RemoveTrailingNewLines });
  // Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert( { Original, Splits, Failure });
}

void SplitText::onSchedule(std::shared_ptr<core::ProcessContext> context, std::shared_ptr<core::ProcessSessionFactory> sessionFactory) {
  context->getProperty(SplitLineCount.getName(), lineCount);
  context->getProperty(FragmentMaxSize.getName(), maxSplitSize);
  context->getProperty(HeaderLineCount.getName(), headerLineCount);
  context->getProperty(HeaderLineMarker.getName(), headerMarker);
  context->getProperty(RemoveTrailingNewLines.getName(), removeNewLines);
}

void SplitText::onTrigger(std::shared_ptr<core::ProcessContext> context, std::shared_ptr<core::ProcessSession> session) {
  metrics_->invoke();
  auto flow_file = session->get();

  if (!flow_file) {
    return;
  }
  metrics_->accept_file();
  SplitLocator locator(removeNewLines, maxSplitSize, lineCount, headerLineCount, headerMarker);
  try {
    /**
     * Locate all splits so that we don't have to perform any future rollback.
     */

  } catch (std::exception &se) {
    logger_->log_error("Error while locating split points %s", se.what());
    session->transfer(flow_file, Failure);
  }
}

SplitText::SplitLocator::SplitLocator(const bool &removeNewLines, const uint32_t &maxSplitSize, const int &lineCount, const int &headerLineCount, const std::string &headerMarker)
    : removeNewLines(removeNewLines),
      maxSplitSize(maxSplitSize),
      lineCount(lineCount),
      headerLineCount(headerLineCount),
      headerMarker(headerMarker),
      logger_(logging::LoggerFactory<SplitText::SplitLocator>::getLogger()) {
}
/**
 * struct Split {
 uint64_t start;
 uint64_t length;
 uint64_t trimmedLength;  // for the case where we removed the newline(s)
 uint64_t lineCount;
 };
 */
int64_t SplitText::SplitLocator::process(std::shared_ptr<io::BaseStream> stream) {
  // iterate through the file
  uint8_t buffer[1024] = { 0 };
  int len_read = 0;
  size_t offset = 0;
  size_t file_offset = 0;

  do {
    len_read = stream->read(buffer, 1024);
    // interact with the buffer
    if (len_read <= 0) {
      break;
    }
    int split_size = splits_.size();
    for (int i = 0; i < len_read; i++) {
      Split offsetInfo = { 0, 0, 0, 0 };
      if (buffer[i] == 13 || buffer[i] == 10) {
        auto diff = offset-file_offset;
        offsetInfo.trimmedLength = diff + i;
        if (buffer[i] == 13 && i + 1 < len_read && buffer[i + 1] == 10) {
          i++;
        }
        offsetInfo.start = offset;
        offsetInfo.length = diff + i + 1;
        offset += i;
      }
      splits_.emplace_back(std::move(offsetInfo));

    }
    if (split_size != splits_.size())
      offset += len_read;
    file_offset += len_read;
  } while (len_read > 0);
  return offset;
}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#endif
