#ifndef SOCKET_FACTORY_H
#define SOCKET_FACTORY_H

#include "ClientSocket.h"
#include "TLSSocket.h"
#include "ClientSocket.h"
#include "Configure.h"
#include "utils/StringUtils.h"

class SocketFactory{
public:
  
  /**
	 * Build an instance, creating a memory fence, which
	 * allows us to avoid locking. This is tantamount to double checked locking.
	 * @returns new SocketFactory;
	 */
	static SocketFactory *getInstance() {
		SocketFactory* atomic_context = context_instance_.load(
				std::memory_order_relaxed);
		std::atomic_thread_fence(std::memory_order_acquire);
		if (atomic_context == nullptr) {
			std::lock_guard<std::mutex> lock(context_mutex_);
			atomic_context = context_instance_.load(std::memory_order_relaxed);
			if (atomic_context == nullptr) {
				atomic_context = new SocketFactory();
				std::atomic_thread_fence(std::memory_order_release);
				context_instance_.store(atomic_context,
						std::memory_order_relaxed);
			}
		}
		return atomic_context;
	}
   
  std::unique_ptr<Socket> createSocket(std::string host, uint16_t port)
  {
      Socket *socket = 0;
      if (is_secure_)
      {
	socket = new TLSSocket(host,port);
      }
      else{
	socket = new Socket(host,port);
      }
      return std::unique_ptr<Socket>(socket);
  }
protected:
  SocketFactory() : configure_(Configure::getConfigure())
  {
	std::string secureStr;
	is_secure_ = false;
	if (configure_->get(Configure::nifi_remote_input_secure, secureStr))
	{
		StringUtils::StringToBool(secureStr, is_secure_);
	}
  }
  
    bool is_secure_;
    static std::atomic<SocketFactory*> context_instance_;
    static std::mutex context_mutex_;
	
  Configure *configure_;
};

#endif