#pragma once

#include "ACAPinc.h"
#include "ThirdParty/json.hpp"
#include "ElementData.hpp"

using json = nlohmann::json;

class JsonParser {
public:
  JsonParser() = delete; // prevent instantiation of this class

  static void Parse(const GS::Array<ElementData>& elemDataList, json& resultJson);

private:
  static void ParseElement(const ElementData& elemData, json& elemJson);
  static void ParseJsonFromProperty(const API_Property& prop, json& propertyJson);
};