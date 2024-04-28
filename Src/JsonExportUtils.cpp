#include "JsonExporter.hpp"
#include "JsonExportDialog.hpp"

using json = nlohmann::json;

namespace JsonExportUtils::details
{
  void GetElementTypes(const GS::Array<GS::UniString>& elemTypeNames, GS::Array<API_ElemTypeID>& elemTypes)
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

  bool IsAnyElementsSelected()
  {
    API_SelectionInfo selectionInfo;
    GS::Array<API_Neig> selNeigs;
    if (ACAPI_Selection_Get(&selectionInfo, &selNeigs, false) != NoError || selectionInfo.sel_nElem == 0)
      return false;

    return true;
  }

  void GetSelectedElements(GS::Array<API_Guid>& elemGuids)
  {
    API_SelectionInfo selectionInfo;
    GS::Array<API_Neig> selNeigs;
    if (ACAPI_Selection_Get(&selectionInfo, &selNeigs, false) != NoError)
      return;

    for (const API_Neig& neig : selNeigs)
    {
      API_Guid elemGuid;
      if (ACAPI_Selection_GetSelectedElement(&neig, &elemGuid) == NoError)
        elemGuids.Push(elemGuid);
    }
  }

  void GetAllElementTypeNames(GS::Array<GS::UniString>& elemTypeNames)
  {
    // Obtain all available element types names
    for (int i = API_FirstElemType; i <= API_LastElemType; ++i)
    {
      API_ElemType elemType = API_ElemTypeID(i);
      GS::UniString elemTypeName;
      if (ACAPI_Element_GetElemTypeName(elemType, elemTypeName) == NoError)
        elemTypeNames.Push(elemTypeName);
    }
  }

  void GetElementTypeNamesFromSelection(GS::Array<GS::UniString>& elemTypeNames)
  {
    GS::Array<API_Guid> elemGuids;
    GetSelectedElements(elemGuids);

    for (int i = API_FirstElemType; i <= API_LastElemType; ++i)
    {
      // Obtain all elements for the given element type
      GS::Array<API_Guid> elemGuidsForType;
      API_ElemType elemType = API_ElemTypeID(i);
      if (ACAPI_Element_GetElemList(elemType, &elemGuidsForType) != NoError)
        continue;

      // Check if there are any common elements between the selected and the given element type
      auto it = std::find_first_of(elemGuids.Begin(), elemGuids.End(), elemGuidsForType.Begin(), elemGuidsForType.End());
      if (it == elemGuids.End())
        continue;

      // Obtain the name of the element type and add
      GS::UniString elemTypeName;
      if (ACAPI_Element_GetElemTypeName(elemType, elemTypeName) == NoError)
        elemTypeNames.Push(elemTypeName);
    }
  }

  void StartExport(const JsonExportSettingsData& data)
  {
    std::string filePath = data.filePath.ToCStr();

    GS::Array<API_ElemTypeID> elemTypes;
    GetElementTypes(data.elemTypeNames, elemTypes);

    json resultJson;
    JsonExporter jsonExporter;
    jsonExporter.Parse(elemTypes, data.propertyDefinitionFilters, resultJson);
    jsonExporter.Export(resultJson, filePath);
  }

  void OpenExportDialog()
  {
    JsonExportDialog jsonExportDialog(IsAnyElementsSelected());
    if (jsonExportDialog.Invoke()) {
      const auto& settingsData = jsonExportDialog.GetSettingsData();
      StartExport(settingsData);
    }
  }
};