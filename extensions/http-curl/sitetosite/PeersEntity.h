/*
 * PeersEntity.h
 *
 *  Created on: Oct 12, 2017
 *      Author: mparisi
 */

#ifndef EXTENSIONS_HTTP_CURL_SITETOSITE_PEERSENTITY_H_
#define EXTENSIONS_HTTP_CURL_SITETOSITE_PEERSENTITY_H_

#include "yaml-cpp/yaml.h"
#include "sitetosite/Peer.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace sitetosite {

class PeersEntity {
 public:

  static bool parse(const std::string &entity,uuid_t id, std::vector<PeerStatus> &peer_statuses) {
    try {
      YAML::Node peer_entities = YAML::Load(entity);
      std::cout << "entity " << entity << std::endl;
      auto peers = peer_entities["peers"];
      std::cout << "entity 2" << entity << std::endl;
      if (peers != nullptr && peers.IsSequence()) {
        std::cout << " got ita" << std::endl;
        for (auto peer : peers) {
          std::cout << " got itb" << std::endl;
          auto hostname = peer["hostname"];
          auto port = peer["port"];
          auto secure = peer["secure"];
          auto flowFileCount = peer["flowFileCount"];
          if (hostname != nullptr && port != nullptr) {
            sitetosite::PeerStatus status(std::move(sitetosite::Peer(id, hostname.as<std::string>(), port.as<int>(), secure.as<bool>())), flowFileCount.as<int>(), true);

            peer_statuses.push_back(std::move(status));
          }
        }
      }

      return true;
    } catch (YAML::InvalidNode &in) {
      std::cout << in.what() << std::endl;
      return false;
    } catch (...) {
      std::cout << "there was an error" << std::endl;
      return false;
    }

  }

};

} /* namespace sitetosite */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* EXTENSIONS_HTTP_CURL_SITETOSITE_PEERSENTITY_H_ */
