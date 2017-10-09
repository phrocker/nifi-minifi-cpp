/**
 * @file Site2SiteClientProtocol.h
 * Site2SiteClientProtocol class declaration
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
#include <thread>
#include <algorithm>
#include <uuid/uuid.h>

#include "SiteToSite.h"
#include "SiteToSiteClient.h"
#include "core/Property.h"
#include "properties/Configure.h"
#include "FlowFileRecord.h"
#include "core/logging/LoggerConfiguration.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"
#include "io/CRCStream.h"
#include "SiteToSitePeer.h"
#include "utils/Id.h"

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

class Site2SiteClientProtocol;

// Transaction Class
class Transaction {
  friend class Site2SiteClientProtocol;
 public:
  // Constructor
  /*!
   * Create a new transaction
   */
  explicit Transaction(TransferDirection direction, org::apache::nifi::minifi::io::CRCStream<SiteToSitePeer> &stream)
      : crcStream(std::move(stream)) {
    _state = TRANSACTION_STARTED;
    _direction = direction;
    _dataAvailable = false;
    _transfers = 0;
    _bytes = 0;

    char uuidStr[37];

    // Generate the global UUID for the transaction
    id_generator_->generate(_uuid);
    uuid_unparse_lower(_uuid, uuidStr);
    _uuidStr = uuidStr;
  }
  // Destructor
  virtual ~Transaction() {
  }
  // getUUIDStr
  std::string getUUIDStr() {
    return _uuidStr;
  }
  // getState
  TransactionState getState() {
    return _state;
  }
  // isDataAvailable
  bool isDataAvailable() {
    return _dataAvailable;
  }
  // setDataAvailable()
  void setDataAvailable(bool value) {
    _dataAvailable = value;
  }
  // getDirection
  TransferDirection getDirection() {
    return _direction;
  }
  // getCRC
  long getCRC() {
    return crcStream.getCRC();
  }
  // updateCRC
  void updateCRC(uint8_t *buffer, uint32_t length) {
    crcStream.updateCRC(buffer, length);
  }

  org::apache::nifi::minifi::io::CRCStream<SiteToSitePeer> &getStream() {
    return crcStream;
  }

  Transaction(const Transaction &parent) = delete;
  Transaction &operator=(const Transaction &parent) = delete;

 protected:

 private:

  org::apache::nifi::minifi::io::CRCStream<SiteToSitePeer> crcStream;
  // Transaction State
  TransactionState _state;
  // Transaction Direction
  TransferDirection _direction;
  // Whether received data is available
  bool _dataAvailable;
  // A global unique identifier
  uuid_t _uuid;
  // UUID string
  std::string _uuidStr;
  // Number of transfer
  int _transfers;
  // Number of content bytes
  uint64_t _bytes;
  static std::shared_ptr<utils::IdGenerator> id_generator_;
};

/**
 * Represents a piece of data that is to be sent to or that was received from a
 * NiFi instance.
 */
class DataPacket {
 public:
  DataPacket(Site2SiteClientProtocol *protocol, Transaction *transaction, std::map<std::string, std::string> attributes, std::string &payload)
      : payload_(payload) {
    _protocol = protocol;
    _size = 0;
    _transaction = transaction;
    _attributes = attributes;
  }
  std::map<std::string, std::string> _attributes;
  uint64_t _size;
  Site2SiteClientProtocol *_protocol;
  Transaction *_transaction;
  std::string & payload_;

};

/**
 * Site2Site Peer
 */
typedef struct Site2SitePeerStatus {
  std::string host_;
  int port_;bool isSecure_;
} Site2SitePeerStatus;

// Site2SiteClientProtocol Class
class Site2SiteClientProtocol : public sitetosite::SiteToSiteClient {
 public:
  // Constructor
  /*!
   * Create a new control protocol
   */
  Site2SiteClientProtocol(std::unique_ptr<SiteToSitePeer> peer)
      : logger_(logging::LoggerFactory<Site2SiteClientProtocol>::getLogger()) {
    peer_ = std::move(peer);
    _batchSize = 0;
    _batchCount = 0;
    _batchDuration = 0;
    _batchSendNanos = 5000000000;  // 5 seconds
    _timeOut = 30000;  // 30 seconds
    _peerState = IDLE;
    _supportedVersion[0] = 5;
    _supportedVersion[1] = 4;
    _supportedVersion[2] = 3;
    _supportedVersion[3] = 2;
    _supportedVersion[4] = 1;
    _currentVersion = _supportedVersion[0];
    _currentVersionIndex = 0;
    _supportedCodecVersion[0] = 1;
    _currentCodecVersion = _supportedCodecVersion[0];
    _currentCodecVersionIndex = 0;
  }
  // Destructor
  virtual ~Site2SiteClientProtocol() {
    tearDown();
  }

 public:
  // setBatchSize
  void setBatchSize(uint64_t size) {
    _batchSize = size;
  }
  // setBatchCount
  void setBatchCount(uint64_t count) {
    _batchCount = count;
  }
  // setBatchDuration
  void setBatchDuration(uint64_t duration) {
    _batchDuration = duration;
  }
  // setTimeOut
  void setTimeOut(uint64_t time) {
    _timeOut = time;
    if (peer_)
      peer_->setTimeOut(time);

  }

  void setPeer(std::unique_ptr<SiteToSitePeer> peer) {
    peer_ = std::move(peer);
  }
  /**
   * Provides a reference to the time out
   * @returns timeout
   */
  const uint64_t getTimeOut() const {
    return _timeOut;
  }


