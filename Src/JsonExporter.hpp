#pragma once

#include "ACAPinc.h"
#include "json.hpp"

using json = nlohmann::json;

class JsonExporter {
public:
  void Parse(const std::vector<API_ElemTypeID>& elemTypes, const std::vector<API_PropertyDefinitionFilter>& filters);
  bool Export(const std::string& filepath, size_t width = 2) const;

private:
  void ParseElementType(API_ElemTypeID elemType, const GS::Array<API_Guid>& propertyDefinitionGuids);
  void GetPropertyGuids(const API_ElemType& elemType, const std::vector<API_PropertyDefinitionFilter>& filters, GS::Array<API_Guid>& filterGuids);
  void ParseJsonFromElement(const API_Guid& elemGuid, const GS::Array<API_Guid>& propertyDefinitionGuids, json& elemJson);
  void ParseJsonFromProperty(const API_Property& prop, json& propertyJson);

  json m_json;
};