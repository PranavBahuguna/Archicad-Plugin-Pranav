#include "JsonExportUtils.hpp"

/** @brief Initialises the export dialog window */
void JsonExportUtils::OpenExportDialog()
{
  JsonExportDialog jsonExportDialog;
  if (jsonExportDialog.Invoke()) {
    const auto& settingsData = jsonExportDialog.GetSettingsData();
    StartExport(settingsData);
  }
}

/**
 * @brief Obtains the names of all available element types
 * @param[in] selectionOnly If true, obtains type names only from the current selection
 * @param[out] elemTypeNames Array containing element type names
 */
void JsonExportUtils::GetAvailableElementTypeNames(bool selectionOnly, GS::Array<GS::UniString>& elemTypeNames)
{
  GS::Array<API_Guid> selectedGuids;
  if (selectionOnly)
    GetSelectedElements(selectedGuids);

  // Obtain all available element type names
  for (int i = API_FirstElemType; i <= API_LastElemType; ++i)
  {
    API_ElemType elemTypeId = API_ElemTypeID(i);
    GS::Array<API_Guid> elemGuids;
    if (ACAPI_Element_GetElemList(elemTypeId, &elemGuids) != NoError || elemGuids.IsEmpty())
      continue;

    if (selectionOnly)
    {
      auto it = std::find_first_of(elemGuids.Begin(), elemGuids.End(), selectedGuids.Begin(), selectedGuids.End());
      if (it == elemGuids.End())
        continue;
    }

    GS::UniString elemTypeName;
    if (ACAPI_Element_GetElemTypeName(elemTypeId, elemTypeName) == NoError)
      elemTypeNames.Push(elemTypeName);
  }
}

/**
 * @brief Determines if a selection of elements has been made in the application
 * @returns True if any selected elements could be found
 */
bool JsonExportUtils::IsAnyElementsSelected()
{
  API_SelectionInfo selectionInfo;
  GSErrCode error = ACAPI_Selection_Get(&selectionInfo, nullptr, false);
  return error == NoError && selectionInfo.sel_nElem > 0;
}


void JsonExportUtils::StartExport(const JsonExportSettingsData& data)
{
  // Obtain all element types and guids
  GS::Array<API_ElemTypeID> elemTypes;
  GetElementTypesFromNames(data.elemTypeNames, elemTypes);

  GS::Array<API_Guid> elemGuids;
  if (data.selectedOnly)
    GetSelectedElements(elemGuids);
  else
    GetElementsFromTypes(elemTypes, elemGuids);

  // Construct data and properties map for parsing
  GS::Array<ElementData> elemData;
  BuildElementData(elemGuids, elemData);

  ElementTypePropertiesMap elemTypePropertiesMap;
  BuildElementTypePropertiesMap(elemTypes, data.propertyDefinitionFilters, elemTypePropertiesMap);

  // Parse data and export json
  json resultJson;
  JsonExporter::Parse(elemData, elemTypePropertiesMap, resultJson);

  if (data.exportToFile)
  {
    std::string filePath = data.filePath.ToCStr();
    JsonExporter::ExportToFile(resultJson, filePath);
  }
  if (data.exportToLink)
  {
    std::string linkPath = data.linkPath.ToCStr();
    JsonExporter::ExportToLink(resultJson, linkPath);
  }
}

void JsonExportUtils::BuildElementData(const GS::Array<API_Guid>& elemGuids, GS::Array<ElementData>& data)
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

    data.PushNew(elemGuid, header.type.typeID, layerName);
  }
}

void JsonExportUtils::BuildElementTypePropertiesMap(const GS::Array<API_ElemTypeID>& elemTypes, const GS::Array<API_PropertyDefinitionFilter>& filters, ElementTypePropertiesMap& elemTypePropertiesMap)
{
  // Get property definitions guids for the given element type and filters
  for (API_ElemTypeID elemTypeId : elemTypes)
  {
    for (API_PropertyDefinitionFilter filter : filters)
    {
      GS::Array<API_PropertyDefinition> propertyDefinitions;
      if (ACAPI_Element_GetPropertyDefinitionsOfDefaultElem(elemTypeId, filter, propertyDefinitions) != NoError)
        continue;

      for (const auto& definition : propertyDefinitions)
        elemTypePropertiesMap[elemTypeId].Push(definition.guid);
    }
  }
}

void JsonExportUtils::GetElementsFromTypes(const GS::Array<API_ElemTypeID>& elemTypes, GS::Array<API_Guid>& elemGuids)
{
  // Extract element guids from each supplied element type
  for (API_ElemTypeID elemTypeId : elemTypes)
  {
    GS::Array<API_Guid> elemTypeGuids;
    if (ACAPI_Element_GetElemList(elemTypeId, &elemGuids) == NoError)
      elemTypeGuids.Append(elemGuids);
  }
}

void JsonExportUtils::GetSelectedElements(GS::Array<API_Guid>& elemGuids)
{
  // Obtain selection neigs
  API_SelectionInfo selectionInfo;
  GS::Array<API_Neig> selNeigs;
  if (ACAPI_Selection_Get(&selectionInfo, &selNeigs, false) != NoError)
    return;

  // Obtain element guids for each neig
  for (const API_Neig& neig : selNeigs)
  {
    API_Guid elemGuid;
    if (ACAPI_Selection_GetSelectedElement(&neig, &elemGuid) == NoError)
      elemGuids.Push(elemGuid);
  }
}


void JsonExportUtils::GetElementTypesFromNames(const GS::Array<GS::UniString>& elemTypeNames, GS::Array<API_ElemTypeID>& elemTypes)
{
  // Iterate over all element types
  for (int i = API_FirstElemType; i <= API_LastElemType; ++i)
  {
    API_ElemTypeID elemTypeId = API_ElemTypeID(i);
    API_ElemType elemType = elemTypeId;

    // Skip if no elements could be found for this element type
    GS::Array<API_Guid> elemGuids;
    if (ACAPI_Element_GetElemList(elemType, &elemGuids) != NoError || elemGuids.IsEmpty())
      continue;

    // Obtain name for the given element type
    GS::UniString elemName;
    if (ACAPI_Element_GetElemTypeName(elemType, elemName) != NoError)
      continue;

    if (elemTypeNames.Contains(elemName))
      elemTypes.Push(elemTypeId);
  }
}