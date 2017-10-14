/*
 * HttpTransaction.h
 *
 *  Created on: Oct 12, 2017
 *      Author: mparisi
 */

#ifndef EXTENSIONS_HTTP_CURL_SITETOSITE_HTTPTRANSACTION_H_
#define EXTENSIONS_HTTP_CURL_SITETOSITE_HTTPTRANSACTION_H_

#include "sitetosite/SiteToSite.h"
#include "io/CRCStream.h"
#include "sitetosite/SiteToSiteClient.h"
#include "sitetosite/Peer.h"
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace sitetosite {

class HttpTransaction : public sitetosite::Transaction {
 public:
  explicit HttpTransaction(sitetosite::TransferDirection direction, org::apache::nifi::minifi::io::CRCStream<SiteToSitePeer> &stream)
      : Transaction(direction, stream),
        client_ref_(nullptr) {

  }

  void initialize(sitetosite::SiteToSiteClient *client, const std::string &url) {
    client_ref_ = client;
    transaction_url_ = url;
  }
 protected:
  sitetosite::SiteToSiteClient *client_ref_;
  std::string transaction_url_;
};

} /* namespace sitetosite */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* EXTENSIONS_HTTP_CURL_SITETOSITE_HTTPTRANSACTION_H_ */
