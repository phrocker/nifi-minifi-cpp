/**
<<<<<<< HEAD
=======
 * @file GetFile.h
 * GetFile class declaration
>>>>>>> d6774b32b40e36afbea80dd09495cceaa5db5233
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
#ifndef __GET_FILE_H__
#define __GET_FILE_H__

<<<<<<< HEAD
#include <atomic>
=======
>>>>>>> d6774b32b40e36afbea80dd09495cceaa5db5233
#include "FlowFileRecord.h"
#include "core/Processor.h"
#include "core/ProcessSession.h"
#include "core/core.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

<<<<<<< HEAD
  struct GetFileRequest{
    std::string directory = ".";
    bool recursive = true;
    bool keepSourceFile = false;
    int64_t minAge = 0;
    int64_t maxAge = 0;
    int64_t minSize = 0;
    int64_t maxSize = 0;
    bool ignoreHiddenFile = true;
    int64_t pollInterval = 0;
    int64_t batchSize = 10;
    std::string fileFilter= "[^\\.].*";
  };

=======
>>>>>>> d6774b32b40e36afbea80dd09495cceaa5db5233
// GetFile Class
class GetFile : public core::Processor {
 public:
  // Constructor
  /*!
   * Create a new processor
   */
<<<<<<< HEAD
  explicit GetFile(std::string name, uuid_t uuid = NULL)
      : Processor(name, uuid) {

=======
  GetFile(std::string name, uuid_t uuid = NULL)
      : Processor(name, uuid) {
    logger_ = logging::Logger::getLogger();
    _directory = ".";
    _recursive = true;
    _keepSourceFile = false;
    _minAge = 0;
    _maxAge = 0;
    _minSize = 0;
    _maxSize = 0;
    _ignoreHiddenFile = true;
    _pollInterval = 0;
    _batchSize = 10;
    _lastDirectoryListingTime = getTimeMillis();
    _fileFilter = "[^\\.].*";
>>>>>>> d6774b32b40e36afbea80dd09495cceaa5db5233
  }
  // Destructor
  virtual ~GetFile() {
  }
  // Processor Name
  static const std::string ProcessorName;
  // Supported Properties
  static core::Property Directory;
  static core::Property Recurse;
  static core::Property KeepSourceFile;
  static core::Property MinAge;
  static core::Property MaxAge;
  static core::Property MinSize;
  static core::Property MaxSize;
  static core::Property IgnoreHiddenFile;
  static core::Property PollInterval;
  static core::Property BatchSize;
  static core::Property FileFilter;
  // Supported Relationships
  static core::Relationship Success;

 public:
  // OnTrigger method, implemented by NiFi GetFile
  virtual void onTrigger(
      core::ProcessContext *context,
      core::ProcessSession *session);
<<<<<<< HEAD
  /**
   * Function that's executed when the processor is scheduled.
   * @param context process context.
   * @param sessionFactory process session factory that is used when creating
   * ProcessSession objects.
   */
  void onSchedule(
        core::ProcessContext *context,
        core::ProcessSessionFactory *sessionFactory);
  // Initialize, over write by NiFi GetFile
  virtual void initialize(void);
  /**
   * performs a listeing on the directory.
   * @param dir directory to list
   * @param request get file request.
   */
  void performListing(std::string dir,const GetFileRequest &request);
=======
  // Initialize, over write by NiFi GetFile
  virtual void initialize(void);
  // perform directory listing
  void performListing(std::string dir);
>>>>>>> d6774b32b40e36afbea80dd09495cceaa5db5233

 protected:

 private:
<<<<<<< HEAD

=======
  // Logger
  std::shared_ptr<logging::Logger> logger_;
>>>>>>> d6774b32b40e36afbea80dd09495cceaa5db5233
  // Queue for store directory list
  std::queue<std::string> _dirList;
  // Get Listing size
  uint64_t getListingSize() {
    std::lock_guard<std::mutex> lock(mutex_);
    return _dirList.size();
  }
  // Whether the directory listing is empty
  bool isListingEmpty();
  // Put full path file name into directory listing
  void putListing(std::string fileName);
  // Poll directory listing for files
<<<<<<< HEAD
  void pollListing(std::queue<std::string> &list,const GetFileRequest &request);
  // Check whether file can be added to the directory listing
  bool acceptFile(std::string fullName, std::string name, const GetFileRequest &request);
  // Get file request object.
  GetFileRequest request_;
  // Mutex for protection of the directory listing

  std::mutex mutex_;

  // last listing time for root directory ( if recursive, we will consider the root
  // as the top level time.
  std::atomic<uint64_t> last_listing_time_;

};

REGISTER_RESOURCE(GetFile)

=======
  void pollListing(std::queue<std::string> &list, int maxSize);
  // Check whether file can be added to the directory listing
  bool acceptFile(std::string fullName, std::string name);
  // Mutex for protection of the directory listing
  std::mutex mutex_;
  std::string _directory;
  bool _recursive;
  bool _keepSourceFile;
  int64_t _minAge;
  int64_t _maxAge;
  int64_t _minSize;
  int64_t _maxSize;
  bool _ignoreHiddenFile;
  int64_t _pollInterval;
  int64_t _batchSize;
  uint64_t _lastDirectoryListingTime;
  std::string _fileFilter;
};

>>>>>>> d6774b32b40e36afbea80dd09495cceaa5db5233
} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
