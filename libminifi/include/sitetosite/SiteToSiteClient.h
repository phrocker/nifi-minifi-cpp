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

#ifndef LIBMINIFI_INCLUDE_CORE_SITETOSITE_SITETOSITECLIENT_H_
#define LIBMINIFI_INCLUDE_CORE_SITETOSITE_SITETOSITECLIENT_H_

#include "SiteToSite.h"
#include "SiteToSitePeer.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace sitetosite {

class SiteToSiteClient {

 public:

  virtual ~SiteToSiteClient() {

  }

  virtual std::unique_ptr<Transaction> createTransaction(std::string &transactionID, TransferDirection direction) = 0;

  virtual void transferFlowFiles(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) = 0;

  void setPortId(uuid_t id) {
    uuid_copy(port_id_, id);
    char idStr[37];
    uuid_unparse_lower(id, idStr);
    port_id_str_ = idStr;
  }

  void setPeer(std::unique_ptr<SiteToSitePeer> peer) {
    peer_ = std::move(peer);
  }

  /**
   * Provides a reference to the port identifier
   * @returns port identifier
   */
  const std::string getPortId() const {
    return port_id_str_;
  }

 protected:

  // portIDStr
  std::string port_id_str_;

  // portId
  uuid_t port_id_;

  // Peer Connection
  std::unique_ptr<SiteToSitePeer> peer_;

};

} /* namespace sitetosite */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CORE_SITETOSITE_SITETOSITECLIENT_H_ */