  // getResourceName
  std::string getResourceName() {
    return "SocketFlowFileProtocol";
  }
  // getCodecResourceName
  std::string getCodecResourceName() {
    return "StandardFlowFileCodec";
  }
  // bootstrap the protocol to the ready for transaction state by going through the state machine
  bool bootstrap();
  // get peerList
  bool getPeerList(std::vector<Site2SitePeerStatus> &peer);
  // establish
  bool establish();
  // handShake
  bool handShake();
  // negotiateCodec
  bool negotiateCodec();
  // initiateResourceNegotiation
  bool initiateResourceNegotiation();
  // initiateCodecResourceNegotiation
  bool initiateCodecResourceNegotiation();
  // tearDown
  void tearDown();
  // write Request Type
  int writeRequestType(RequestType type);
  // read Request Type
  int readRequestType(RequestType &type);
  // read Respond
  int readRespond(RespondCode &code, std::string &message);
  // write respond
  int writeRespond(RespondCode code, std::string message);
  // getRespondCodeContext
  RespondCodeContext *getRespondCodeContext(RespondCode code) {
    for (unsigned int i = 0; i < sizeof(respondCodeContext) / sizeof(RespondCodeContext); i++) {
      if (respondCodeContext[i].code == code) {
        return &respondCodeContext[i];
      }
    }
    return NULL;
  }

  // Creation of a new transaction, return the transaction ID if success,
  // Return NULL when any error occurs
  Transaction *createTransaction(std::string &transactionID, TransferDirection direction);
  // Receive the data packet from the transaction
  // Return false when any error occurs
  bool receive(std::string transactionID, DataPacket *packet, bool &eof);
  // Send the data packet from the transaction
  // Return false when any error occurs
  int16_t send(std::string transactionID, DataPacket *packet, std::shared_ptr<FlowFileRecord> flowFile, core::ProcessSession *session);
  // Confirm the data that was sent or received by comparing CRC32's of the data sent and the data received.
  bool confirm(std::string transactionID);
  // Cancel the transaction
  void cancel(std::string transactionID);
  // Complete the transaction
  bool complete(std::string transactionID);
  // Error the transaction
  void error(std::string transactionID);
  // Receive flow files for the process session
  void receiveFlowFiles(core::ProcessContext *context, core::ProcessSession *session);
  // Transfer flow files for the process session
  void transferFlowFiles(core::ProcessContext *context, core::ProcessSession *session);
  //! Transfer string for the process session
  void transferString(core::ProcessContext *context, core::ProcessSession *session, std::string &payload, std::map<std::string, std::string> attributes);
  // deleteTransaction
  void deleteTransaction(std::string transactionID);
  // Nest Callback Class for write stream
  class WriteCallback : public OutputStreamCallback {
   public:
    WriteCallback(DataPacket *packet)
        : _packet(packet) {
    }
    DataPacket *_packet;
    //void process(std::ofstream *stream) {
    int64_t process(std::shared_ptr<io::BaseStream> stream) {
      uint8_t buffer[8192];
      int len = _packet->_size;
      while (len > 0) {
        int size = std::min(len, (int) sizeof(buffer));
        int ret = _packet->_transaction->getStream().readData(buffer, size);
        if (ret != size) {
          _packet->_protocol->logger_->log_error("Site2Site Receive Flow Size %d Failed %d", size, ret);
          return -1;
        }
        stream->write(buffer, size);
        len -= size;
      }
      return len;
    }
  };
  // Nest Callback Class for read stream
  class ReadCallback : public InputStreamCallback {
   public:
    ReadCallback(DataPacket *packet)
        : _packet(packet) {
    }
    DataPacket *_packet;
    int64_t process(std::shared_ptr<io::BaseStream> stream) {
      _packet->_size = 0;
      uint8_t buffer[8192] = { 0 };
      int readSize;
      size_t size = 0;
      do {
        readSize = stream->read(buffer, 8192);

        if (readSize == 0) {
          break;
        }
        if (readSize < 0) {
          return -1;
        }
        int ret = _packet->_transaction->getStream().writeData(buffer, readSize);
        if (ret != readSize) {
          _packet->_protocol->logger_->log_error("Site2Site Send Flow Size %d Failed %d", readSize, ret);
          return -1;
        }
        size += readSize;
      } while (size < stream->getSize());
      _packet->_size = size;
      return size;
    }
  };

 protected:

 private:

  // Mutex for protection
  std::mutex mutex_;
  // Logger
  std::shared_ptr<logging::Logger> logger_;
  // Batch Count
  std::atomic<uint64_t> _batchCount;
  // Batch Size
  std::atomic<uint64_t> _batchSize;
  // Batch Duration in msec
  std::atomic<uint64_t> _batchDuration;
  // Timeout in msec
  std::atomic<uint64_t> _timeOut;



  // BATCH_SEND_NANOS
  uint64_t _batchSendNanos;
  // Peer State
  PeerState _peerState;
  uint32_t _supportedVersion[5];
  uint32_t _currentVersion;
  int _currentVersionIndex;
  uint32_t _supportedCodecVersion[1];
  uint32_t _currentCodecVersion;
  int _currentCodecVersionIndex;
  // commsIdentifier
  std::string _commsIdentifier;
  // transaction map
  std::map<std::string, Transaction *> _transactionMap;

  // Prevent default copy constructor and assignment operation
  // Only support pass by reference or pointer
  Site2SiteClientProtocol(const Site2SiteClientProtocol &parent);
  Site2SiteClientProtocol &operator=(const Site2SiteClientProtocol &parent);
  static std::shared_ptr<utils::IdGenerator> id_generator_;
};

} /* namespace sitetosite */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
#endif
