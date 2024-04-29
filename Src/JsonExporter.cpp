#include "JsonExporter.hpp"

#include <fstream>

void JsonExporter::Parse(const GS::Array<NewElementData>& elemDataList, const ElementTypePropertiesMap& elemTypePropertiesMap, json& resultJson) const
{
  for (const NewElementData& elemData : elemDataList)
  {
    const auto& typeProperties = elemTypePropertiesMap.at(elemData.elemType.typeID);
    ParseElement(elemData, typeProperties, resultJson);
  }
}

void JsonExporter::Parse(const GS::Array<ElementsForTypeData>& elemDataList, json& resultJson) const
{
  for (const ElementsForTypeData& elemData : elemDataList)
    ParseElement(elemData, resultJson);
}

bool JsonExporter::Export(const json& exportJson, const std::string& filepath, size_t width) const
{
  // Open a file (overwrites existing one)
  std::ofstream outFile(filepath, std::ofstream::trunc);
  if (!outFile.is_open())
    return false;

  // Write json with given width
  outFile << std::setw(width) << exportJson << std::endl;
  outFile.close();
  return true;
}

void JsonExporter::ParseElement(const NewElementData& elemData, const GS::Array<API_Guid>& propertyDefinitionGuids, json& elemJson) const
{
  // Get element properties json
  json elemPropertiesJson;
  if (!ParseJsonFromElement(elemData.elemGuid, propertyDefinitionGuids, elemPropertiesJson))
    return;

  // Get element type name
  GS::UniString elemTypeNameUniStr;
  ACAPI_Element_GetElemTypeName(elemData.elemType, elemTypeNameUniStr);
  std::string elemTypeName = elemTypeNameUniStr.ToCStr();

  // Get element and layer names
  std::string elemName = APIGuidToString(elemData.elemGuid).ToCStr();
  std::string layerName = elemData.layerName.ToCStr();

  elemJson[layerName][elemTypeName][elemName] = elemPropertiesJson;
}

void JsonExporter::ParseElement(const ElementsForTypeData& elemTypeData, json& elemJson) const
{
  // Get element type name
  GS::UniString elemTypeNameUniStr;
  ACAPI_Element_GetElemTypeName(elemTypeData.elemType, elemTypeNameUniStr);
  std::string elemTypeName = elemTypeNameUniStr.ToCStr();

  // Parse json for all properties of each element
  for (const ElementData& elemData : elemTypeData.elemDataList)
  {
    json elemPropertiesJson;
    if (!ParseJsonFromElement(elemData.elemGuid, elemTypeData.propertyGuids, elemPropertiesJson))
      continue;

    std::string elemName = APIGuidToString(elemData.elemGuid).ToCStr();
    std::string layerName = elemData.layerName.ToCStr();

    elemJson[layerName][elemTypeName][elemName] = elemPropertiesJson;
  }
}

bool JsonExporter::ParseJsonFromElement(const API_Guid& elemGuid, const GS::Array<API_Guid>& propertyGuids, json& elemPropertiesJson) const
{
  // Get properties for the element
  GS::Array<API_Property> properties;
  if (ACAPI_Element_GetPropertyValuesByGuid(elemGuid, propertyGuids, properties) != NoError || properties.IsEmpty())
    return false;

  // Parse json for all element properties
  for (const auto& prop : properties)
    ParseJsonFromProperty(prop, elemPropertiesJson);

  return true;
}

void JsonExporter::ParseJsonFromProperty(const API_Property& prop, json& propertyJson) const
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