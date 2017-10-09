/*
 * SiteToSite.h
 *
 *  Created on: Oct 9, 2017
 *      Author: mparisi
 */

#ifndef LIBMINIFI_INCLUDE_CORE_SITETOSITE_SITETOSITE_H_
#define LIBMINIFI_INCLUDE_CORE_SITETOSITE_SITETOSITE_H_



namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace sitetosite {

// Resource Negotiated Status Code
#define RESOURCE_OK 20
#define DIFFERENT_RESOURCE_VERSION 21
#define NEGOTIATED_ABORT 255
// ! Max attributes
#define MAX_NUM_ATTRIBUTES 25000

/**
 * An enumeration for specifying the direction in which data should be
 * transferred between a client and a remote NiFi instance.
 */
typedef enum {
  /**
   * * The client is to send data to the remote instance.
   * */
  SEND,
  /**
   * * The client is to receive data from the remote instance.
   * */
  RECEIVE
} TransferDirection;

// Peer State
typedef enum {
  /**
   * * IDLE
   * */
  IDLE = 0,
  /**
   * * Socket Established
   * */
  ESTABLISHED,
  /**
   * * HandShake Done
   * */
  HANDSHAKED,
  /**
   * * After CodeDec Completion
   * */
  READY
} PeerState;

// Transaction State
typedef enum {
  /**
   * * Transaction has been started but no data has been sent or received.
   * */
  TRANSACTION_STARTED,
  /**
   * * Transaction has been started and data has been sent or received.
   * */
  DATA_EXCHANGED,
  /**
   * * Data that has been transferred has been confirmed via its CRC.
   * * Transaction is ready to be completed.
   * */
  TRANSACTION_CONFIRMED,
  /**
   * * Transaction has been successfully completed.
   * */
  TRANSACTION_COMPLETED,
  /**
   * * The Transaction has been canceled.
   * */
  TRANSACTION_CANCELED,
  /**
   * * The Transaction ended in an error.
   * */
  TRANSACTION_ERROR
} TransactionState;

// Request Type
typedef enum {
  NEGOTIATE_FLOWFILE_CODEC = 0,
  REQUEST_PEER_LIST,
  SEND_FLOWFILES,
  RECEIVE_FLOWFILES,
  SHUTDOWN,
  MAX_REQUEST_TYPE
} RequestType;

// Request Type Str
static const char *RequestTypeStr[MAX_REQUEST_TYPE] = { "NEGOTIATE_FLOWFILE_CODEC", "REQUEST_PEER_LIST", "SEND_FLOWFILES", "RECEIVE_FLOWFILES", "SHUTDOWN" };

// Respond Code
typedef enum {
  RESERVED = 0,
  // ResponseCode, so that we can indicate a 0 followed by some other bytes

  // handshaking properties
  PROPERTIES_OK = 1,
  UNKNOWN_PROPERTY_NAME = 230,
  ILLEGAL_PROPERTY_VALUE = 231,
  MISSING_PROPERTY = 232,
  // transaction indicators
  CONTINUE_TRANSACTION = 10,
  FINISH_TRANSACTION = 11,
  CONFIRM_TRANSACTION = 12,  // "Explanation" of this code is the checksum
  TRANSACTION_FINISHED = 13,
  TRANSACTION_FINISHED_BUT_DESTINATION_FULL = 14,
  CANCEL_TRANSACTION = 15,
  BAD_CHECKSUM = 19,
  // data availability indicators
  MORE_DATA = 20,
  NO_MORE_DATA = 21,
  // port state indicators
  UNKNOWN_PORT = 200,
  PORT_NOT_IN_VALID_STATE = 201,
  PORTS_DESTINATION_FULL = 202,
  // authorization
  UNAUTHORIZED = 240,
  // error indicators
  ABORT = 250,
  UNRECOGNIZED_RESPONSE_CODE = 254,
  END_OF_STREAM = 255
} RespondCode;

// Respond Code Class
typedef struct {
  RespondCode code;
  const char *description;bool hasDescription;
} RespondCodeContext;

// Respond Code Context
static RespondCodeContext respondCodeContext[] = { { RESERVED, "Reserved for Future Use", false }, { PROPERTIES_OK, "Properties OK", false }, { UNKNOWN_PROPERTY_NAME, "Unknown Property Name", true },
    { ILLEGAL_PROPERTY_VALUE, "Illegal Property Value", true }, { MISSING_PROPERTY, "Missing Property", true }, { CONTINUE_TRANSACTION, "Continue Transaction", false }, { FINISH_TRANSACTION,
        "Finish Transaction", false }, { CONFIRM_TRANSACTION, "Confirm Transaction", true }, { TRANSACTION_FINISHED, "Transaction Finished", false }, { TRANSACTION_FINISHED_BUT_DESTINATION_FULL,
        "Transaction Finished But Destination is Full", false }, { CANCEL_TRANSACTION, "Cancel Transaction", true }, { BAD_CHECKSUM, "Bad Checksum", false }, { MORE_DATA, "More Data Exists", false },
    { NO_MORE_DATA, "No More Data Exists", false }, { UNKNOWN_PORT, "Unknown Port", false }, { PORT_NOT_IN_VALID_STATE, "Port Not in a Valid State", true }, { PORTS_DESTINATION_FULL,
        "Port's Destination is Full", false }, { UNAUTHORIZED, "User Not Authorized", true }, { ABORT, "Abort", true }, { UNRECOGNIZED_RESPONSE_CODE, "Unrecognized Response Code", false }, {
        END_OF_STREAM, "End of Stream", false } };


} /* namespace sitetosite */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */


#endif /* LIBMINIFI_INCLUDE_CORE_SITETOSITE_SITETOSITE_H_ */
