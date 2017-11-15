/**
 * @file ArchiveMetadata.cpp
 * ArchiveMetadata class definition
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

#include "ArchiveMetadata.h"

#include <archive.h>
#include <archive_entry.h>

#include <list>
#include <string>
#include <algorithm>
#include <iostream>

#include "utils/file/FileManager.h"
#include "Exception.h"

using org::apache::nifi::minifi::Exception;
using org::apache::nifi::minifi::ExceptionType::GENERAL_EXCEPTION;

Json::Value ArchiveEntryMetadata::toJson() const {
    Json::Value entryVal(Json::objectValue);
    entryVal["entry_name"] = Json::Value(entryName);
    entryVal["entry_type"] = Json::Value(entryType);
    entryVal["entry_perm"] = Json::Value(entryPerm);
    entryVal["entry_size"] = Json::Value(entrySize);
    entryVal["entry_uid"] = Json::Value(entryUID);
    entryVal["entry_gid"] = Json::Value(entryGID);
    entryVal["entry_mtime"] = Json::Value(entryMTime);
    entryVal["entry_mtime_nsec"] = Json::Value(entryMTimeNsec);

    if (entryType == AE_IFREG) {
        entryVal["stash_key"] = Json::Value(stashKey);
    }

    return entryVal;
}

void ArchiveEntryMetadata::loadJson(const Json::Value& entryVal) {
    entryName.assign(entryVal["entry_name"].asString());
    entryType = entryVal["entry_type"].asUInt64();
    entryPerm = entryVal["entry_perm"].asUInt64();
    entrySize = entryVal["entry_size"].asUInt64();
    entryUID = entryVal["entry_uid"].asUInt64();
    entryGID = entryVal["entry_gid"].asUInt64();
    entryMTime = entryVal["entry_mtime"].asUInt64();
    entryMTimeNsec = entryVal["entry_mtime_nsec"].asInt64();

    if (entryType == AE_IFREG)
        stashKey.assign(entryVal["stash_key"].asString());
}

ArchiveEntryMetadata ArchiveEntryMetadata::fromJson(const Json::Value& entryVal) {
    ArchiveEntryMetadata aem;
    aem.loadJson(entryVal);
    return aem;
}

ArchiveEntryIterator ArchiveMetadata::find(const std::string& name) {
    auto targetTest = [&](const ArchiveEntryMetadata& entry) -> bool {
        return entry.entryName == name;
    };

    return std::find_if(entryMetadata.begin(),
                        entryMetadata.end(),
                        targetTest);
}

ArchiveEntryIterator ArchiveMetadata::eraseEntry(ArchiveEntryIterator position) {
    return entryMetadata.erase(position);
}

ArchiveEntryIterator ArchiveMetadata::insertEntry(
    ArchiveEntryIterator position, const ArchiveEntryMetadata& entry) {
    return entryMetadata.insert(position, entry);
}

Json::Value ArchiveMetadata::toJson() const {
    Json::Value structVal(Json::arrayValue);

    for (const auto &entry : entryMetadata) {
      Json::Value entryVal = entry.toJson();
      structVal.append(entryVal);
    }

    Json::Value lensVal(Json::objectValue);
    lensVal["archive_format_name"] = Json::Value(archiveFormatName);
    lensVal["archive_format"] = Json::Value(archiveFormat);
    lensVal["archive_structure"] = structVal;

    if (!archiveName.empty())
      lensVal["archive_name"] = Json::Value(archiveName);

    lensVal["focused_entry"] = Json::Value(focusedEntry);

    return lensVal;
}

ArchiveMetadata ArchiveMetadata::fromJson(const Json::Value& metadataDoc) {
    ArchiveMetadata am;
    am.loadJson(metadataDoc);
    return am;
}

void ArchiveMetadata::loadJson(const Json::Value& metadataDoc) {
    Json::Value archiveName_ = metadataDoc.get("archive_name", Json::Value(Json::nullValue));

    if (!archiveName_.isNull())
      archiveName.assign(archiveName_.asString());

    archiveFormatName.assign(metadataDoc["archive_format_name"].asString());
    archiveFormat = metadataDoc["archive_format"].asUInt64();

    focusedEntry = metadataDoc["focused_entry"].asString();
  
    for (const auto &entryVal : metadataDoc["archive_structure"]) {
        entryMetadata.push_back(ArchiveEntryMetadata::fromJson(entryVal));
    }
}

void ArchiveMetadata::seedTempPaths(fileutils::FileManager *file_man, bool keep = false) {
    for (auto& entry : entryMetadata)
        entry.tmpFileName.assign(file_man->unique_file("/tmp/", keep));
}

ArchiveStack ArchiveStack::fromJson(const Json::Value& input) {
    ArchiveStack as;
    as.loadJson(input);
    return as;
}

ArchiveStack ArchiveStack::fromJsonString(const std::string& input) {
    ArchiveStack as;
    as.loadJsonString(input);
    return as;
}

void ArchiveStack::loadJson(const Json::Value& lensStack) {
    for (Json::Value::ArrayIndex i=0; i < lensStack.size(); i++) {
        stack_.push_back(ArchiveMetadata::fromJson(lensStack[i]));
    }
}

void ArchiveStack::loadJsonString(const std::string& input) {
    Json::Value lensStack {Json::arrayValue};
    Json::Reader reader;

    if (!reader.parse(input, lensStack)) {
        std::stringstream ss;
        ss << "Failed to parse archive lens stack from JSON string: "
           << reader.getFormattedErrorMessages();
        std::string exception_msg = ss.str();

        throw Exception(GENERAL_EXCEPTION, exception_msg.c_str());
    }

    loadJson(lensStack);
}

Json::Value ArchiveStack::toJson() const {
    Json::Value lensStack {Json::arrayValue};

    for (const auto& metadata : stack_) {
        lensStack.append(metadata.toJson());
    }

    return lensStack;
}

std::string ArchiveStack::toJsonString() const {
    Json::FastWriter writer;
    std::string jsonString = writer.write(toJson());
    return jsonString;
}