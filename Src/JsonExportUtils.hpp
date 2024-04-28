#pragma once

#include "JsonExporter.hpp"
#include "JsonExportDialog.hpp"

using json = nlohmann::json;

namespace JsonExportUtils::details
{
  static void GetElementTypes(const GS::Array<GS::UniString>& elemTypeNames, GS::Array<API_ElemTypeID>& elemTypes)
  {
    // Create map of element type names to ids
    std::unordered_map<std::string, API_ElemTypeID> allElemTypes;
    for (int i = API_FirstElemType; i <= API_LastElemType; ++i)
    {
      API_ElemTypeID elemTypeId = API_ElemTypeID(i);
      API_ElemType elemType = elemTypeId;
      GS::UniString elemName;
      if (ACAPI_Element_GetElemTypeName(elemType, elemName) == NoError)
        allElemTypes[elemName.ToCStr().Get()] = elemTypeId;
    }

    // Extract all element names from text edit and build element types list
    for (GS::UniString name : elemTypeNames)
    {
      name.Trim();
      std::string nameStr = name.ToCStr();
      if (allElemTypes.find(nameStr) != allElemTypes.end())
        elemTypes.Push(allElemTypes[nameStr]);
    }
  }

  static void StartExport(const JsonExportSettingsData& data)
  {
    std::string filePath = data.filePath.ToCStr();

    GS::Array<API_ElemTypeID> elemTypes;
    GetElementTypes(data.elemTypeNames, elemTypes);

    json resultJson;
    JsonExporter jsonExporter;
    jsonExporter.Parse(elemTypes, data.propertyDefinitionFilters, resultJson);
    jsonExporter.Export(resultJson, filePath);
  }

  static void GetAvailableElementTypesText(GS::UniString& elemTypesText)
  {
    // Obtain all available element types and fill the type filters
    // with comma separated options
    for (int i = API_FirstElemType; i <= API_LastElemType; ++i)
    {
      API_ElemType elemType = API_ElemTypeID(i);
      GS::UniString elemName;
      if (ACAPI_Element_GetElemTypeName(elemType, elemName) != NoError)
        continue;

      elemTypesText += elemName;
      if (i < API_LastElemType)
        elemTypesText += ", ";
    }
  }
}

namespace JsonExportUtils
{
  static void OpenExportDialog()
  {
    GS::UniString elementTypesText;
    details::GetAvailableElementTypesText(elementTypesText);
    JsonExportDialog jsonExportDialog(elementTypesText);

    if (jsonExportDialog.Invoke()) {
      const auto& settingsData = jsonExportDialog.GetSettingsData();
      details::StartExport(settingsData);
    }
  }
};