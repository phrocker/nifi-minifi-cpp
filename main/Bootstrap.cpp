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

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <zipper/zipper.h>
#include <zipper/unzipper.h>
#include <zipper/tools.h>
#include "cxxopts.hpp"
#include "utils/file/FileUtils.h"
#include "utils/Id.h"
#include "properties/Configure.h"
#include "core/FlowConfiguration.h"
#include "agent/agent_version.h"
#include "agent/build_description.h"
#include "agent/agent_docs.h"
#include "core/ClassLoader.h"
#include "Manifest.h"


static void addOrCreate(const std::string &binary, std::istream &stream, const std::string &name, bool replace = false) {
  if (!replace) {
    try {
      zipper::Zipper zip_handler(binary);
      zip_handler.add(stream, name);
      zip_handler.close();
      return;
    } catch (...) {

    }
  }

  std::cout << "adding " << name << " to zip" << std::endl;
  const auto property = std::make_shared<minifi::Properties>();
  auto generator = utils::IdGenerator::getIdGenerator();
  generator->initialize(property);
  utils::Identifier ident;
  generator->generate(ident);

  const auto &out_file = temp_dir() + "/" + ident.to_string() + ".zip";
  zipper::Zipper zip_handler(out_file);
  zip_handler.add(stream, name);
  zip_handler.close();
  std::fstream zip_manifest_file(out_file, std::ios::in);
  std::fstream binary_file(binary, std::ios::binary | std::ios::app);
  binary_file << zip_manifest_file.rdbuf();
  zip_manifest_file.close();
  binary_file.close();
}

static void addOrCreate(const std::string &binary, const std::string &file, const std::string &name, bool replace = false) {
  std::fstream stream(file, std::ios::in);
  addOrCreate(binary, stream, name, replace);
}

int main(int argc, char **argv) {

  cxxopts::Options options("AgentBootstrap", "Bootstraps the agent with known properties");
  options.positional_help("[optional args]").show_positional_help();

  std::cout << "Creating temp dir " << temp_dir() << std::endl;
  options.add_options()  //NOLINT
  ("h,help", "Shows Help")  //NOLINT
  ("agent", "Agent binary Location", cxxopts::value<std::string>())  //NOLINT
  ("manifest", "Manifest path")  //NOLINT
  ("extension", "Add Extension Docs");  //NOLINT

  core::FlowConfiguration::initialize_static_functions();

  try {
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::cout << options.help( { "", "Group" }) << std::endl;
      exit(0);
    }

    bool exists = false;
    std::string manifest;
    if (result.count("agent") > 0) {
      auto& binary = result["agent"].as<std::string>();
      try {
        zipper::Unzipper unz(binary);
        const auto entries = unz.entries();
        if (!entries.empty()) {
          exists = true;
        }
        for (const auto &entry : entries) {
          std::cout << "Binary already contains " + entry.name << std::endl;
        }
      } catch (const std::exception & e) {
        std::cout << "Could not open zip, no files embedded" << std::endl;
      }

      if (result.count("manifest")) {
        std::stringstream str;
        produce_manifest(str);
        addOrCreate(binary, str, "META-INF/MANIFEST.MF", exists);
        if (result.count("extension")) {
          addOrCreate(binary, create_extension_manifest(), "META-INF/docs/extension-manifest.xml");
        }
      }

      /*
       unzFile uf = unzOpen(binary.c_str());
       int res = unzGoToFirstFile(uf);
       std::cout << "res" << res << std::endl;
       if (res == UNZ_OK) {
       do {
       char filename_inzip[256] = { 0 };
       unz_file_info64 file_info = { 0 };
       const char *string_method = NULL;
       unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
       ZPOS64_T uncompressed_size = file_info.uncompressed_size;
       void *ptr = malloc(uncompressed_size * (sizeof(ZPOS64_T)));
       unzReadCurrentFile(uf, ptr, uncompressed_size);
       printf("%s\n", filename_inzip);
       res = unzGoToNextFile(uf);
       free(ptr);

       } while (res != UNZ_END_OF_LIST_OF_FILE);
       }
       unzCloseCurrentFile(uf);
       */
    }

    std::cout << "Deleting temp dir " << temp_dir() << std::endl;
    //utils::file::FileUtils::delete_dir(temp_dir());
    std::cout << "yah boi" << std::endl;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
