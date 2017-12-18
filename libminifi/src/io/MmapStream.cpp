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

#include "io/MmapStream.h"
#include <fstream>
#include <sys/mman.h>
#include <sys/types.h>
#include <vector>
#include <memory>
#include <string>
#include "io/validation.h"
namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace io {

MmapStream::MmapStream(const std::string &path)
    : logger_(logging::LoggerFactory<MmapStream>::getLogger()),
      path_(path),
      offset_(0) {
  open_stream(path, false);
}

MmapStream::MmapStream(const std::string &path, uint32_t offset, bool write_enable)
    : logger_(logging::LoggerFactory<MmapStream>::getLogger()),
      path_(path),
      offset_(offset) {

  open_stream(path, write_enable);
  seek(offset);
}

void MmapStream::closeStream() {
  std::lock_guard<std::recursive_mutex> lock(file_lock_);
  munmap(data_ptr_, length_);
  close(fd);

}

void MmapStream::open_stream(const std::string path, bool write_enable) {

  fd = open(path.c_str(), write_enable ? O_RDWR | O_CREAT | O_TRUNC : O_RDONLY, 0);

  // get file size
  length_ = lseek(fd, 0, SEEK_END);
  // seek to the beginning
  lseek(fd, 0, SEEK_SET);
  if (write_enable) {
    data_ptr_ = static_cast<char*>(mmap(NULL, length_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  } else {
    data_ptr_ = static_cast<char*>(mmap(NULL, length_, PROT_READ, MAP_PRIVATE, fd, 0));
  }
}

void MmapStream::seek(uint64_t offset) {
  std::lock_guard<std::recursive_mutex> lock(file_lock_);

  offset_ = offset;
}

int MmapStream::writeData(std::vector<uint8_t> &buf, int buflen) {
  if (static_cast<int>(buf.capacity()) < buflen) {
    return -1;
  }
  return writeData(reinterpret_cast<uint8_t *>(&buf[0]), buflen);
}

// data stream overrides

int MmapStream::writeData(uint8_t *value, int size) {
  if (!IsNullOrEmpty(value)) {
    std::lock_guard<std::recursive_mutex> lock(file_lock_);
    memcpy(data_ptr_ + offset_, value, size);
    offset_ += size;
    if (offset_ > length_) {
      length_ = offset_;
    }
    msync(data_ptr_, length_, MS_SYNC);
    return size;

  } else {
    return -1;
  }
}

template<typename T>
inline std::vector<uint8_t> MmapStream::readBuffer(const T& t) {
  std::vector<uint8_t> buf;
  buf.resize(sizeof t);
  readData(reinterpret_cast<uint8_t *>(&buf[0]), sizeof(t));
  return buf;
}

int MmapStream::readData(std::vector<uint8_t> &buf, int buflen) {
  if (static_cast<int>(buf.capacity()) < buflen) {
    buf.resize(buflen);
  }
  int ret = readData(reinterpret_cast<uint8_t*>(&buf[0]), buflen);

  if (ret < buflen) {
    buf.resize(ret);
  }
  return ret;
}

int MmapStream::readData(uint8_t *buf, int buflen) {
  if (!IsNullOrEmpty(buf)) {
    std::lock_guard<std::recursive_mutex> lock(file_lock_);

    if (offset_ + buflen > length_) {
      // can't read beyond the end of file
      size_t rem = (offset_ + buflen) - length_;
      size_t ret = buflen - rem;
      offset_ = length_;
      memcpy(buf, data_ptr_ + offset_, ret);
      return ret;
    } else {
      memcpy(buf, data_ptr_ + offset_, buflen);
      offset_ += buflen;
      return buflen;
    }

  } else {
    return -1;
  }
}

} /* namespace io */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

