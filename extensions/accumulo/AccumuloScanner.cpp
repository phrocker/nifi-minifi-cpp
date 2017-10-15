/**
 * @file AccumuloScanner.cpp
 * AccumuloScanner class implementation
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
#include "AccumuloScanner.h"
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <data/constructs/rfile/RFile.h>
#include <data/constructs/KeyValue.h>
#include <data/streaming/EndianTranslation.h>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include "utils/TimeUtil.h"
#include "utils/StringUtils.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {
core::Property AccumuloScanner::Zookeepers("Zookeepers", "List of zookeepers", "info");
core::Property AccumuloScanner::InstanceId("InstanceId", "Accumulo instance identifier", "info");
core::Property AccumuloScanner::Username("Username", "Username ", "info");
core::Property AccumuloScanner::Password("Password", "Password", "info");
core::Property AccumuloScanner::Table("Table", "table we are bulk importing into", "info");
core::Relationship AccumuloScanner::Success("success", "All files are routed to success");

void AccumuloScanner::onSchedule(std::shared_ptr<core::ProcessContext> context, std::shared_ptr<core::ProcessSessionFactory> sessionFactory) {

  if (!context->getProperty(Zookeepers.getName(), zookeepers_)) {
    std::cout << "no zk " << std::endl;
    return;
  }

  if (!context->getProperty(InstanceId.getName(), instanceid_)) {
    std::cout << "no InstanceId " << std::endl;
    return;
  }

  if (!context->getProperty(Username.getName(), username_)) {
    std::cout << "no username " << std::endl;
    return;
  }

  if (!context->getProperty(Password.getName(), password_)) {
    std::cout << "no password " << std::endl;
    return;
  }

  if (!context->getProperty(Table.getName(), table_)) {
    std::cout << "no table " << std::endl;
    return;
  }


  try {
    cclient::impl::Configuration *confC = (new cclient::impl::Configuration());
    confC->set("FILE_SYSTEM_ROOT", "/accumulo");

    instance = std::unique_ptr<cclient::data::zookeeper::ZookeeperInstance>(
        new cclient::data::zookeeper::ZookeeperInstance(instanceid_, zookeepers_, 1000, std::unique_ptr<cclient::impl::Configuration>(confC)));
  } catch (...) {
    logger_->log_info("Exit Accumulo Scanner: %s");
    return;
  }

  cclient::data::security::AuthInfo creds(username_, password_, instance->getInstanceId());

  try {
    master = std::unique_ptr<interconnect::MasterConnect>(new interconnect::MasterConnect(&creds, instance.get()));
  } catch (...) {
    logger_->log_info("Exit Accumulo Scanner: %s");
    return;
  }
  if (instance == nullptr || master == nullptr || table_.empty()) {
    logger_->log_info("Exit Accumulo Writer: missing config");
    return;
  }

  ops = master->tableOps(table_);

}
void AccumuloScanner::initialize() {
  // Set the supported properties
  std::set<core::Property> properties;
  properties.insert(Zookeepers);
  properties.insert(InstanceId);
  properties.insert(Username);
  properties.insert(Password);
  properties.insert(Table);
  setSupportedProperties(properties);
  // Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert(Success);


  setSupportedRelationships(relationships);
}

void AccumuloScanner::onTrigger(core::ProcessContext *context, core::ProcessSession *session) {

  std::lock_guard<std::mutex> lock(scanner_mutex_);
  std::cout << "on trigger" << std::endl;
  std::cout << "2get rf for " << table_ << std::endl;
  cclient::data::security::Authorizations scanAuths;
  scanAuths.addAuthorization("00000001");
  std::cout << "3get rf for " << table_ << std::endl;
  auto scanner = ops->createScanner(&scanAuths, 3);
  std::cout << "3get rf for " << table_ << std::endl;
  std::shared_ptr<cclient::data::Key> startkey = std::make_shared<cclient::data::Key>();
  startkey->setRow("a", 1);
  std::shared_ptr<cclient::data::Key> stopKey = std::make_shared<cclient::data::Key>();
  stopKey->setRow("c", 1);
  cclient::data::Range *range = new cclient::data::Range(startkey,true,stopKey,false);
  std::cout << "4get rf for " << table_ << std::endl;
  scanner->addRange(std::unique_ptr<cclient::data::Range>(range));
  std::cout << "5get rf for " << table_ << std::endl;

  auto results = scanner->getResultSet();
  std::cout << "6get rf for " << table_ << std::endl;
  for (auto keyvalue : *results) {
    auto value = keyvalue->getValue();

    auto flow = session->create();
    auto data = value->getValue();
    ValueCallback callback(data.first,data.second);
    session->write(flow, &callback);

    session->transfer(flow, Success);
  }



// Transfer to the relationship

}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
