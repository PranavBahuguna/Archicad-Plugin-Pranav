#pragma once

#include "ACAPinc.h"
#include "json.hpp"

using json = nlohmann::json;

struct NewElementData {
  API_Guid elemGuid;
  API_ElemType elemType;
  GS::UniString layerName;
};

struct ElementData {
  API_Guid elemGuid;
  GS::UniString layerName;
};

struct ElementsForTypeData {
  API_ElemType elemType;
  GS::Array<ElementData> elemDataList;
  GS::Array<API_Guid> propertyGuids;
};

typedef std::unordered_map<API_ElemTypeID, GS::Array<API_Guid>> ElementTypePropertiesMap;

class JsonExporter {
public:
  void Parse(const GS::Array<NewElementData>& elemDataList, const ElementTypePropertiesMap& elemTypePropertiesMap, json& resultJson) const;
  void Parse(const GS::Array<ElementsForTypeData>& elemDataList, json& resultJson) const;
  bool Export(const json& exportJson, const std::string& filepath, size_t width = 2) const;

private:
  void ParseElement(const NewElementData& elemData, const GS::Array<API_Guid>& propertyDefinitionGuids, json& elemJson) const;

  void ParseElement(const ElementsForTypeData& elemData, json& elemJson) const;
  bool ParseJsonFromElement(const API_Guid& elemGuid, const GS::Array<API_Guid>& propertyDefinitionGuids, json& elemPropertiesJson) const;
  void ParseJsonFromProperty(const API_Property& prop, json& propertyJson) const;
};