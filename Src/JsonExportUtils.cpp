#include "JsonExportUtils.hpp"

#include <algorithm>

/**
 * @brief Constructs a list of element data from the application
 * @param[in] settingsData Settings for determining what data to extract from the current project
 * @param[out] elemData The output data for json parsing
 */
void JsonExportUtils::BuildData(
  const GS::Array<GS::UniString> elemTypeNames,
  const GS::Array<API_PropertyDefinitionFilter> propertyDefinitionFilters,
  bool selectedOnly,
  GS::Array<ElementData> &elemData)
{
  // Obtain all element types and guids
  GS::Array<API_ElemTypeID> elemTypes;
  GetElementTypesFromNames(elemTypeNames, elemTypes);

  GS::Array<API_Guid> elemGuids;
  if (selectedOnly)
  {
    GS::Array<API_Guid> selectedGuids;
    GetSelectedElements(selectedGuids);
    FilterElementsByType(elemTypes, selectedGuids, elemGuids);
  }
  else
  {
    GetElementsFromTypes(elemTypes, elemGuids);
  }

  // Construct data for parsing
  BuildElementData(elemGuids, propertyDefinitionFilters, elemData);
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

void JsonExportUtils::BuildElementData(const GS::Array<API_Guid>& elemGuids, const GS::Array<API_PropertyDefinitionFilter>& filters, GS::Array<ElementData>& data)
{
  for (const API_Guid& elemGuid : elemGuids)
  {
    // Get header data
    API_Elem_Head header;
    BNZeroMemory(&header, sizeof(API_Elem_Head));
    header.guid = elemGuid;

    if (ACAPI_Element_GetHeader(&header) != NoError)
      continue;

    // Get properies data
    GS::Array<API_Property> properties;
    if (!GetElementProperties(elemGuid, filters, properties))
      continue;

    // Get layer name
    GS::UniString layerName;
    API_Attribute attrib;
    BNZeroMemory(&attrib, sizeof(API_Attribute));

    attrib.header.typeID = API_LayerID;
    attrib.header.index = header.layer;
    bool layerAttribFound = ACAPI_Attribute_Get(&attrib) == NoError;
    layerName = layerAttribFound ? attrib.header.name : "UNKNOWN LAYER";

    data.PushNew(elemGuid, header.type.typeID, layerName, properties);
  }
}

bool JsonExportUtils::GetElementProperties(const API_Guid& elemGuid, const GS::Array<API_PropertyDefinitionFilter>& filters, GS::Array<API_Property>& properties)
{
  // Get all property definitions for the given element and filters
  GS::Array<API_PropertyDefinition> propertyDefinitions;
  for (API_PropertyDefinitionFilter filter : filters)
  {
    if (ACAPI_Element_GetPropertyDefinitions(elemGuid, filter, propertyDefinitions) != NoError)
      continue;
  }

  // Try obtaining property values from the given properties
  if (ACAPI_Element_GetPropertyValues(elemGuid, propertyDefinitions, properties) != NoError || properties.IsEmpty())
    return false;

  return true;
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

void JsonExportUtils::FilterElementsByType(const GS::Array<API_ElemTypeID>& elemTypes, const GS::Array<API_Guid>& inputElemGuids, GS::Array<API_Guid>& outputGuids)
{
  for (const API_Guid& inputGuid : inputElemGuids)
  {
    // Get header data
    API_Elem_Head header;
    BNZeroMemory(&header, sizeof(API_Elem_Head));
    header.guid = inputGuid;

    if (ACAPI_Element_GetHeader(&header) != NoError)
      continue;

    // Add elements whose type is contained in the type filters
    if (elemTypes.Contains(header.type.typeID))
      outputGuids.Push(inputGuid);
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
    if (ACAPI_Selection_GetSelectedElement(&neig, &elemGuid) != NoError)
      continue;

    // Ignore duplicates
    if (!elemGuids.Contains(elemGuid))
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