#pragma once

#include "ACAPinc.h"
#include "json.hpp"

using json = nlohmann::json;

struct ElementData {
  API_Guid elemGuid;
  API_ElemTypeID elemTypeId;
  GS::UniString layerName;
};

typedef std::unordered_map<API_ElemTypeID, GS::Array<API_Guid>> ElementTypePropertiesMap;

class JsonExporter {
public:
  JsonExporter() = delete; // prevent instantiation of this class

  static void Parse(const GS::Array<ElementData>& elemDataList, const ElementTypePropertiesMap& elemTypePropertiesMap, json& resultJson);
  static bool ExportToFile(const json& exportJson, const std::string& filepath, size_t width = 2);

private:
  static void ParseElement(const ElementData& elemData, const GS::Array<API_Guid>& propertyDefinitionGuids, json& elemJson);
  static bool ParseJsonFromElement(const API_Guid& elemGuid, const GS::Array<API_Guid>& propertyDefinitionGuids, json& elemPropertiesJson);
  static void ParseJsonFromProperty(const API_Property& prop, json& propertyJson);
};