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
#ifndef MAIN_MANIFEST_H_
#define MAIN_MANIFEST_H_

#include "agent/agent_version.h"
#include "agent/build_description.h"
#include "agent/agent_docs.h"
#include "core/ClassLoader.h"
#include "tinyxml2.h"
#include "Main.h"


void produce_manifest(std::ostream &out) {
  out << "Nar-Group: org.apache.nifi" << std::endl;
  out << "Nar-Id: minifi" << std::endl;
  out << "Nar-Version: " << minifi::AgentBuild::VERSION << std::endl;
  out << "Build-Tag: " << minifi::AgentBuild::BUILD_REV << std::endl;
  out << "Build-Revision: " << minifi::AgentBuild::BUILD_REV << std::endl;
  out << "Build-Timestamp: " << minifi::AgentBuild::BUILD_DATE << std::endl;
}

void addExtension(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *element, const minifi::ClassDescription &desc, const std::string &type) {
  auto extension = doc.NewElement("extension");

  auto name = doc.NewElement("name");
  name->SetText(desc.class_name_.c_str());
  extension->InsertEndChild(name);

  auto class_type = doc.NewElement("type");
  class_type->SetText(type.c_str());
  extension->InsertEndChild(class_type);

  auto description = doc.NewElement("description");
  std::string classDescription;
  minifi::AgentDocs::getDescription(desc.class_name_, classDescription);
  description->SetText(classDescription.c_str());
  extension->InsertEndChild(description);

  auto properties = doc.NewElement("properties");
  for (const auto &class_property : desc.class_properties_) {

    auto property = doc.NewElement("property");

    auto property_name = doc.NewElement("name");
    property_name->SetText(class_property.second.getName().c_str());
    property->InsertEndChild(property_name);

    auto displayName = doc.NewElement("displayName");
    displayName->SetText(class_property.second.getDisplayName().c_str());
    property->InsertEndChild(displayName);

    auto description = doc.NewElement("description");
    description->SetText(class_property.second.getDescription().c_str());
    property->InsertEndChild(description);

    if (!class_property.second.getDefaultValue().empty()) {
      auto defaultValue = doc.NewElement("defaultValue");
      defaultValue->SetText(class_property.second.getDefaultValue().to_string().c_str());
      property->InsertEndChild(defaultValue);
    }

    // controll service definit
    // allowable values
    auto required = doc.NewElement("required");
    required->SetText(class_property.second.getRequired());
    property->InsertEndChild(required);

    const auto allowableValues = class_property.second.getAllowedValues();
    if (!allowableValues.empty()) {
      auto allowableValuesElements = doc.NewElement("allowableValues");
      for (const auto &allowableValue : allowableValues) {
        auto alowableValueElement = doc.NewElement("allowableValue");

        auto alowableValueValue = doc.NewElement("value");
        alowableValueValue->SetText(allowableValue.to_string().c_str());

        alowableValueElement->InsertEndChild(alowableValueValue);

        allowableValuesElements->InsertEndChild(alowableValueElement);
      }

      property->InsertEndChild(allowableValuesElements);
    }

    properties->InsertEndChild(property);
  }
  extension->InsertEndChild(properties);

  auto relationships = doc.NewElement("relationships");
  for (const auto &rel : desc.class_relationships_) {
    auto relationship = doc.NewElement("relationship");

    auto rel_name = doc.NewElement("name");
    rel_name->SetText(rel.getName().c_str());
    relationship->InsertEndChild(rel_name);

    auto rel_description = doc.NewElement("description");
    rel_description->SetText(rel.getDescription().c_str());
    relationship->InsertEndChild(rel_description);

    relationships->InsertEndChild(relationship);
  }
  extension->InsertEndChild(relationships);

  element->InsertEndChild(extension);

}

const std::string create_extension_manifest() {
  tinyxml2::XMLDocument doc;

  auto extensionManifest = doc.NewElement("extensionManifest");
  doc.InsertFirstChild(extensionManifest);

  // add system API version
  auto systemApiVersion = doc.NewElement("systemApiVersion");
  systemApiVersion->SetText(minifi::AgentBuild::VERSION);
  extensionManifest->InsertEndChild(systemApiVersion);

  auto extensions = doc.NewElement("extensions");

  for (auto group : minifi::AgentBuild::getExtensions()) {
    std::cout << "Adding " << group << std::endl;
    for (auto clazz : core::ClassLoader::getDefaultClassLoader().getClasses(group)) {
      std::cout << "should have " << clazz << std::endl;
    }
    struct minifi::Components components = minifi::BuildDescription::getClassDescriptions(group);
    for (const auto component : components.processors_) {
      std::cout << "Adding " + component.class_name_ << std::endl;
      addExtension(doc, extensions, component, "PROCESSOR");
    }

    //serializeClassDescription(components.processors_, "processors", resp);
    //serializeClassDescription(components.controller_services_, "controllerServices", resp);

  }

  extensionManifest->InsertEndChild(extensions);

  const auto property = std::make_shared<minifi::Properties>();
  auto generator = utils::IdGenerator::getIdGenerator();
  generator->initialize(property);
  utils::Identifier ident;
  generator->generate(ident);

  std::string file = temp_dir() + "/" + ident.to_string();
  doc.SaveFile(file.c_str());
  return file;
}

#endif /* MAIN_MANIFEST_H_ */
