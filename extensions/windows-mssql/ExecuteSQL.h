/**
 * @file ExecuteSQL.h
 * ExecuteSQL class declaration
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
#ifndef TAIL_EVENT_LOG_H_
#define TAIL_EVENT_LOG_H_


#include "core/Core.h"
#include "FlowFileRecord.h"
#include "core/Processor.h"
#include "core/ProcessSession.h"
#include <sqltypes.h> 

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {


#define SQLSUCCESS(rc) ((rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO) )  

//Server=localhost;Database=blahblah;Trusted_Connection=True;
/**
This class can and should be moved to 
*/
class ExecuteSQL : public core::Processor
{
public:
	//! Constructor
	/*!
	 * Create a new processor
	 */
	ExecuteSQL(std::string name, utils::Identifier uuid = utils::Identifier())
	: core::Processor(name, uuid), hdbc(nullptr), logger_(logging::LoggerFactory<ExecuteSQL>::getLogger()),max_events_(1){
	}
	//! Destructor
	virtual ~ExecuteSQL()
	{
	}
	//! Processor Name
	static const std::string ProcessorName;
	//! Supported Properties
	static core::Property ConnectionURL;
	static core::Property SQLStatement;

	static core::Relationship Success;
	static core::Relationship Original;
	static core::Relationship Failure;

public:
  /**
   * Function that's executed when the processor is scheduled.
   * @param context process context.
   * @param sessionFactory process session factory that is used when creating
   * ProcessSession objects.
   */
  void onSchedule(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSessionFactory> &sessionFactory) override;
	//! OnTrigger method, implemented by NiFi ExecuteSQL
	virtual void onTrigger(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) override;
	//! Initialize, over write by NiFi ExecuteSQL
	virtual void initialize(void) override;

protected:

	virtual void notifyStop() override{
		if (hdbc) {
			SQLFreeConnect(hdbc);
			SQLFreeEnv(henv);
		}
	}

private:
  std::mutex log_mutex_;
  std::string connection_url_;
  std::string sql_statement_;
  

  HENV henv;
  HDBC hdbc;
  // Logger
  std::shared_ptr<logging::Logger> logger_;
};
REGISTER_RESOURCE(ExecuteSQL);

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif
