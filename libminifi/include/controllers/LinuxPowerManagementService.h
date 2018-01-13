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
#ifndef LIBMINIFI_INCLUDE_CONTROLLERS_LINUXPOWERMANAGEMENTSERVICE_H_
#define LIBMINIFI_INCLUDE_CONTROLLERS_LINUXPOWERMANAGEMENTSERVICE_H_

#include <iostream>
#include <memory>
#include "core/Resource.h"
#include "utils/StringUtils.h"
#include "io/validation.h"
#include "core/controller/ControllerService.h"
#include "core/logging/LoggerConfiguration.h"
#include "ThreadManagementService.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace controllers {

enum battery_monitor_mode{
    ALL,
    ATLEASTONE
};

class LinuxPowerManagerService : public ThreadManagementService {
 public:
  explicit LinuxPowerManagerService(const std::string &name, const std::string &id)
      : ThreadManagementService(name, id),
        enabled_(false),
        battery_level_(0),
        wait_period_(0),
        last_time_(0),
        trigger_(0),
        low_battery_trigger_(0),
        mode_(ALL),
        logger_(logging::LoggerFactory<LinuxPowerManagerService>::getLogger()) {
  }

  explicit LinuxPowerManagerService(const std::string &name, uuid_t uuid = 0)
      : ThreadManagementService(name, uuid),
        enabled_(false),
        battery_level_(0),
        wait_period_(0),
        last_time_(0),
        trigger_(0),
        low_battery_trigger_(0),
        mode_(ALL),
        logger_(logging::LoggerFactory<LinuxPowerManagerService>::getLogger()) {
  }

  explicit LinuxPowerManagerService(const std::string &name, const std::shared_ptr<Configure> &configuration)
      : LinuxPowerManagerService(name, nullptr) {
    setConfiguration(configuration);
    initialize();
  }

  static core::Property BatteryCapacityPath;
  static core::Property BatteryStatusPath;
  static core::Property BatteryStatusDischargeKeyword;
  static core::Property BatteryMonitorMode;
  static core::Property TriggerThreshold;
  static core::Property LowBatteryThreshold;
  static core::Property WaitPeriod;

  virtual bool isAboveMax(int current_tasks);

  virtual uint16_t getMaxThreads();

  /**
   * We expect that the wait period has been
   */
  bool shouldReduce();

  bool canIncrease();

  void initialize();

  void yield();

  bool isRunning();

  bool isWorkAvailable();

  void reduce();

  virtual void onEnable();
 protected:

  std::vector<std::pair<std::string,std::string>> paths_;

  bool enabled_;

  std::atomic<int> battery_level_;

  std::atomic<uint64_t> wait_period_;

  std::atomic<uint64_t> last_time_;

  int trigger_;

  int low_battery_trigger_;

  battery_monitor_mode mode_;

  std::string status_keyword_;

 private:
  std::shared_ptr<logging::Logger> logger_;
};

REGISTER_RESOURCE(LinuxPowerManagerService);

} /* namespace controllers */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* LIBMINIFI_INCLUDE_CONTROLLERS_LINUXPOWERMANAGEMENTSERVICE_H_ */
