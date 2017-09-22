/**
 * @file AccumuloWriter.cpp
 * AccumuloWriter class implementation
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
#include "processors/AccumuloWriter.h"
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <data/constructs/rfile/RFile.h>
#include <data/constructs/KeyValue.h>
#include <data/streaming/EndianTranslation.h>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include "utils/TimeUtil.h"
#include "utils/StringUtils.h"
#include "core/ProcessContext.h"
#include "core/ProcessSession.h"

namespace org {
namespace apache {
namespace nifi {
namespace minifi {
namespace processors {
core::Property AccumuloWriter::RfileDirectory("Rfile Directory", "Directory to which we will write Rfiles", "info");
core::Property AccumuloWriter::KeysPerRFile("Rfile Keys", "Maximum number of keys per rfile", "info");
core::Relationship AccumuloWriter::Success("success", "All files are routed to success");
core::Relationship AccumuloWriter::Import("import", "Files to bulk import");
static bool keyCompare(cclient::data::KeyValue* a, cclient::data::KeyValue* b) {
  return *(a->getKey()) < *(b->getKey());
}

static void writeRfile(std::string outputFile, bool bigEndian, std::vector<cclient::data::KeyValue*> &keyValues) {
  std::ofstream ofs(outputFile.c_str(), std::ofstream::out);

  cclient::data::streams::OutputStream *stream = new cclient::data::streams::OutputStream(&ofs, 0);

  cclient::data::streams::ByteOutputStream *outStream = new cclient::data::streams::BigEndianByteStream(250 * 1024 * 1024, stream);

  cclient::data::compression::Compressor *compressor = new cclient::data::compression::ZLibCompressor(256 * 1024);

  cclient::data::BlockCompressedFile bcFile(compressor);

  cclient::data::RFile *newRFile = new cclient::data::RFile(outStream, &bcFile);

  std::sort(keyValues.begin(), keyValues.end(), keyCompare);

  newRFile->addLocalityGroup();

  for (auto it : keyValues) {
    newRFile->append(it);
  }

  outStream->flush();
  newRFile->close();

  delete outStream;

  delete stream;

  delete newRFile;

}

void AccumuloWriter::onSchedule(std::shared_ptr<core::ProcessContext> context, std::shared_ptr<core::ProcessSessionFactory> sessionFactory) {

  std::string value;

  if (context->getProperty(RfileDirectory.getName(), value)) {
    output_dir_ = value;
  }

  value = "";
  if (context->getProperty(KeysPerRFile.getName(), value)) {
    max_keys_ = std::stoi(value);
  } else {
    max_keys_ = 1000;
  }

  current_count_ = 0;
  key_count_ = 0;
}
void AccumuloWriter::initialize() {
  // Set the supported properties
  std::set<core::Property> properties;
  properties.insert(RfileDirectory);
  properties.insert(KeysPerRFile);
  setSupportedProperties(properties);
  // Set the supported relationships
  std::set<core::Relationship> relationships;
  relationships.insert(Success);
  setSupportedRelationships(relationships);
}

void AccumuloWriter::onTrigger(core::ProcessContext *context, core::ProcessSession *session) {
  logger_->log_info("Enter Accumulo Writer %d %d", current_count_.load(), max_keys_);
  std::shared_ptr<core::FlowFile> flow = nullptr;
  std::lock_guard<std::mutex> lock(rfile_mutex_);
  do {
    std::string dashLine = "--------------------------------------------------";
    bool logPayload = false;
    std::ostringstream message;
    flow = session->get();

    if (!flow || output_dir_.empty()) {
      logger_->log_info("Exit Accumulo Writer: no flow");
      return;
    }

    message << "Logging for flow file " << "\n";
    message << dashLine;
    message << "\nStandard FlowFile Attributes";
    message << "\n" << "UUID:" << flow->getUUIDStr();
    message << "\n" << "EntryDate:" << getTimeStr(flow->getEntryDate());
    message << "\n" << "lineageStartDate:" << getTimeStr(flow->getlineageStartDate());
    message << "\n" << "Size:" << flow->getSize() << " Offset:" << flow->getOffset();
    message << "\nFlowFile Attributes Map Content";
    std::map<std::string, std::string> attrs = flow->getAttributes();
    std::map<std::string, std::string>::iterator it;
    for (it = attrs.begin(); it != attrs.end(); it++) {
      message << "\n" << "key:" << it->first << " value:" << it->second;
    }
    message << "\nFlowFile Resource Claim Content";
    std::shared_ptr<ResourceClaim> claim = flow->getResourceClaim();
    if (claim) {
      message << "\n" << "Content Claim:" << claim->getContentFullPath();
    }
    if (logPayload && flow->getSize() <= 1024 * 1024) {
      message << "\n" << "Payload:" << "\n";
      ReadCallback callback(flow->getSize());
      session->read(flow, &callback);
      for (unsigned int i = 0, j = 0; i < callback.read_size_; i++) {
        message << std::hex << callback.buffer_[i];
        j++;
        if (j == 80) {
          message << '\n';
          j = 0;
        }
      }
    }
    std::string vis = "00000001";
    message << "\n" << dashLine << std::ends;
    std::string output = message.str();

    char rw[13], cf[9], cq[9], cv[9];

    cclient::data::Value *v = new cclient::data::Value(output);

    cclient::data::Key *k = new cclient::data::Key();

    std::string rowSt = "2";

    memset(rw, 0x00, 13);
    sprintf(rw, "bat");

    k->setRow((const char*) rw, 3);

    key_count_++;
    current_count_++;
    memset(cf, 0x00, 13);
    sprintf(cf, "%08d", key_count_.load());

    k->setColFamily((const char*) cf, 8);

    k->setColQualifier((const char*) cf, 8);

    k->setColVisibility(vis.c_str(), vis.size());

    k->setTimeStamp(1445105294261L);

    cclient::data::KeyValue *kv = new cclient::data::KeyValue();

    kv->setKey(k);
    kv->setValue(v);

    keyValues.push_back(kv);

    if (current_count_ > max_keys_) {
      std::string outputDir = output_dir_ + "/";

      uuid_t uid;

      std::shared_ptr<utils::IdGenerator> id_gen = utils::IdGenerator::getIdGenerator();
      id_gen->generate(uid);

      char uuidStr[37];
      uuid_unparse_lower(uid, uuidStr);
      std::string id = uuidStr;

      outputDir += id;

      logger_->log_debug("Writing rfile %s", outputDir);

      mkdir(outputDir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);

      std::string rfile = outputDir + "/rf.rf";

      logger_->log_info("Writing %d to %s", keyValues.size(), rfile);
      writeRfile(rfile, true, keyValues);

      for (std::vector<cclient::data::KeyValue*>::iterator it = keyValues.begin(); it != keyValues.end(); ++it) {
        delete (*it)->getKey();
        delete (*it);
      }
      keyValues.clear();

      current_count_ = 0;

      flow->addAttribute("Rfile", outputDir);

      session->import(rfile,flow,false,0);

      session->transfer(flow, Import);
      return;
    }

//  flow->pu

// Transfer to the relationship
    session->transfer(flow, Success);
  } while (flow != nullptr);
}

} /* namespace processors */
} /* namespace minifi */
} /* namespace nifi */
} /* namespace apache */
} /* namespace org */
