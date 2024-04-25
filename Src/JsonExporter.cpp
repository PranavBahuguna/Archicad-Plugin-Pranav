#include "JsonExporter.hpp"

#include <fstream>

void JsonExporter::Parse(const std::vector<API_ElemTypeID>& elemTypes, const std::vector<API_PropertyDefinitionFilter>& filters)
{
  // Parse json for all elements of each given type matching the definition filters
  for (API_ElemTypeID elemType : elemTypes)
  {
    GS::Array<API_Guid> propertyGuids;
    GetPropertyGuids(elemType, filters, propertyGuids);
    ParseElementType(elemType, propertyGuids);
  }
}

bool JsonExporter::Export(const std::string& filepath, size_t width) const
{
  // Open a file (overwrites existing one)
  std::ofstream outFile(filepath, std::ofstream::trunc);
  if (!outFile.is_open())
  {
    return false;
  }

  // Write json with given width
  outFile << std::setw(width) << m_json << std::endl;
  outFile.close();

  return true;
}

void JsonExporter::GetPropertyGuids(const API_ElemType& elemType, const std::vector<API_PropertyDefinitionFilter>& filters, GS::Array<API_Guid>& propertyGuids)
{
  // Get property definitions guids for the given element type and filters
  for (API_PropertyDefinitionFilter filter : filters)
  {
    GS::Array<API_PropertyDefinition> propertyDefinitions;
    if (ACAPI_Element_GetPropertyDefinitionsOfDefaultElem(elemType, filter, propertyDefinitions) != NoError)
    {
      continue;
    }

    for (const auto& definition : propertyDefinitions)
    {
      propertyGuids.Push(definition.guid);
    }
  }
}

void JsonExporter::ParseElementType(API_ElemTypeID elemType, const GS::Array<API_Guid>& propertyGuids)
{
  // Get element type name
  GS::UniString elemTypeNameUniStr;
  ACAPI_Element_GetElemTypeName(elemType, elemTypeNameUniStr);
  std::string elemTypeName = elemTypeNameUniStr.ToCStr();

  // Find all elements matching the given type
  GS::Array<API_Guid> elemList;
  if (ACAPI_Element_GetElemList(elemType, &elemList) != NoError)
  {
    return;
  }

  // Parse json for all properties of each element
  for (const API_Guid& elemGuid : elemList)
  {
    json elemJson;
    ParseJsonFromElement(elemGuid, propertyGuids, elemJson);

    if (!elemJson.empty())
    {
      std::string layerName;
      if (GetElementLayer(elemGuid, layerName))
      {
        m_json[layerName][elemTypeName] = elemJson;
      }
      else
      {
        m_json["UNKNOWN LAYER"][elemTypeName] = elemJson;
      }
    }
  }
}

void JsonExporter::ParseJsonFromElement(const API_Guid& elemGuid, const GS::Array<API_Guid>& propertyGuids, json& elemJson)
{
  // Get properties for the element
  GS::Array<API_Property> properties;
  if (ACAPI_Element_GetPropertyValuesByGuid(elemGuid, propertyGuids, properties) != NoError)
  {
    return;
  }

  std::string elemName = APIGuidToString(elemGuid).ToCStr();

  // Parse json for all element properties
  json propertyJson;
  for (const auto& prop : properties)
  {
    ParseJsonFromProperty(prop, propertyJson);
  }

  elemJson[elemName] = propertyJson;
}

void JsonExporter::ParseJsonFromProperty(const API_Property& prop, json& propertyJson)
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
      {
        variantsJson.push_back(variant.intValue);
      }
      break;
    case API_PropertyRealValueType:
      for (const auto& variant : listVariants)
      {
        variantsJson.push_back(variant.doubleValue);
      }
      break;
    case API_PropertyStringValueType:
      for (const auto& variant : listVariants)
      {
        variantsJson.push_back(variant.uniStringValue.ToCStr());
      }
      break;
    case API_PropertyBooleanValueType:
      for (const auto& variant : listVariants)
      {
        variantsJson.push_back(variant.boolValue);
      }
      break;
    case API_PropertyGuidValueType:
      for (const auto& variant : listVariants)
      {
        variantsJson.push_back(APIGuidToString(variant.guidValue).ToCStr());
      }
      break;
    }
    propertyJson[name] = variantsJson;
  }
}

bool JsonExporter::GetElementLayer(const API_Guid& elemGuid, std::string& layerName)
{
  GS::Array<API_PropertyDefinition> definitions;
  if (ACAPI_Element_GetPropertyDefinitions(elemGuid, API_PropertyDefinitionFilter_FundamentalBuiltIn, definitions) != NoError)
  {
    return false;
  }

  // Search for layer name property definition
  API_Guid layerNameGuid;
  bool found = false;
  for (const auto& definition : definitions)
  {
    if (definition.name == "Layer Name")
    {
      layerNameGuid = definition.guid;
      found = true;
    }
  }
  if (!found)
  {
    return false;
  }

  // Obtain layer name value
  API_Property layerNameProperty;
  if (ACAPI_Element_GetPropertyValue(elemGuid, layerNameGuid, layerNameProperty) != NoError)
  {
    return false;
  }

  layerName = layerNameProperty.value.singleVariant.variant.uniStringValue.ToCStr();
  return true;
}