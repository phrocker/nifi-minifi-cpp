#include "io/SocketFactory.h"

#include <atomic>
#include <mutex> 
  
std::atomic<SocketFactory*> SocketFactory::context_instance_;
std::mutex SocketFactory::context_mutex_;
