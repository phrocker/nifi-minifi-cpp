/**
 * @file MiNiFiMain.cpp 
 * MiNiFiMain implementation 
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
#include <fcntl.h>
#include <stdio.h>
#include <semaphore.h>
#include <signal.h>
#include <vector>
#include <queue>
#include <map>
#include <unistd.h>
#include <yaml-cpp/yaml.h>
#include <iostream>

#include "core/Core.h"

#include "core/FlowConfiguration.h"
#include "core/ConfigurationFactory.h"
#include "core/RepositoryFactory.h"
#include "FlowController.h"

#include "cxxopts.hpp"

#define MINIFI_HOME_ENV_KEY "MINIFI_HOME"


int main(int argc, char **argv) {

  std::shared_ptr<logging::Logger> logger = logging::LoggerConfiguration::getConfiguration().getLogger("controller");

  // assumes POSIX compliant environment
   std::string minifiHome;
   if (const char *env_p = std::getenv(MINIFI_HOME_ENV_KEY)) {
     minifiHome = env_p;
     logger->log_info("Using MINIFI_HOME=%s from environment.", minifiHome);
   } else {
     logger->log_info("MINIFI_HOME is not set; determining based on environment.");
     char *path = nullptr;
     char full_path[PATH_MAX];
     path = realpath(argv[0], full_path);

     if (path != nullptr) {
       std::string minifiHomePath(path);
       if (minifiHomePath.find_last_of("/\\") != std::string::npos) {
         minifiHomePath = minifiHomePath.substr(0, minifiHomePath.find_last_of("/\\"));  //Remove /minifi from path
         minifiHome = minifiHomePath.substr(0, minifiHomePath.find_last_of("/\\"));    //Remove /bin from path
       }
     }

     // attempt to use cwd as MINIFI_HOME
     if (minifiHome.empty() || !validHome(minifiHome)) {
       char cwd[PATH_MAX];
       getcwd(cwd, PATH_MAX);
       minifiHome = cwd;
     }
   }

   if (!validHome(minifiHome)) {
     logger->log_error("No valid MINIFI_HOME could be inferred. "
                       "Please set MINIFI_HOME or run minifi from a valid location.");
     return -1;
   }

  std::shared_ptr<minifi::Configure> configure = std::make_shared<minifi::Configure>();
    configure->setHome(minifiHome);
    configure->loadConfigureFile(DEFAULT_NIFI_PROPERTIES_FILE);


  cxxopts::Options options("MiNiFiController", "MiNiFi local agent controller");

  options.add_options()
    ("p,pid", "Enable debugging")
    ("p,pid", "Process ID of the MiNiFi Agent", cxxopts::value<std::string>())
    ;
    return 0;
}
