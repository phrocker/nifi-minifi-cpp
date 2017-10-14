/**
 * @file Site2SiteProtocol.cpp
 * Site2SiteProtocol class implementation
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
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <map>
#include <string>
#include <memory>
#include <thread>
#include <random>
#include <iostream>
#include <vector>

#include "PeersEntity.h"
#include "HttpTransaction.h"
#include "HttpProtocol.h"
#include "io/CRCStream.h"
#include "sitetosite/Peer.h"
#include "io/validation.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace sitetosite {

std::shared_ptr<utils::IdGenerator> HttpSiteToSiteClient::id_generator_ = utils::IdGenerator::getIdGenerator();

std::shared_ptr<Transaction> HttpSiteToSiteClient::createTransaction(std::string &transactionID, TransferDirection direction) {
  std::cout << "attempting to build a transaction" << std::endl;
  std::string dir_str = direction == SEND ? "input-ports" : "output-ports";
  std::stringstream uri;
  uri << getBaseURI() << "data-transfer/" << dir_str << "/" << getPortId() << "/transactions";
  std::cout << "attempting to build a transaction to " << uri.str() << std::endl;
  auto client = create_http_client(uri.str(), "POST");

  client->appendHeader("x-nifi-site-to-site-protocol-version: 1");

  client->setConnectionTimeout(1);
  client->setReadTimeout(1);

  client->setContentType("application/json");
  client->appendHeader("Accept: application/json");
  client->setVerbose();
  client->setUseChunkedEncoding();
  client->setPostFields("");
  client->submit();

  std::cout << "succeed!" << std::endl;
  if (client->getResponseCode() == 201) {
    std::cout << "succeed2!" << std::endl;
    // parse the headers
    auto headers = client->getParsedHeaders();
    auto intent_name = headers["x-location-uri-intent"];
    std::cout << "intent name is " << intent_name << std::endl;
    if (intent_name == "transaction-url") {
      auto url = headers["Location"];

      if (IsNullOrEmpty(&url)) {
        logger_->log_info("Location is empty");
      } else {
        org::apache::nifi::minifi::io::CRCStream<SiteToSitePeer> crcstream(peer_.get());
        auto transaction = std::make_shared<HttpTransaction>(direction, crcstream);
        transaction->initialize(this, url);
        return transaction;
      }
    } else {
      std::cout << "oh no  -" << intent_name << "-" << std::endl;
    }
  } else {
    std::cout << "response code is " << client->getResponseCode();
  }
  return nullptr;
}

bool HttpSiteToSiteClient::getPeerList(std::vector<PeerStatus> &peers) {
  std::stringstream uri;
  uri << getBaseURI() << "site-to-site/peers";

  auto client = create_http_client(uri.str(), "GET");

  client->appendHeader("x-nifi-site-to-site-protocol-version: 1");

  client->submit();

  if (client->getResponseCode() == 200) {
    std::cout << "attempting to parse" << std::endl;
    sitetosite::PeersEntity::parse(std::string(client->getResponseBody().data(), client->getResponseBody().size()), port_id_, peers);
  }
  return false;
}

bool HttpSiteToSiteClient::receiveFlowFiles(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) {
  return false;
}
// Transfer flow files for the process session
bool HttpSiteToSiteClient::transferFlowFiles(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) {

  std::string transactionId;
  auto transaction = createTransaction(transactionId, SEND);
  if (transaction == nullptr){
    session->rollback();
    context->yield();
    return false;
  }
  known_transactions_.insert(transaction);

  return false;
}
//! Transfer string for the process session
bool HttpSiteToSiteClient::transmitPayload(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session, const std::string &payload,
                                           std::map<std::string, std::string> attributes) {
  return false;
}
// deleteTransaction
void HttpSiteToSiteClient::deleteTransaction(std::string transactionID) {
}

} /* namespace sitetosite */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
