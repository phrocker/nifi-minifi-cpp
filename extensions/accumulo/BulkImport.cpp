/**
 * @file BulkImport.cpp
 * BulkImport class implementation
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
#include "BulkImport.h"
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
core::Property BulkImport::Zookeepers("Zookeepers", "List of zookeepers", "info");
core::Property BulkImport::InstanceId("InstanceId", "Accumulo instance identifier", "info");
core::Property BulkImport::Username("Username", "Username ", "info");
core::Property BulkImport::Password("Password", "Password", "info");
core::Property BulkImport::Table("Table", "table we are bulk importing into", "info");
core::Relationship BulkImport::Success("success", "All files are routed to success");

void BulkImport::onSchedule(std::shared_ptr<core::ProcessContext> context, std::shared_ptr<core::ProcessSessionFactory> sessionFactory) {


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

}
void BulkImport::initialize() {
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

void BulkImport::onTrigger(core::ProcessContext *context, core::ProcessSession *session) {

  std::shared_ptr<core::FlowFile> flow = session->get();

  if (!flow) {
    logger_->log_info("Exit Accumulo Writer: no flow");
    return;
  }

  std::unique_ptr<cclient::data::zookeeper::ZookeeperInstance> instance;

  std::unique_ptr<interconnect::MasterConnect> master;

  try {
    cclient::impl::Configuration *confC = (new cclient::impl::Configuration());
    confC->set("FILE_SYSTEM_ROOT", "/accumulo");

    instance = std::unique_ptr<cclient::data::zookeeper::ZookeeperInstance>(
        new cclient::data::zookeeper::ZookeeperInstance(instanceid_, zookeepers_, 1000, std::unique_ptr<cclient::impl::Configuration>(confC)));
  } catch (cclient::exceptions::ClientException ce) {
    logger_->log_info("Exit Accumulo Writer: %s", ce.what());
    std::cout << "Could not connect to ZK. Error: " << ce.what() << std::endl;
    return;
  }

  cclient::data::security::AuthInfo creds(username_, password_, instance->getInstanceId());

  try {
    master = std::unique_ptr<interconnect::MasterConnect>(new interconnect::MasterConnect(&creds, instance.get()));
  } catch (cclient::exceptions::ClientException ce) {
    logger_->log_info("Exit Accumulo Writer: %s", ce.what());
    return;
  }
  if (instance == nullptr || master == nullptr || table_.empty()) {
    logger_->log_info("Exit Accumulo Writer: missing config");
    return;
  }

  std::string rfiledir;

  flow->getAttribute("Rfile", rfiledir);
  logger_->log_info("get rf %s",rfiledir);
  if (!rfiledir.empty()) {


    std::cout << "get rf for " << table_ << std::endl;
    std::unique_ptr<interconnect::AccumuloTableOperations> ops = master->tableOps(table_);
    if (!ops->exists()){
      ops->create();
    }
    std::string faildir = rfiledir + "f";
    mkdir(faildir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
    std::string source = "file://" + rfiledir;
    std::string source_fail = "file://" + faildir;
    if ( ops->import(source, source_fail, true)){
      std::cout << "oh yeah" << std::endl;
    }
    std::cout << "import " << source << std::endl;
    if (ops->compact("a", "z", true)) {

    }
    else{
      std::cout << "BI fail " << source << std::endl;
    }

  }
// Transfer to the relationship
  session->transfer(flow, Success);
}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
