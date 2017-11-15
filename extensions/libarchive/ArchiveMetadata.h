/**
 * @file ArchiveMetadata.h
 * ArchiveMetadata class declaration
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
#ifndef EXTENSIONS_LIBARCHIVE_ARCHIVEMETADATA_H_
#define EXTENSIONS_LIBARCHIVE_ARCHIVEMETADATA_H_

#include "json/json.h"
#include "json/writer.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <list>
#include <vector>
#include <string>
#include <algorithm>

#include "core/Core.h"
#include "utils/file/FileManager.h"

class ArchiveEntryMetadata {
public:
    std::string entryName;
    mode_t entryType;
    mode_t entryPerm;
    uid_t entryUID;
    gid_t entryGID;
    uint64_t entryMTime;
    uint64_t entryMTimeNsec;
    uint64_t entrySize;

    std::string tmpFileName;
    std::string stashKey;

    inline Json::Value toJson() const;
    static inline ArchiveEntryMetadata fromJson(const Json::Value&);

private:
    inline void loadJson(const Json::Value&);
};

using ArchiveEntryIterator = typename std::list<ArchiveEntryMetadata>::iterator;

class ArchiveMetadata {
public:
    std::string archiveName;
    std::string archiveFormatName;
    int archiveFormat;
    std::list<ArchiveEntryMetadata> entryMetadata;

    std::string focusedEntry;

    ArchiveEntryIterator find(const std::string& name);
    ArchiveEntryIterator eraseEntry(ArchiveEntryIterator position);
    ArchiveEntryIterator insertEntry(ArchiveEntryIterator it, const ArchiveEntryMetadata& entry);

    void seedTempPaths(fileutils::FileManager* file_man, bool keep);

    Json::Value toJson() const;
    static ArchiveMetadata fromJson(const Json::Value&);

private:
    void loadJson(const Json::Value&);
};

class ArchiveStack {
public:
    static ArchiveStack fromJsonString(const std::string& input);
    static ArchiveStack fromJson(const Json::Value& input);
    void push(const ArchiveMetadata& metadata) { stack_.push_back(metadata); }
    ArchiveMetadata pop() { auto x = top(); stack_.pop_back(); return x; }
    ArchiveMetadata top() const { return stack_.back(); }
    void loadJson(const Json::Value& input);
    void loadJsonString(const std::string& input);
    std::string toJsonString() const;
    Json::Value toJson() const;

private:
    std::vector<ArchiveMetadata> stack_;
};

#endif  // EXTENSIONS_LIBARCHIVE_ARCHIVEMETADATA_H_