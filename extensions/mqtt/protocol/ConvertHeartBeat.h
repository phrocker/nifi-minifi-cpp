/**
 * @file ConvertHeartBeat.h
 * ConvertHeartBeat class declaration
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
#ifndef __CONVERT_HEARTBEAT_H__
#define __CONVERT_HEARTBEAT_H__

#include "FlowFileRecord.h"
#include "core/Processor.h"
#include "core/ProcessSession.h"
#include "core/Core.h"
#include "core/Resource.h"
#include "core/Property.h"
#include "core/logging/LoggerConfiguration.h"
#include "MQTTClient.h"
#include "MQTTContextService.h"
#include "c2/protocols/RESTProtocol.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {

class ConvertHeartBeat: public core::Processor, public minifi::c2::RESTProtocol {
public:
  // Constructor
  /*!
   * Create a new processor
   */
  explicit ConvertHeartBeat(std::string name, uuid_t uuid = NULL)
    : core::Processor(name, uuid), logger_(logging::LoggerFactory<ConvertHeartBeat>::getLogger()) {
  }
  // Destructor
  virtual ~ConvertHeartBeat() {
  }
  // Processor Name
  static constexpr char const* ProcessorName = "ConvertHeartBeat";
  // Supported Properties
  static core::Property MQTTControllerService;


  static core::Relationship Success;

public:
  /**
   * Function that's executed when the processor is scheduled.
   * @param context process context.
   * @param sessionFactory process session factory that is used when creating
   * ProcessSession objects.
   */

  virtual void onTrigger(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) override;
  virtual void initialize() override;
  virtual void onSchedule(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSessionFactory> &sessionFactory) override;

protected:

  std::shared_ptr<controllers::MQTTContextService> mqtt_service_;

private:
  std::shared_ptr<logging::Logger> logger_;
};


} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
