/**
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
#ifndef LIBMINIFI_INCLUDE_THREAD_POOL_H
#define LIBMINIFI_INCLUDE_THREAD_POOL_H

#include <iostream>
#include <atomic>
#include <mutex>
#include <vector>
#include <queue>
#include <future>
#include <thread>
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace utils {

/**
 * Worker task
 */
template< typename T>
class Worker{
public:
  explicit Worker(std::function<T()> &task) : task(task)
  {
    promise = std::make_shared<std::promise<T>>();
  }
  
  Worker(Worker &&other) : task (std::move(other.task)),
						promise(other.promise)
  {
  }
  
   
  void run()
  {
    T result = task();
    promise->set_value(result);
  }
  
   Worker<T>(const Worker<T>&) = delete;
  Worker<T>& operator = (const Worker<T>&) = delete;
  
  Worker<T>& operator = (Worker<T>&&) ;
  
  std::shared_ptr<std::promise<T>> getPromise();
  
private:
   std::function<T()> task;
   std::shared_ptr<std::promise<T>> promise;
};

template< typename T>
Worker<T>&  Worker<T>::operator = (Worker<T>&& other)
{
    task = std::move(other.task);
    promise = other.promise;
    return *this;
}


template<typename T>
std::shared_ptr<std::promise<T>> Worker<T>::getPromise(){
    return promise;
  }

template<typename T>
class ThreadPool
    {
    public:
        ThreadPool(int max_worker_threads) : max_worker_threads_(max_worker_threads){
	  current_workers_ = 0;
	}
        virtual ~ThreadPool(){
	  shutdown();
	}
	
        std::future<T> execute(Worker<T> &&task);
        void start();
	void shutdown();

    protected:
      
	void drain()
	{
	  while(current_workers_ > 0)
	  {
	    tasks_available_.notify_one();
	  }
	}
        int max_worker_threads_;
	std::atomic<int> current_workers_;
        std::vector<std::thread> thread_queue_;
        std::thread manager_thread_;
	std::atomic<bool> running_;
        std::queue<Worker<T>> worker_queue_;
        std::condition_variable tasks_available_;
        
        std::mutex worker_queue_mutex_;
	
	
	void startWorkers();
	
        void run_tasks();
    };
    
template<typename T>
std::future<T> ThreadPool<T>::execute(Worker<T> &&task){
  std::unique_lock<std::mutex> lock(worker_queue_mutex_);
  bool wasEmpty = worker_queue_.empty();
  std::future<T> future = task.getPromise()->get_future();
  worker_queue_.push(std::move(task));
  if (wasEmpty)
  {
      tasks_available_.notify_one();
  }
  return future;
}

template< typename T>
void  ThreadPool<T>::startWorkers(){
    for (int i = 0; i < max_worker_threads_; i++)
    {
	thread_queue_.push_back(std::thread(&ThreadPool::run_tasks, this));
	current_workers_++;
    }

    for (auto &thread : thread_queue_)
    {
	thread.join();
    }
}
template< typename T>
void  ThreadPool<T>::run_tasks()
{
  while (running_.load())
    {
	std::unique_lock<std::mutex> lock(worker_queue_mutex_);
	if (worker_queue_.empty())
	{

	    tasks_available_.wait(lock);
	}
	
	if (!running_.load())
	  break;

	if (worker_queue_.empty())
	  continue;
	Worker<T> task = std::move(worker_queue_.front());
	worker_queue_.pop();
	task.run();
    }
    current_workers_--;
    
}
template< typename T>
 void ThreadPool<T>::start()
{
  running_ = true;
  manager_thread_ = std::thread(&ThreadPool::startWorkers, this);
}
template< typename T>
void ThreadPool<T>::shutdown(){

  if (running_.load())
  {
    
    running_.store(false);

    drain();
    manager_thread_.join();
  }
}

    
} /* namespace utils */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

    
#endif