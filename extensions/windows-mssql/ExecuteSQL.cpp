/**
 * @file ExecuteSQL.cpp
 * ExecuteSQL class implementation
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

#include "ExecuteSQL.h"
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <sstream>
#include <stdio.h>
#include <string>
#include <iostream>

#include "io/DataStream.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"


namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {



core::Property ExecuteSQL::ConnectionURL(
		core::PropertyBuilder::createProperty("Connection URL")->withDescription("The database URL to connect to")->build());

core::Property ExecuteSQL::SQLStatement(
	core::PropertyBuilder::createProperty("SQL Statement")->withDescription("The SQL statement to execute")->build());

core::Relationship ExecuteSQL::Success(  
	"success",
	"After a successful SQL execution, result FlowFiles are sent here");
core::Relationship ExecuteSQL::Original( 
	"original",
	"The original FlowFile is sent here");
core::Relationship ExecuteSQL::Failure(  
	"failure",
	"Failures which will not work if retried");

void ExecuteSQL::initialize() {
  //! Set the supported properties
  std::set<core::Property> properties;
  properties.insert(ConnectionURL);
  properties.insert(SQLStatement);
  setSupportedProperties(properties);
  //! Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert(Success);
  relationships.insert(Original);
  relationships.insert(Failure);
  setSupportedRelationships(relationships);
}

void ExecuteSQL::onSchedule(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSessionFactory> &sessionFactory) {
  std::string value;

  if (context->getProperty(ConnectionURL.getName(), value)) {
	  connection_url_ = value;
  }
  if (context->getProperty(SQLStatement.getName(), value)) {
	  sql_statement_ = value;
  }

  if (connection_url_.empty() || sql_statement_.empty()) {
	  throw std::runtime_error("Must supply connection URL and sql statement");
  }

  SQLAllocEnv(&henv);
  SQLAllocConnect(henv, &hdbc);

  auto rc = SQLConnect(hdbc, (unsigned char*)connection_url_.c_str(), SQL_NTS, 0, 0, 0, 0);

  if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO))
  {
	  SQLFreeConnect(hdbc);
	  SQLFreeEnv(henv);
	  hdbc = nullptr;
  }
  
}

void ExecuteSQL::onTrigger(const std::shared_ptr<core::ProcessContext> &context, const std::shared_ptr<core::ProcessSession> &session) {
  
	if (hdbc == nullptr) {
		logger_->log_debug("Handle could not be created for %s", connection_url_);
		return;
	}

	HSTMT hstmt = nullptr;
	
	auto rc = SQLAllocStmt(hdbc, &hstmt);

	if (SQLSUCCESS(SQLExecDirect(hstmt, (unsigned char*)sql_statement_.c_str(), SQL_NTS))) {
		SQLSMALLINT columns;
		for (rc = SQLFetch(hstmt); rc == SQL_SUCCESS; rc = SQLFetch(hstmt)) {
			SQLNumResultCols(hstmt, &columns);
			// 0 is the bookmark, so we start at 1
			for (int i = 1; i < columns) {
				SQLGetData(hstmt, i, SQL_C_CHAR, szData, sizeof(szData), &cbData);
				printf("%s\n", (const char *)szData);
			}
		}
	}
	else {
		SQLFreeStmt(hstmt, SQL_DROP);
		logger_->log_error("Error while running statement");
	}


}
} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */