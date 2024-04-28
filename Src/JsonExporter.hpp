#pragma once

#include "ACAPinc.h"
#include "json.hpp"

using json = nlohmann::json;

class JsonExporter {
public:
  void Parse(const GS::Array<API_ElemTypeID>& elemTypes, const GS::Array<API_PropertyDefinitionFilter>& filters, json& resultJson) const;
  bool Export(const json &exportJson, const std::string& filepath, size_t width = 2) const;

private:
  void ParseElementType(API_ElemTypeID elemType, const GS::Array<API_Guid>& propertyDefinitionGuids, json& elemTypeJson) const;
  bool ParseJsonFromElement(const API_Guid& elemGuid, const GS::Array<API_Guid>& propertyDefinitionGuids, json& elemJson) const;
  void ParseJsonFromProperty(const API_Property& prop, json& propertyJson) const;
  void GetPropertyGuids(const API_ElemType& elemType, const GS::Array<API_PropertyDefinitionFilter>& filters, GS::Array<API_Guid>& filterGuids) const;
  bool GetElementLayer(const API_Guid& elemGuid, std::string& layerName) const;
};