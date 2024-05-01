#include "JsonParser.hpp"

/**
 * @brief Transforms a collection of supplied element and properties data into json format
 * @param[in] elemDataList Array of element data to process
 * @param[out] resultJson The json structure to write to
 */
void JsonParser::Parse(const GS::Array<ElementData>& elemDataList, json& resultJson)
{
  for (const ElementData& elemData : elemDataList)
  {
    if (!elemData.properties.IsEmpty())
      ParseElement(elemData, resultJson);
  }
}

void JsonParser::ParseElement(const ElementData& elemData, json& elemJson)
{
  // Get element properties json
  json elemPropertiesJson;
  for (const API_Property& prop : elemData.properties)
    ParseJsonFromProperty(prop, elemPropertiesJson);

  // Get element type name
  GS::UniString elemTypeNameUniStr;
  ACAPI_Element_GetElemTypeName(elemData.elemTypeId, elemTypeNameUniStr);
  std::string elemTypeName = elemTypeNameUniStr.ToCStr();

  // Get element and layer names
  std::string elemName = APIGuidToString(elemData.elemGuid).ToCStr();
  std::string layerName = elemData.layerName.ToCStr();

  elemJson[layerName][elemTypeName][elemName] = elemPropertiesJson;
}

void JsonParser::ParseJsonFromProperty(const API_Property& prop, json& propertyJson)
{
  std::string name = prop.definition.name.ToCStr();
  bool isSingle =
    prop.definition.collectionType != API_PropertyListCollectionType &&
    prop.definition.collectionType != API_PropertyMultipleChoiceEnumerationCollectionType;

  if (isSingle)
  {
    // Extract json string value from single variant
    const auto& singleVariant = prop.value.singleVariant.variant;

    switch (prop.definition.valueType)
    {
    case API_PropertyUndefinedValueType:
      propertyJson[name] = "";
      break;
    case API_PropertyIntegerValueType:
      propertyJson[name] = singleVariant.intValue;
      break;
    case API_PropertyRealValueType:
      propertyJson[name] = singleVariant.doubleValue;
      break;
    case API_PropertyStringValueType:
      propertyJson[name] = singleVariant.uniStringValue.ToCStr();
      break;
    case API_PropertyBooleanValueType:
      propertyJson[name] = singleVariant.boolValue;
      break;
    case API_PropertyGuidValueType:
      propertyJson[name] = APIGuidToString(singleVariant.guidValue).ToCStr();
    }
  }
  else
  {
    // Construct json string value from list variant
    const auto& listVariants = prop.value.listVariant.variants;
    json variantsJson = json::array();

    switch (prop.definition.valueType)
    {
    case API_PropertyUndefinedValueType:
      break;
    case API_PropertyIntegerValueType:
      for (const auto& variant : listVariants)
        variantsJson.push_back(variant.intValue);
      break;
    case API_PropertyRealValueType:
      for (const auto& variant : listVariants)
        variantsJson.push_back(variant.doubleValue);
      break;
    case API_PropertyStringValueType:
      for (const auto& variant : listVariants)
        variantsJson.push_back(variant.uniStringValue.ToCStr());
      break;
    case API_PropertyBooleanValueType:
      for (const auto& variant : listVariants)
        variantsJson.push_back(variant.boolValue);
      break;
    case API_PropertyGuidValueType:
      for (const auto& variant : listVariants)
        variantsJson.push_back(APIGuidToString(variant.guidValue).ToCStr());
      break;
    }
    propertyJson[name] = variantsJson;
  }
}