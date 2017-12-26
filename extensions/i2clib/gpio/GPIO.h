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
#ifndef EXTENSIONS_I2CLIB_GPIO_GPIO_H_
#define EXTENSIONS_I2CLIB_GPIO_GPIO_H_

#include "../../i2clib/gpio/c_gpio.h"

class GPIO_INIT {
 public:
  GPIO_INIT() {
    setup();
  }

  ~GPIO_INIT() {
    cleanup();
  }
};

class GPIO {
 public:
  explicit GPIO(uint8_t pin)
      : pin_(pin) {
    static GPIO_INIT static_initializer;
  }

  explicit GPIO(GPIO &&other)
      : pin_(std::move(other.pin_)) {

  }

  void setInput(){
    setup_gpio(pin_,INPUT,PUD_OFF);
  }

  void setOutput(){
      setup_gpio(pin_,OUTPUT,PUD_OFF);
    }

  void output(int value){
    output_gpio(pin_,value);
  }

  GPIO(GPIO &other) = delete;

 private:

  uint8_t pin_;

};

#endif /* EXTENSIONS_I2CLIB_GPIO_GPIO_H_ */
