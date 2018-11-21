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

#include <sys/stat.h>
#undef NDEBUG
#include <cassert>
#include <utility>
#include <chrono>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>
#include <iostream>
#include <sstream>
#include "HTTPClient.h"
#include "processors/InvokeHTTP.h"
#include "TestBase.h"
#include "utils/StringUtils.h"
#include "core/Core.h"
#include "core/logging/Logger.h"
#include "core/ProcessGroup.h"
#include "core/yaml/YamlConfiguration.h"
#include "FlowController.h"
#include "properties/Configure.h"
#include "unit/ProvenanceTestHelper.h"
#include "io/StreamFactory.h"
#include "CivetServer.h"
#include "RemoteProcessorGroupPort.h"
#include "core/ConfigurableComponent.h"
#include "controllers/SSLContextService.h"
#include "c2/C2Agent.h"
#include "protocols/RESTReceiver.h"
#include "HTTPIntegrationBase.h"
#include "processors/LogAttribute.h"
#include "CoapC2Protocol.h"
#include "CoapServer.h"

class Responder : public CivetHandler {
 public:
  explicit Responder(bool isSecure)
      : isSecure(isSecure) {
  }
  bool handlePost(CivetServer *server, struct mg_connection *conn) {
    std::string resp = "{\"operation\" : \"heartbeat\", \"requested_operations\" : [{ \"operationid\" : 41, \"operation\" : \"stop\", \"name\" : \"invoke\"  }, "
        "{ \"operationid\" : 42, \"operation\" : \"stop\", \"name\" : \"FlowController\"  } ]}";
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: "
              "text/plain\r\nContent-Length: %lu\r\nConnection: close\r\n\r\n",
              resp.length());
    mg_printf(conn, "%s", resp.c_str());
    return true;
  }

 protected:
  bool isSecure;
};

class VerifyCoAPServer : public HTTPIntegrationBase {
 public:
  explicit VerifyCoAPServer(bool isSecure)
      : isSecure(isSecure) {
    char format[] = "/tmp/ssth.XXXXXX";
    dir = testController.createTempDirectory(format);
  }

  void testSetup() {
    LogTestController::getInstance().setDebug<utils::HTTPClient>();
    LogTestController::getInstance().setOff<processors::InvokeHTTP>();
    LogTestController::getInstance().setDebug<minifi::c2::RESTReceiver>();
    LogTestController::getInstance().setDebug<minifi::c2::C2Agent>();
    LogTestController::getInstance().setTrace<minifi::coap::c2::CoapProtocol>();
    LogTestController::getInstance().setOff<processors::LogAttribute>();
    LogTestController::getInstance().setOff<minifi::core::ProcessSession>();
    std::fstream file;
    ss << dir << "/" << "tstFile.ext";
    file.open(ss.str(), std::ios::out);
    file << "tempFile";
    file.close();
  }

  void cleanup() {
    unlink(ss.str().c_str());
  }

  void runAssertions() {


    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    assert(LogTestController::getInstance().contains("Import offset 0") == true);

    assert(LogTestController::getInstance().contains("Outputting success and response") == true);

  }

  void queryRootProcessGroup(std::shared_ptr<core::ProcessGroup> pg) {
    std::shared_ptr<core::Processor> proc = pg->findProcessor("invoke");
    assert(proc != nullptr);

    std::shared_ptr<minifi::processors::InvokeHTTP> inv = std::dynamic_pointer_cast<minifi::processors::InvokeHTTP>(proc);

    assert(inv != nullptr);
    std::string url = "";
    inv->getProperty(minifi::processors::InvokeHTTP::URL.getName(), url);

    std::string port, scheme, path;

    parse_http_components(url, port, scheme, path);
    auto new_port_str = std::to_string(std::stoi(port)+1);
    std::cout << "created port " << new_port_str << std::endl;
    server = std::unique_ptr<minifi::coap::CoapServer>(new minifi::coap::CoapServer("127.0.0.1",std::stoi(port)+1));

    server->add_endpoint(minifi::coap::METHOD::POST, [](minifi::coap::CoapQuery)->int{
        std::cout << "got heartbeat!" << std::endl;
        return 0;

      });


    server->add_endpoint("heartbeat",minifi::coap::METHOD::GET, [](minifi::coap::CoapQuery)->int{
      std::cout << "got heartbeat!" << std::endl;
      return 0;

    });


    server->add_endpoint("heartbeat",minifi::coap::METHOD::POST, [](minifi::coap::CoapQuery)->int{
      std::cout << "got heartbeat!" << std::endl;
      return 0;

    });


    server->add_endpoint("acknowledge",minifi::coap::METHOD::POST, [](minifi::coap::CoapQuery)->int{
      std::cout << "got acknowledge!" << std::endl;
      return 0;

    });

    server->start();
    configuration->set("c2.enable", "true");
    configuration->set("c2.agent.class", "test");
    configuration->set("nifi.c2.root.classes","DeviceInfoNode,AgentInformation,FlowInformation,RepositoryMetrics");
    configuration->set("nifi.c2.agent.protocol.class", "CoapProtocol");
    configuration->set("nifi.c2.agent.coap.host","127.0.0.1");
    configuration->set("nifi.c2.agent.coap.port", new_port_str);
    configuration->set("c2.agent.heartbeat.period", "10");
    configuration->set("c2.rest.listener.heartbeat.rooturi", path);
  }

 protected:
  std::unique_ptr<minifi::coap::CoapServer> server;
  bool isSecure;
  char *dir;
  std::stringstream ss;
  TestController testController;
};

int main(int argc, char **argv) {
  std::string key_dir, test_file_location, url;
  if (argc > 1) {
    test_file_location = argv[1];
    key_dir = argv[2];
  }

  bool isSecure = false;
  if (url.find("https") != std::string::npos) {
    isSecure = true;
  }


  VerifyCoAPServer harness(isSecure);

  harness.setKeyDir(key_dir);

  harness.run(test_file_location);

  return 0;
}
