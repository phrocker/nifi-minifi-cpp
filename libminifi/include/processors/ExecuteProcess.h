/**
 * @file ExecuteProcess.h
 * ExecuteProcess class declaration
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

struct ChildProcess{
  std::atomic<bool> running_;
  int pipefd_[2];
  pid_t pid_;
  
  ChildProcess &operator=(ChildProcess &other)
  {
      running_ = other.running_;
      pipefd_[0] = other.pipefd_[0];
      pipefd_[1] = other.pipefd_[1];
      pid_ = other.pid_;
  }
  
  bool operator<(const ChildProcess &b)
  {
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
      : Processor(name, uuid), pool_(getMaxConcurrentTasks) {
    logger_ = logging::Logger::getLogger();
    _redirectErrorStream = false;
    _batchDuration = 0;
    _workingDir = ".";
//    _processRunning = false;
  //  _pid = 0;
   // forks_ = 0;
    //
    uint8_t max_tasks = getMaxConcurrentTasks();
    /*
    for (int i=0; i < max_tasks; i++)
    {
      threads.push( std::thread() );
    }*/
  }
  // Destructor
  virtual ~ExecuteProcess() {
    
    for(ChildProcess process: forks)
    {
      if (process.running_ && process.pid_ > 0)
	kill(process.pid_, SIGTERM);
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
  // OnTrigger method, implemented by NiFi ExecuteProcess
  virtual void onTrigger(
      core::ProcessContext *context,
      core::ProcessSession *session);
  // Initialize, over write by NiFi ExecuteProcess
  virtual void initialize(void);

  
 protected:

 private:
  // Logger
  std::mutex running_lock_;
  
  ThreadPool pool_;
  
//  std::stack<std::thread> threads;
  
  std::shared_ptr<logging::Logger> logger_;
  // Property
  
  
  
  std::list<ChildProcess> forks;
  // whether the process is running
//  std::atomic<uint64_t> forks_;
  //std::atomic<bool> _processRunning;
  //int _pipefd[2];
  //pid_t _pid;
};

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
