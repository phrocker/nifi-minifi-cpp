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

#include "io/BaseStream.h"
#include "Site2SitePeer.h"
#include "Site2SiteClientProtocol.h"
#include <uuid/uuid.h>
#include "core/logging/LogAppenders.h"
#include "core/logging/BaseLogger.h"
#include "SiteToSiteHelper.h"
#include <algorithm>
#include <string>
#include <memory>
#include "properties/Configure.h"
#include "../TestBase.h"
#define FMT_DEFAULT fmt_lower

using namespace org::apache::nifi::minifi::io;

void sunny_path(SiteToSiteResponder &collector) {

  char a = 0x14;
  std::string resp_code;
  resp_code.insert(resp_code.begin(), a);
  collector.push_response(resp_code);
  resp_code.insert(resp_code.begin(), 'R');
  collector.push_response(resp_code);
  resp_code.insert(resp_code.begin(), 'C');
  collector.push_response(resp_code);
  resp_code.insert(resp_code.begin(), (char) 0x01);
  collector.push_response(resp_code);
  resp_code.insert(resp_code.begin(), (char) 0x01);
  collector.push_response(resp_code);
}

TEST_CASE("TestSiteToSiteVerifyNegotiation", "[S2S2]") {

  std::ostringstream oss;

  std::unique_ptr<logging::BaseLogger> outputLogger = std::unique_ptr<
      logging::BaseLogger>(
      new org::apache::nifi::minifi::core::logging::OutputStreamAppender(
          std::cout, minifi::Configure::getConfigure()));
  std::shared_ptr<logging::Logger> logger = logging::Logger::getLogger();
  logger->updateLogger(std::move(outputLogger));
  logger->setLogLevel("off");

  SiteToSiteResponder collector;

  sunny_path(collector);

  std::unique_ptr<minifi::Site2SitePeer> peer = std::unique_ptr<
      minifi::Site2SitePeer>(
      new minifi::Site2SitePeer(
          std::unique_ptr<DataStream>(new BaseStream(&collector)), "fake_host",
          65433));

  minifi::Site2SiteClientProtocol protocol(std::move(peer));

  std::string uuid_str = "C56A4180-65AA-42EC-A945-5FD21DEC0538";

  uuid_t fakeUUID;

  uuid_parse(uuid_str.c_str(), fakeUUID);

  protocol.setPortId(fakeUUID);

  bool bootstrap_succeed = protocol.bootstrap();

  REQUIRE(collector.get_next_client_response() == "NiFi");
  collector.get_next_client_response();
  REQUIRE(collector.get_next_client_response() == "SocketFlowFileProtocol");
  collector.get_next_client_response();
  collector.get_next_client_response();
  collector.get_next_client_response();
  collector.get_next_client_response();
  REQUIRE(collector.get_next_client_response() == "nifi://fake_host:65433");
  collector.get_next_client_response();
  collector.get_next_client_response();
  REQUIRE(collector.get_next_client_response() == "GZIP");
  collector.get_next_client_response();
  REQUIRE(collector.get_next_client_response() == "false");
  collector.get_next_client_response();
  REQUIRE(collector.get_next_client_response() == "PORT_IDENTIFIER");
  collector.get_next_client_response();
  REQUIRE(
      collector.get_next_client_response()
          == "c56a4180-65aa-42ec-a945-5fd21dec0538");
  collector.get_next_client_response();
  REQUIRE(collector.get_next_client_response() == "REQUEST_EXPIRATION_MILLIS");
  collector.get_next_client_response();
  REQUIRE(collector.get_next_client_response() == "30000");
  collector.get_next_client_response();
  REQUIRE(collector.get_next_client_response() == "NEGOTIATE_FLOWFILE_CODEC");

  REQUIRE(true == bootstrap_succeed);
}

TEST_CASE("TestSiteToSiteVerifyNegotiationFail", "[S2S2]") {

  std::ostringstream oss;

  std::unique_ptr<logging::BaseLogger> outputLogger = std::unique_ptr<
      logging::BaseLogger>(
      new org::apache::nifi::minifi::core::logging::OutputStreamAppender(oss,
                                                                         0));
  std::shared_ptr<logging::Logger> logger = logging::Logger::getLogger();
  logger->updateLogger(std::move(outputLogger));
  //logger->setLogLevel("off");

  SiteToSiteResponder collector;

  char a = 0xFF;
  std::string resp_code;
  resp_code.insert(resp_code.begin(), a);
  collector.push_response(resp_code);
  collector.push_response(resp_code);

  std::unique_ptr<minifi::Site2SitePeer> peer = std::unique_ptr<
      minifi::Site2SitePeer>(
      new minifi::Site2SitePeer(
          std::unique_ptr<DataStream>(new BaseStream(&collector)), "fake_host",
          65433));

  minifi::Site2SiteClientProtocol protocol(std::move(peer));

  std::string uuid_str = "C56A4180-65AA-42EC-A945-5FD21DEC0538";

  uuid_t fakeUUID;

  uuid_parse(uuid_str.c_str(), fakeUUID);

  protocol.setPortId(fakeUUID);

  REQUIRE(false == protocol.bootstrap());

}
