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
#ifndef EXTENSIONS_I2CLIB_I2C_H_
#define EXTENSIONS_I2CLIB_I2C_H_

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
//#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define BASE_DEVICE_FILE "/dev/i2c-"
class I2C {
 public:
  I2C(int bus)
      : bus_(bus),
        address_(0x00),
        fd(-1) {
    std::stringstream str;
    str << BASE_DEVICE_FILE << bus_;
    device_file_ = str.str();

    open_i2c();
  }

  int8_t write_data(uint8_t register_address, uint8_t *buf, size_t length) {
    if (fail || fd < 0)
      return -1;

    std::vector<uint8_t> new_buf;
    new_buf.resize(length + 1);

    new_buf[0] = register_address;
    memcpy(new_buf.data() + 1, buf, length);

    if (write(fd, new_buf.data(), length + 1) != length + 1)
      return -1;

    return 0;
  }

  int8_t read_data(uint8_t register_address, uint8_t *buf, size_t length) {
    if (fail || fd < 0)
      return -1;

    std::vector<uint8_t> new_buf;
    new_buf.push_back(register_address);

    if (write(fd, new_buf.data(), 1) != 1)
      return -1;

    if (read(fd, buf, length) != length)
      return -1;

    return 0;
  }

 private:

  void open_i2c() {
    if (fd > 0) {
      close_i2c();
    }
    fd = open(device_file_.c_str(), O_RDWR);
    if (fd < 0) {
      fail = true;
      return;
    }

    if (ioctl(fd, I2C_SLAVE, address_) < 0) {
      fail = true;
    }
  }

  void close_i2c() {
    if (fd > 0) {
      close(fd);
      fd = 0;
    }
  }

  bool fail;

  std::string device_file_;
  int fd;
  uint8_t address_;
  int bus_;

};

#endif /* EXTENSIONS_I2CLIB_I2C_H_ */
