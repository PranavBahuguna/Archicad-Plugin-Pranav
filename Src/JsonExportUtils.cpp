#include "JsonExportUtils.hpp"

namespace JsonExportUtils
{
  void GetAvailableElementTypeNames(bool selectionOnly, GS::Array<GS::UniString>& elemTypeNames)
  {
    GS::Array<API_Guid> selectedGuids;
    if (selectionOnly)
      GetSelectedElements(selectedGuids);

    // Obtain all available element type names
    for (int i = API_FirstElemType; i <= API_LastElemType; ++i)
    {
      API_ElemType elemType = API_ElemTypeID(i);
      GS::Array<API_Guid> elemGuids;
      if (ACAPI_Element_GetElemList(elemType, &elemGuids) != NoError || elemGuids.IsEmpty())
        continue;

      if (selectionOnly)
      {
        auto it = std::find_first_of(elemGuids.Begin(), elemGuids.End(), selectedGuids.Begin(), selectedGuids.End());
        if (it == elemGuids.End())
          continue;
      }

      GS::UniString elemTypeName;
      if (ACAPI_Element_GetElemTypeName(elemType, elemTypeName) == NoError)
        elemTypeNames.Push(elemTypeName);
    }
  }

  void GetElementTypesFromNames(const GS::Array<GS::UniString>& elemTypeNames, GS::Array<API_ElemType>& elemTypes)
  {
    for (int i = API_FirstElemType; i <= API_LastElemType; ++i)
    {
      API_ElemType elemType = API_ElemTypeID(i);

      // Skip if no elements could be found for this element type
      GS::Array<API_Guid> elemGuids;
      if (ACAPI_Element_GetElemList(elemType, &elemGuids) != NoError || elemGuids.IsEmpty())
        continue;

      GS::UniString elemName;
      if (ACAPI_Element_GetElemTypeName(elemType, elemName) != NoError)
        continue;

      if (elemTypeNames.Contains(elemName))
        elemTypes.Push(elemType);
    }
  }

  bool IsAnyElementsSelected()
  {
    API_SelectionInfo selectionInfo;
    GSErrCode error = ACAPI_Selection_Get(&selectionInfo, nullptr, false);
    return error == NoError && selectionInfo.sel_nElem > 0;
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

  void GetElementsFromTypes(const GS::Array<API_ElemType>& elemTypes, GS::Array<API_Guid>& elemGuids)
  {
    for (const API_ElemType& elemType : elemTypes)
    {
      GS::Array<API_Guid> elemTypeGuids;
      if (ACAPI_Element_GetElemList(elemType, &elemGuids) == NoError)
        elemTypeGuids.Append(elemGuids);
    }
  }

  void BuildElementData(const GS::Array<API_Guid>& elemGuids, GS::Array<NewElementData>& data)
  {
    for (const API_Guid& elemGuid : elemGuids)
    {
      // Get header data
      API_Elem_Head header;
      BNZeroMemory(&header, sizeof(API_Elem_Head));
      header.guid = elemGuid;

      if (ACAPI_Element_GetHeader(&header) != NoError)
        continue;

      // Get layer name
      GS::UniString layerName;
      API_Attribute attrib;
      BNZeroMemory(&attrib, sizeof(API_Attribute));

      attrib.header.typeID = API_LayerID;
      attrib.header.index = header.layer;
      bool layerAttribFound = ACAPI_Attribute_Get(&attrib) == NoError;
      layerName = layerAttribFound ? attrib.header.name : "UNKNOWN LAYER";

      data.PushNew(elemGuid, header.type, layerName);
    }
  }

  void BuildElementTypePropertiesMap(const GS::Array<API_ElemType>& elemTypes, const GS::Array<API_PropertyDefinitionFilter>& filters, ElementTypePropertiesMap& elemTypePropertiesMap)
  {
    // Get property definitions guids for the given element type and filters
    for (const API_ElemType& elemType : elemTypes)
    {
      for (API_PropertyDefinitionFilter filter : filters)
      {
        GS::Array<API_PropertyDefinition> propertyDefinitions;
        if (ACAPI_Element_GetPropertyDefinitionsOfDefaultElem(elemType, filter, propertyDefinitions) != NoError)
          continue;

        for (const auto& definition : propertyDefinitions)
          elemTypePropertiesMap[elemType.typeID].Push(definition.guid);
      }
    }
  }

  void StartExport(const JsonExportSettingsData& data)
  {
    std::string filePath = data.filePath.ToCStr();

    // Obtain all element types and guids
    GS::Array<API_ElemType> elemTypes;
    GetElementTypesFromNames(data.elemTypeNames, elemTypes);

    GS::Array<API_Guid> elemGuids;
    if (data.selectedOnly)
      GetSelectedElements(elemGuids);
    else
      GetElementsFromTypes(elemTypes, elemGuids);

    // Construct data and properties map for parsing
    GS::Array<NewElementData> newElemData;
    BuildElementData(elemGuids, newElemData);

    ElementTypePropertiesMap elemTypePropertiesMap;
    BuildElementTypePropertiesMap(elemTypes, data.propertyDefinitionFilters, elemTypePropertiesMap);

    // Parse data and export json to file
    json resultJson;
    JsonExporter jsonExporter;
    jsonExporter.Parse(newElemData, elemTypePropertiesMap, resultJson);
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