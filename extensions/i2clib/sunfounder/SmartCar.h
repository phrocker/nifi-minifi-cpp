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
#ifndef EXTENSIONS_I2CLIB_SUNFOUNDER_SMARTCAR_H_
#define EXTENSIONS_I2CLIB_SUNFOUNDER_SMARTCAR_H_
#include "../../i2clib/gpio/GPIO.h"
#include "../../i2clib/I2C.h"
#include "core/Core.h"
#include "utils/StringUtils.h"

#define MODE_1 0x00
#define MODE_2 0x01
#define SUBADDRESS_1 0x02
#define SUBADDRESS_2 0x03

#define ALL_LED_ON_L 0xFA
#define ALL_LED_ON_H 0xFB
#define ALL_LED_OFF_L 0xFC
#define ALL_LED_OFF_H 0xFD

#define PRESCALE 0xfe

#define SLEEP 0x10
#define INTDRV 0x10
#define ALLCALL 0x01
#define OUTDRV 0x04

#define DETECTION_FAILURE 0x00
#define RPI_0 0x01
#define RPI_1_B 0x02
#define RPI_1_A 0x03
#define RPI_1_BP 0x04
#define RPI_1_AP 0x05
#define RPI_2 0x06
#define RPI_3 0x07

// there are two motors
#define MOTOR0_A 11
#define MOTOR0_B 12
#define MOTOR1_A 13
#define MOTOR1_B 15

// controls the servo which controls the speed of the car
#define SERVO_EN_M0 4
#define SERVO_EN_M1 5

// THIS IS SPECIFIC TO PCA9685
class SmartCar {

 public:
  SmartCar(uint8_t bus_number, uint32_t address)
      : bus_number_(bus_number),
        address_(address),
        frequency(60),
        motor_0_A(MOTOR0_A),
        motor_0_B(MOTOR0_B),
        motor_1_A(MOTOR1_A),
        motor_1_B(MOTOR1_B) {

    initialize();

    smbus = std::unique_ptr<I2C>(new I2C(bus_number));

    clear_leds();

    uint8_t value = OUTDRV;
    smbus->write_data(MODE_2, &value, 1);

    value = ALLCALL;
    smbus->write_data(MODE_1, &value, 1);

    smbus->read_data(MODE_1, &initial_mode_, 1);

    // WAKE UP!!
    initial_mode_ &= ~SLEEP;

    smbus->write_data(MODE_1, &initial_mode_, 1);

    motor_0_A.setOutput();
    motor_0_B.setOutput();

    motor_1_A.setOutput();
    motor_1_B.setOutput();

  }

  ~SmartCar() {

  }

  void setSpeed(int speed) {
    int32_t speed_m = speed * 40;
    smbus->write_data(SERVO_EN_M0, reinterpret_cast<uint8_t*>(&speed_m), 4);
    smbus->write_data(SERVO_EN_M1, reinterpret_cast<uint8_t*>(&speed_m), 4);
  }

  void forward() {
     move_motor_0(true);
     move_motor_1(true);
   }

  void forward(int speed = 50) {
    setSpeed(speed);
    move_motor_0(true);
    move_motor_1(true);
  }

  void reverse(int speed = 50) {
    setSpeed(speed);
    move_motor_0(false);
    move_motor_1(false);
  }

  void reverse() {
      move_motor_0(false);
      move_motor_1(false);
    }

 protected:

  void move_motor_0(bool forward) {
    if (forward) {
      motor_0_A.output(0x00);
      motor_0_B.output(0x01);
    } else {
      motor_0_A.output(0x01);
      motor_0_B.output(0x00);
    }
  }

  void move_motor_1(bool forward) {
    if (forward) {
      motor_1_A.output(0x00);
      motor_1_B.output(0x01);
    } else {
      motor_1_A.output(0x01);
      motor_1_B.output(0x00);
    }
  }

  void stop() {
    // set them all low
    motor_0_A.output(0x00);
    motor_0_B.output(0x00);

    motor_1_A.output(0x00);
    motor_1_B.output(0x00);
  }

  void clear_leds() {

    uint8_t buf = 0;
    smbus->write_data(ALL_LED_ON_L, &buf, 1);
    smbus->write_data(ALL_LED_ON_H, &buf, 1);
    smbus->write_data(ALL_LED_OFF_L, &buf, 1);
    smbus->write_data(ALL_LED_OFF_H, &buf, 1);
  }

  void initialize() {
    if (bus_number_ == 0) {
      bus_number_ = get_bus_number();
    }
  }

  uint8_t get_pi_revision() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    /*
     I read somewhere that the processor ID could be repeated. I don't know if
     that's true or not.
     */
    while (!cpuinfo.eof()) {

      std::getline(cpuinfo, line);
      if (!line.size())
        continue;

      if (line.find("Revision") != 0)
        continue;

      if (line.find("1 Module B") != std::string::npos)
        return RPI_0;
      else if (line.find("1 Module B") != std::string::npos)
        return RPI_1_B;
      else if (line.find("1 Module A") != std::string::npos)
        return RPI_1_A;
      else if (line.find("1 Module B+") != std::string::npos)
        return RPI_1_BP;
      else if (line.find("1 Module A+") != std::string::npos)
        return RPI_1_AP;
      else if (line.find("2 Module B") != std::string::npos)
        return RPI_2;
      else if (line.find("a22082") != std::string::npos || line.find("a32082") != std::string::npos)
        return RPI_3;

    }
    return DETECTION_FAILURE;
  }

  uint8_t get_bus_number() {
    auto revision = get_pi_revision();
    switch (revision) {
      case RPI_0:
      case RPI_1_B:
      case RPI_1_A:
      case RPI_1_AP:
        return 0;
      case RPI_1_BP:
      case RPI_2:
      case RPI_3:
        return 1;
    }
    return 0;
  }
 private:

  GPIO motor_0_A;
  GPIO motor_0_B;

  GPIO motor_1_A;
  GPIO motor_1_B;

  float frequency;

  std::unique_ptr<I2C> smbus;
  uint8_t bus_number_;
  uint32_t address_;

  uint8_t initial_mode_;
};

#endif /* EXTENSIONS_I2CLIB_SUNFOUNDER_SMARTCAR_H_ */
