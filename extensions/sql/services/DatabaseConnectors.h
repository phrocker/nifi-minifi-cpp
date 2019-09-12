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

#ifndef EXTENSIONS_SQL_SERVICES_DATABASECONNECTORS_H_
#define EXTENSIONS_SQL_SERVICES_DATABASECONNECTORS_H_

#include <memory>

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace sql {

class Statement {
 public:
};

class Connection {
 public:
  virtual ~Connection() {
  }
  virtual std::unique_ptr<Statement> prepareStatement(const std::string &query) const = 0;
};

} /* namespace sql */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */

#endif /* EXTENSIONS_SQL_SERVICES_DATABASECONNECTORS_H_ */
