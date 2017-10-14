/**
 * @file HttpSiteToSiteClient.h
 * HttpSiteToSiteClient class declaration
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
#ifndef __SITE2SITE_CLIENT_PROTOCOL_H__
#define __SITE2SITE_CLIENT_PROTOCOL_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <string>
#include <errno.h>
#include <chrono>
#include <set>
#include <thread>
#include <algorithm>
#include <uuid/uuid.h>

#include "sitetosite/SiteToSite.h"
#include "sitetosite/SiteToSiteClient.h"
#include "core/Property.h"
#include "properties/Configure.h"
#include "FlowFileRecord.h"
#include "core/logging/LoggerConfiguration.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"
#include "io/CRCStream.h"
#include "sitetosite/Peer.h"
#include "utils/Id.h"
#include "../client/HTTPClient.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace sitetosite {

// Respond Code Sequence Pattern
static const uint8_t CODE_SEQUENCE_VALUE_1 = (uint8_t) 'R';
static const uint8_t CODE_SEQUENCE_VALUE_2 = (uint8_t) 'C';

/**
 * Enumeration of Properties that can be used for the Site-to-Site Socket
 * Protocol.
 */
typedef enum {
  /**
   * Boolean value indicating whether or not the contents of a FlowFile should
   * be GZipped when transferred.
   */
  GZIP,
  /**
   * The unique identifier of the port to communicate with
   */
  PORT_IDENTIFIER,
  /**
   * Indicates the number of milliseconds after the request was made that the
   * client will wait for a response. If no response has been received by the
   * time this value expires, the server can move on without attempting to
   * service the request because the client will have already disconnected.
   */
  REQUEST_EXPIRATION_MILLIS,
  /**
   * The preferred number of FlowFiles that the server should send to the
   * client when pulling data. This property was introduced in version 5 of
   * the protocol.
   */
  BATCH_COUNT,
  /**
   * The preferred number of bytes that the server should send to the client
   * when pulling data. This property was introduced in version 5 of the
   * protocol.
   */
  BATCH_SIZE,
  /**
   * The preferred amount of time that the server should send data to the
   * client when pulling data. This property was introduced in version 5 of
   * the protocol. Value is in milliseconds.
   */
  BATCH_DURATION,
  MAX_HANDSHAKE_PROPERTY
} HandshakeProperty;

// HandShakeProperty Str
static const char *HandShakePropertyStr[MAX_HANDSHAKE_PROPERTY] = {
/**
 * Boolean value indicating whether or not the contents of a FlowFile should
 * be GZipped when transferred.
 */
"GZIP",
/**
 * The unique identifier of the port to communicate with
 */
"PORT_IDENTIFIER",
/**
 * Indicates the number of milliseconds after the request was made that the
 * client will wait for a response. If no response has been received by the
 * time this value expires, the server can move on without attempting to
 * service the request because the client will have already disconnected.
 */
"REQUEST_EXPIRATION_MILLIS",
/**
 * The preferred number of FlowFiles that the server should send to the
 * client when pulling data. This property was introduced in version 5 of
 * the protocol.
 */
"BATCH_COUNT",
/**
 * The preferred number of bytes that the server should send to the client
 * when pulling data. This property was introduced in version 5 of the
 * protocol.
 */
"BATCH_SIZE",
/**
 * The preferred amount of time that the server should send data to the
 * client when pulling data. This property was introduced in version 5 of
 * the protocol. Value is in milliseconds.
 */
"BATCH_DURATION" };

/**
 * Represents a piece of data that is to be sent to or that was received from a
 * NiFi instance.
 */
class DataPacket {
 public:
  DataPacket(SiteToSiteClient *protocol, const std::shared_ptr<Transaction> &transaction, std::map<std::string, std::string> attributes, const std::string &payload)
      : payload_(payload) {
    _protocol = protocol;
    _size = 0;
    transaction_ = transaction;
    _attributes = attributes;
  }
  std::map<std::string, std::string> _attributes;
  uint64_t _size;
  SiteToSiteClient *_protocol;
  std::shared_ptr<Transaction> transaction_;
  const std::string & payload_;

};

/**
 * Site2Site Peer
 */
typedef struct Site2SitePeerStatus {
  std::string host_;
  int port_;bool isSecure_;
} Site2SitePeerStatus;

// HttpSiteToSiteClient Class
class HttpSiteToSiteClient : public sitetosite::SiteToSiteClient {
 public:

  /*!
   * Create a new http protocol
   */
  HttpSiteToSiteClient(std::string name, uuid_t uuid = 0)
      : SiteToSiteClient(),
        logger_(logging::LoggerFactory<HttpSiteToSiteClient>::getLogger()) {

  }

  /*!
   * Create a new http protocol
   */
  HttpSiteToSiteClient(std::unique_ptr<SiteToSitePeer> peer)
      : SiteToSiteClient(),
        logger_(logging::LoggerFactory<HttpSiteToSiteClient>::getLogger()) {
    peer_ = std::move(peer);

  }
  // Destructor
  virtual ~HttpSiteToSiteClient() {

  }

 public:

  void setPeer(std::unique_ptr<SiteToSitePeer> peer) {
    peer_ = std::move(peer);
  }

  virtual bool getPeerList(std::vector<PeerStatus> &peers);

  virtual bool establish() {
    return true;
  }

  virtual std::shared_ptr<Transaction> createTransaction(std::string &transactionID, TransferDirection direction);

  virtual bool receiveFlowFiles(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session);
  // Transfer flow files for the process session
  virtual bool transferFlowFiles(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session);
  //! Transfer string for the process session
  virtual bool transmitPayload(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session, const std::string &payload,
                               std::map<std::string, std::string> attributes);
  // deleteTransaction
  void deleteTransaction(std::string transactionID);

 protected:

  const std::string getBaseURI() {
    std::cout << "host name is " << peer_->getHostName() << std::endl;
    std::string uri = "http://";
    uri.append(peer_->getHostName());
    uri.append(":");
    uri.append(std::to_string(peer_->getPort()));
    uri.append("/nifi-api/");
    return uri;
  }

  std::unique_ptr<utils::HTTPClient> create_http_client(const std::string &uri, const std::string method = "POST") {
    std::unique_ptr<utils::HTTPClient> http_client_ = std::unique_ptr<utils::HTTPClient>(new minifi::utils::HTTPClient(uri, nullptr));
    http_client_->initialize(method, uri, nullptr);
    return http_client_;
  }

 private:

  std::shared_ptr<logging::Logger> logger_;

  std::set<std::shared_ptr<Transaction>> known_transactions_;

  // Prevent default copy constructor and assignment operation
  // Only support pass by reference or pointer
  HttpSiteToSiteClient(const HttpSiteToSiteClient &parent);
  HttpSiteToSiteClient &operator=(const HttpSiteToSiteClient &parent);
  static std::shared_ptr<utils::IdGenerator> id_generator_;
};

} /* namespace sitetosite */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
#endif
