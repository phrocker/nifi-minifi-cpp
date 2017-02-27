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
#include <uuid/uuid.h>
#include <fstream>
#include "FlowController.h"
#include "ProvenanceTestHelper.h"
#include "../TestBase.h"
#include "core/logging/LogAppenders.h"
#include "core/logging/BaseLogger.h"
#include "processors/GetFile.h"
#include "core/core.h"
#include "core/Record.h"
#include "core/Processor.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"
#include "core/ProcessorNode.h"

TEST_CASE("Test Creation of GetFile", "[getfileCreate]") {
  org::apache::nifi::minifi::processors::GetFile processor("processorname");
  REQUIRE(processor.getName() == "processorname");
}

TEST_CASE("Test Find file", "[getfileCreate2]") {

  TestController testController;

  testController.enableDebug();

  ProvenanceTestRepository repo;
  TestFlowController controller(repo);
  minifi::FlowControllerFactory::getFlowController(
      dynamic_cast<minifi::FlowController*>(&controller));

  std::shared_ptr<core::Processor> processor = std::make_shared<
      org::apache::nifi::minifi::processors::GetFile>("getfileCreate2");

  char format[] = "/tmp/gt.XXXXXX";
  char *dir = testController.createTempDirectory(format);

  uuid_t processoruuid;
  REQUIRE(true == processor->getUUID(processoruuid));

  std::shared_ptr<minifi::Connection> connection = std::make_shared<
      minifi::Connection>("getfileCreate2Connection");
  connection->setRelationship(core::Relationship("success", "description"));

  // link the connections so that we can test results at the end for this
  connection->setSource(processor);

  connection->setSourceUUID(processoruuid);
  connection->setDestinationUUID(processoruuid);

  processor->addConnection(connection);
  REQUIRE(dir != NULL);

  core::ProcessorNode node(processor);

  core::ProcessContext context(node);
  context.setProperty(org::apache::nifi::minifi::processors::GetFile::Directory,
                      dir);
  core::ProcessSession session(&context);

  REQUIRE(processor->getName() == "getfileCreate2");

  std::shared_ptr<core::Record> record;
  processor->setScheduledState(core::ScheduledState::RUNNING);
  processor->onTrigger(&context, &session);

  minifi::ProvenanceReporter *reporter = session.getProvenanceReporter();
  std::set<minifi::ProvenanceEventRecord*> records = reporter->getEvents();
  record = session.get();
  REQUIRE(record == nullptr);
  REQUIRE(records.size() == 0);

  std::fstream file;
  std::stringstream ss;
  ss << dir << "/" << "tstFile.ext";
  file.open(ss.str(), std::ios::out);
  file << "tempFile";
  file.close();

  processor->incrementActiveTasks();
  processor->setScheduledState(core::ScheduledState::RUNNING);
  processor->onTrigger(&context, &session);
  unlink(ss.str().c_str());
  rmdir(dir);
  reporter = session.getProvenanceReporter();

  records = reporter->getEvents();

  for (minifi::ProvenanceEventRecord *provEventRecord : records) {
    REQUIRE(provEventRecord->getComponentType() == processor->getName());
  }
  session.commit();
  std::shared_ptr<core::Record> ffr = session.get();

  ffr->getResourceClaim()->decreaseFlowFileRecordOwnedCount();
  REQUIRE(2 == repo.getRepoMap().size());

  for (auto entry : repo.getRepoMap()) {
    minifi::ProvenanceEventRecord newRecord;
    newRecord.DeSerialize((uint8_t*) entry.second.data(),
                          entry.second.length());

    bool found = false;
    for (auto provRec : records) {
      if (provRec->getEventId() == newRecord.getEventId()) {
        REQUIRE(provRec->getEventId() == newRecord.getEventId());
        REQUIRE(provRec->getComponentId() == newRecord.getComponentId());
        REQUIRE(provRec->getComponentType() == newRecord.getComponentType());
        REQUIRE(provRec->getDetails() == newRecord.getDetails());
        REQUIRE(provRec->getEventDuration() == newRecord.getEventDuration());
        found = true;
        break;
      }
    }
    if (!found)
      throw std::runtime_error("Did not find record");

  }

}

