/**
 * @file ExecuteProcess.cpp
 * ExecuteProcess class implementation
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
#include "processors/ExecuteProcess.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"
#include <cstring>
#include "utils/StringUtils.h"
#include "utils/TimeUtil.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

const std::string ExecuteProcess::ProcessorName("ExecuteProcess");
core::Property ExecuteProcess::Command(
    "Command",
    "Specifies the command to be executed; if just the name of an executable is provided, it must be in the user's environment PATH.",
    "");
core::Property ExecuteProcess::CommandArguments(
    "Command Arguments",
    "The arguments to supply to the executable delimited by white space. White space can be escaped by enclosing it in double-quotes.",
    "");
core::Property ExecuteProcess::WorkingDir(
    "Working Directory",
    "The directory to use as the current working directory when executing the command",
    "");
core::Property ExecuteProcess::BatchDuration(
    "Batch Duration",
    "If the process is expected to be long-running and produce textual output, a batch duration can be specified.",
    "0");
core::Property ExecuteProcess::RedirectErrorStream(
    "Redirect Error Stream",
    "If true will redirect any error stream output of the process to the output stream.",
    "false");
core::Relationship ExecuteProcess::Success(
    "success", "All created FlowFiles are routed to this relationship.");

void ExecuteProcess::initialize() {
  // Set the supported properties
  std::set<core::Property> properties;
  properties.insert(Command);
  properties.insert(CommandArguments);
  properties.insert(WorkingDir);
  properties.insert(BatchDuration);
  properties.insert(RedirectErrorStream);
  setSupportedProperties(properties);
  // Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert(Success);
  setSupportedRelationships(relationships);
  pool_.start();
}

int ExecuteProcess::executeProcess(std::shared_ptr<ChildProcess> process,
                                   core::ProcessContext *context,
                                   core::ProcessSession *session) {
  // if the process has not launched yet
  // create the pipe

  std::cout << "running process! " << process->full_command_ << std::endl;
  char cstr[process->full_command_.length() + 1];
  std::strcpy(cstr, process->full_command_.c_str());
  char *p = std::strtok(cstr, " ");
  int argc = 0;
  char *argv[64];
  while (p != 0 && argc < 64) {
    argv[argc] = p;
    p = std::strtok(NULL, " ");
    argc++;
  }
  argv[argc] = NULL;
  int status, died;
  std::cout << "fork it" << std::endl;
  if (pipe(process->pipefd_) == -1) {
    std::cout << "Huh?0" << std::endl;
    return -1;
  }
  std::cout << "forked it" << std::endl;
  process->pid_ = fork();
  logger_->log_info("pid is %d", process->pid_);
  switch (process->pid_) {
    case -1:
      std::cout << "fork it failed" << std::endl;
     // logger_->log_error("Execute Process fork failed");
      close(process->pipefd_[0]);
      close(process->pipefd_[1]);
      return -1;
      break;
    case 0:  // this is the code the child runs
      std::cout << "Huh?1" << std::endl;
      close(1);      // close stdout
      dup(process->pipefd_[1]);  // points pipefd at file descriptor
      if (process->redirect_error_stream_)
        // redirect stderr
        dup2(process->pipefd_[1], 2);
      close(process->pipefd_[0]);
      execvp(argv[0], argv);
      exit(1);
      break;
    default:  // this is the code the parent runs
      std::cout << "Huh?2" << std::endl;
      // the parent isn't going to write to the pipe
      close(process->pipefd_[1]);
      std::cout << "Huh?2" << std::endl;
      if (process->batch_duration_ > 0) {
        while (isRunning()) {
          std::cout << "huh? " << std::endl;
          std::this_thread::sleep_for(
              std::chrono::milliseconds(process->batch_duration_));
          char buffer[4096];
          int numRead = read(process->pipefd_[0], buffer, sizeof(buffer));
          if (numRead <= 0)
            break;
          logger_->log_info("Execute Command Respond %d", numRead);
          ExecuteProcess::WriteCallback callback(buffer, numRead);
          std::shared_ptr<FlowFileRecord> flowFile = std::static_pointer_cast<
              FlowFileRecord>(session->create());
          if (!flowFile)
            continue;
          flowFile->addAttribute("command", process->command_.c_str());
          flowFile->addAttribute("command.arguments",
                                 process->command_argument_.c_str());
          session->write(flowFile, &callback);
          session->transfer(flowFile, Success);
          session->commit();
        }
      } else {
        std::cout << "fork it2" << std::endl;
        char buffer[4096];
        char *bufPtr = buffer;
        int totalRead = 0;
        std::shared_ptr<FlowFileRecord> flowFile = nullptr;
        while (isRunning()) {
          int numRead = read(process->pipefd_[0], bufPtr,
                             (sizeof(buffer) - totalRead));
          if (numRead <= 0) {
            if (totalRead > 0) {
              logger_->log_info("Execute Command Respond %d", totalRead);
              // child exits and close the pipe
              ExecuteProcess::WriteCallback callback(buffer, totalRead);
              if (!flowFile) {
                flowFile = std::static_pointer_cast<FlowFileRecord>(
                    session->create());
                if (!flowFile)
                  break;
                flowFile->addAttribute("command", process->command_.c_str());
                flowFile->addAttribute("command.arguments",
                                       process->command_argument_.c_str());
                session->write(flowFile, &callback);
              } else {
                session->append(flowFile, &callback);
              }
              session->transfer(flowFile, Success);
            }
            break;
          } else {
            if (numRead == (sizeof(buffer) - totalRead)) {
              // we reach the max buffer size
              logger_->log_info("Execute Command Max Respond %d",
                                sizeof(buffer));
              ExecuteProcess::WriteCallback callback(buffer, sizeof(buffer));
              if (!flowFile) {
                flowFile = std::static_pointer_cast<FlowFileRecord>(
                    session->create());
                if (!flowFile)
                  continue;
                flowFile->addAttribute("command", process->command_.c_str());
                flowFile->addAttribute("command.arguments",
                                       process->command_argument_.c_str());
                session->write(flowFile, &callback);
              } else {
                session->append(flowFile, &callback);
              }
              // Rewind
              totalRead = 0;
              bufPtr = buffer;
            } else {
              totalRead += numRead;
              bufPtr += numRead;
            }
          }
        }
      }
      died = wait(&status);
      if (WIFEXITED(status)) {
        logger_->log_info("Execute Command Complete %s status %d pid %d",
                          process->full_command_.c_str(), WEXITSTATUS(status),
                          process->pid_);
      } else {
        logger_->log_info("Execute Command Complete %s status %d pid %d",
                          process->full_command_.c_str(), WTERMSIG(status),
                          process->pid_);
      }

      close(process->pipefd_[0]);
      break;
  }

  return 1;

}

void ExecuteProcess::onTrigger(core::ProcessContext *context,
                               core::ProcessSession *session) {
  std::string value;
  std::string command_ = "";
  std::string command_argument_ = "";
  std::string working_dir_ = ".";
  int64_t batch_duration_ = 0;
  bool redirect_error_stream_ = false;

  logger_->log_info("onTrigger ExecuteProcess");
  // Full command
  std::string full_command_;
  if (context->getProperty(Command.getName(), value)) {
    command_ = value;
  }
  if (context->getProperty(CommandArguments.getName(), value)) {
    command_argument_ = value;
  }
  if (context->getProperty(WorkingDir.getName(), value)) {
    working_dir_ = value;
  }
  if (context->getProperty(BatchDuration.getName(), value)) {
    core::TimeUnit unit;
    if (core::Property::StringToTime(value, batch_duration_, unit)
        && core::Property::ConvertTimeUnitToMS(batch_duration_, unit,
                                               batch_duration_)) {

    }
  }
  if (context->getProperty(RedirectErrorStream.getName(), value)) {
    org::apache::nifi::minifi::utils::StringUtils::StringToBool(
        value, redirect_error_stream_);
  }
  full_command_ = command_ + " " + command_argument_;

  if (full_command_.length() == 0) {
    yield();
    return;
  }
  if (working_dir_.length() > 0 && working_dir_ != ".") {
    // change to working directory
    if (chdir(working_dir_.c_str()) != 0) {
      logger_->log_error("Execute Command can not chdir %s",
                         working_dir_.c_str());
      yield();
      return;
    }
  }
  logger_->log_info("Execute Command %s", full_command_.c_str());
  // split the command into array
  char cstr[full_command_.length() + 1];
  std::strcpy(cstr, full_command_.c_str());
  char *p = std::strtok(cstr, " ");
  int argc = 0;
  char *argv[64];
  while (p != 0 && argc < 64) {
    argv[argc] = p;
    p = std::strtok(NULL, " ");
    argc++;
  }
  argv[argc] = NULL;
  int status, died;

  std::shared_ptr<ChildProcess> newProcess = std::make_shared<ChildProcess>();
  newProcess->full_command_ = full_command_;
  newProcess->command_ = command_;
  newProcess->batch_duration_ = batch_duration_;
  newProcess->command_argument_ = command_argument_;
  newProcess->redirect_error_stream_ = redirect_error_stream_;
  std::future<int> fut = submitProcess(newProcess, context, session);

  logger_->log_info("Awaiting result of future");
  fut.wait();
  if (fut.get() < 0)
    yield();

}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
