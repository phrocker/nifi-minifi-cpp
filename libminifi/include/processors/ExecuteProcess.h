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
#ifndef __EXECUTE_PROCESS_H__
#define __EXECUTE_PROCESS_H__

#include <stdio.h>
#include <functional>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include "FlowFileRecord.h"
#include "core/Processor.h"
#include "core/ProcessSession.h"
#include "core/core.h"
#include "utils/ThreadPool.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {
class ExecuteProcess;

class ChildProcess {
 public:
  ChildProcess()
      : running_(false),
        redirect_error_stream_(false) {
  }
  std::atomic<bool> running_;
  int pipefd_[2];
  pid_t pid_;
  std::string full_command_;
  std::string command_;
  std::string command_argument_;
  bool redirect_error_stream_;
  uint64_t batch_duration_;

  ChildProcess &operator=(ChildProcess &other) {
    running_.store(other.running_.load());
    pipefd_[0] = other.pipefd_[0];
    pipefd_[1] = other.pipefd_[1];
    pid_ = other.pid_;
    full_command_ = other.full_command_;
    command_ = other.command_;
    command_argument_ = other.command_argument_;
    redirect_error_stream_ = other.redirect_error_stream_;
    batch_duration_ = other.batch_duration_;
    return *this;
  }

  bool operator<(const ChildProcess &b) {
    return pid_ < b.pid_;
  }
};

// ExecuteProcess Class
class ExecuteProcess : public core::Processor {
 public:
  // Constructor
  /*!
   * Create a new processor
   */
  explicit ExecuteProcess(std::string name, uuid_t uuid = NULL)
      : Processor(name, uuid),
        pool_(getMaxConcurrentTasks(), true) {


    uint8_t max_tasks = getMaxConcurrentTasks();


    logger_->log_info("Creating thread pool with %d threads",max_tasks);

  }
  // Destructor
  virtual ~ExecuteProcess() {

    for (auto process : forks) {
      if (process->running_ && process->pid_ > 0)
        kill(process->pid_, SIGTERM);
    }

  }
  // Processor Name
  static const std::string ProcessorName;
  // Supported Properties
  static core::Property Command;
  static core::Property CommandArguments;
  static core::Property WorkingDir;
  static core::Property BatchDuration;
  static core::Property RedirectErrorStream;
  // Supported Relationships
  static core::Relationship Success;

  // Nest Callback Class for write stream
  class WriteCallback : public OutputStreamCallback {
   public:
    WriteCallback(char *data, uint64_t size)
        : _data(data),
          _dataSize(size) {
    }
    char *_data;
    uint64_t _dataSize;
    void process(std::ofstream *stream) {
      if (_data && _dataSize > 0)
        stream->write(_data, _dataSize);
    }
  };

 public:

  void setMaxConcurrentTasks(const uint8_t tasks) {
    max_concurrent_tasks_ = tasks;
    pool_.setMaxConcurrentTasks(max_concurrent_tasks_);
  }

  // OnTrigger method, implemented by NiFi ExecuteProcess
  virtual void onTrigger(core::ProcessContext *context,
                         core::ProcessSession *session);
  // Initialize, over write by NiFi ExecuteProcess
  virtual void initialize(void);

 private:
  // Logger
  std::mutex running_lock_;

  //  std::stack<std::thread> threads;

  std::future<int> submitProcess(std::shared_ptr<ChildProcess> process,
                                 core::ProcessContext *context,
                                 core::ProcessSession *session) {
    std::lock_guard<std::mutex> lock(running_lock_);
    forks.push_back(process);
    std::function<int()> functor =
        [&,process,context,session]() -> int {return ExecuteProcess::executeProcess(process,context,session);};
    utils::Worker<int> worker(functor);
    return pool_.execute(std::move(worker));
  }

  int executeProcess(std::shared_ptr<ChildProcess> process,
                     core::ProcessContext *context,
                     core::ProcessSession *session);

  void stopProcess(std::shared_ptr<ChildProcess> process) {
    std::lock_guard<std::mutex> lock(running_lock_);
    forks.erase(std::remove(forks.begin(), forks.end(), process), forks.end());
  }

  void executeProcessRunner() {
    while (isRunning()) {
      std::lock_guard<std::mutex> lock(running_lock_);
    }
  }

  // thread pool
  utils::ThreadPool<int> pool_;

  // future return values.
  std::queue<std::future<int> > futures;

  // forked child processes.
  std::vector<std::shared_ptr<ChildProcess> > forks;
};

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
