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

#include "processors/MergeContent.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

static core::Property MergeStrategy;
static core::Property MergeFormat;
static core::Property AttributeStrategy;
static core::Property DelimiterStrategy;

core::Property MergeContent::MergeStrategy("Merge Strategy", "Strategy used to merge flow files", "Bin-Packing Algorithm");
core::Property MergeContent::MergeFormat("Merge Format", "Method of concatenation", "Binary Concatenation");
core::Property MergeContent::AttributeStrategy("Attribute Strategy", "Strategy for Attribute merging", "Keep Only Common Attributes");
core::Property MergeContent::DelimiterStrategy("Delimiter Strategy", "Determines how the resulting bundle points to the original flowfiles", "Filename");

core::Relationship MergeContent::Original("original", "Flowfiles used to create a bundle");
core::Relationship MergeContent::Failure("failure", "Flowfiles that would have been used to create a bundle");
core::Relationship MergeContent::Merged("merged", "Flowfile bundle created from MergeContent");

void MergeContent::onTrigger(core::ProcessContext *context, core::ProcessSession *session) {
}
void MergeContent::initialize() {
  logger_->log_info("Initializing MergeContent");

  // Set the supported properties
  std::set<core::Property> properties;

  properties.insert(MergeStrategy);
  properties.insert(MergeFormat);
  properties.insert(AttributeStrategy);
  properties.insert(DelimiterStrategy);

  setSupportedProperties(properties);
  // Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert(Original);
  relationships.insert(Failure);
  relationships.insert(Merged);
  setSupportedRelationships(relationships);
}

void MergeContent::onSchedule(core::ProcessContext *context, core::ProcessSessionFactory *sessionFactory) {

}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
