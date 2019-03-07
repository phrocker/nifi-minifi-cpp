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
#include <map>
#include <memory>
#include <utility>
#include <string>
#include <set>
#include "FlowController.h"
#include "../TestBase.h"
#include "core/Core.h"
#include "FlowFile.h"
#include "../unit/ProvenanceTestHelper.h"
#include "core/Processor.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"
#include "core/ProcessorNode.h"
#include "processors/TailFile.h"
#include "processors/LogAttribute.h"
#include <iostream>
#include "CaptureRTSPFrame.h"


TEST_CASE("CaptureRTSPFrame Dyer Home", "[opencvtest1]") {

    TestController testController;

    LogTestController::getInstance().setTrace<minifi::processors::CaptureRTSPFrame>();
    LogTestController::getInstance().setDebug<core::ProcessSession>();
    LogTestController::getInstance().setDebug<minifi::processors::LogAttribute>();

    std::shared_ptr<TestPlan> plan = testController.createPlan();

    std::shared_ptr<core::Processor> captureRTSP = plan->addProcessor("CaptureRTSPFrame", "CaptureRTSPFrame");

    plan->addProcessor("LogAttribute", "logattribute", core::Relationship("Success", "description"), true);

    //plan->setProperty(captureRTSP, org::apache::nifi::minifi::processors::GetFile::Directory.getName(), dir);
    testController.runSession(plan, false);
    auto records = plan->getProvenanceRecords();
    std::shared_ptr<core::FlowFile> record = plan->getCurrentFlowFile();
    REQUIRE(record == nullptr);
    REQUIRE(records.size() == 0);

    plan->reset();
    testController.runSession(plan, false);

    //unlink(ss.str().c_str());

    records = plan->getProvenanceRecords();
    record = plan->getCurrentFlowFile();
    testController.runSession(plan, false);

    records = plan->getProvenanceRecords();
    record = plan->getCurrentFlowFile();

    //REQUIRE(true == LogTestController::getInstance().contains("GPSD client scheduled"));
    LogTestController::getInstance().reset();
}