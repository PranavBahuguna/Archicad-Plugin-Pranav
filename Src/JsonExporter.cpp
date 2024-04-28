#include "JsonExporter.hpp"

#include <fstream>

void JsonExporter::Parse(const GS::Array<API_ElemTypeID>& elemTypes, const GS::Array<API_PropertyDefinitionFilter>& filters, json& resultJson) const
{
  // Parse json for all elements of each given type matching the definition filters
  for (API_ElemTypeID elemType : elemTypes)
  {
    GS::Array<API_Guid> propertyGuids;
    GetPropertyGuids(elemType, filters, propertyGuids);
    ParseElementType(elemType, propertyGuids, resultJson);
  }
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

void JsonExporter::ParseElementType(API_ElemTypeID elemType, const GS::Array<API_Guid>& propertyGuids, json& elemTypeJson) const
{
  // Get element type name
  GS::UniString elemTypeNameUniStr;
  ACAPI_Element_GetElemTypeName(elemType, elemTypeNameUniStr);
  std::string elemTypeName = elemTypeNameUniStr.ToCStr();

  // Find all elements matching the given type
  GS::Array<API_Guid> elemList;
  if (ACAPI_Element_GetElemList(elemType, &elemList) != NoError)
    return;

  // Parse json for all properties of each element
  for (const API_Guid& elemGuid : elemList)
  {
    json elemJson;
    if (!ParseJsonFromElement(elemGuid, propertyGuids, elemJson))
      continue;

    std::string layerName;
    if (GetElementLayer(elemGuid, layerName))
      elemTypeJson[layerName][elemTypeName] = elemJson;
    else
      elemTypeJson["UNKNOWN LAYER"][elemTypeName] = elemJson;
  }
}

bool JsonExporter::ParseJsonFromElement(const API_Guid& elemGuid, const GS::Array<API_Guid>& propertyGuids, json& elemJson) const
{
  // Get properties for the element
  GS::Array<API_Property> properties;
  if (ACAPI_Element_GetPropertyValuesByGuid(elemGuid, propertyGuids, properties) != NoError || properties.IsEmpty())
    return false;

  std::string elemName = APIGuidToString(elemGuid).ToCStr();

  // Parse json for all element properties
  json propertyJson;
  for (const auto& prop : properties)
    ParseJsonFromProperty(prop, propertyJson);

  elemJson[elemName] = propertyJson;
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

void JsonExporter::GetPropertyGuids(const API_ElemType& elemType, const GS::Array<API_PropertyDefinitionFilter>& filters, GS::Array<API_Guid>& propertyGuids) const
{
  // Get property definitions guids for the given element type and filters
  for (API_PropertyDefinitionFilter filter : filters)
  {
    GS::Array<API_PropertyDefinition> propertyDefinitions;
    if (ACAPI_Element_GetPropertyDefinitionsOfDefaultElem(elemType, filter, propertyDefinitions) != NoError)
      continue;

    for (const auto& definition : propertyDefinitions)
      propertyGuids.Push(definition.guid);
  }
}

bool JsonExporter::GetElementLayer(const API_Guid& elemGuid, std::string& layerName) const
{
  GS::Array<API_PropertyDefinition> definitions;
  if (ACAPI_Element_GetPropertyDefinitions(elemGuid, API_PropertyDefinitionFilter_FundamentalBuiltIn, definitions) != NoError)
    return false;

  // Search for layer name property definition
  auto it = std::find_if(definitions.Begin(), definitions.End(), [&](const API_PropertyDefinition& p) { return p.name == "Layer Name"; });
  if (it == definitions.End())
    return false;

  API_Guid layerNameGuid = it->guid;

  // Obtain layer name value
  API_Property layerNameProperty;
  if (ACAPI_Element_GetPropertyValue(elemGuid, layerNameGuid, layerNameProperty) != NoError)
    return false;

  layerName = layerNameProperty.value.singleVariant.variant.uniStringValue.ToCStr();
  return true;
}