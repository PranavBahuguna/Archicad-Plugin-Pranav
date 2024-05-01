#pragma once

#include "JsonExportDialog.hpp"
#include "ElementData.hpp"

class JsonExportUtils {
public:
  JsonExportUtils() = delete; // prevent instantiation of this class

  static void BuildData(const GS::Array<GS::UniString> elemTypeNames, const GS::Array<API_PropertyDefinitionFilter> propertyDefinitionFilters, bool selectedOnly, GS::Array<ElementData>& elemData);
  static void GetAvailableElementTypeNames(bool selectionOnly, GS::Array<GS::UniString>& elemTypeNames);
  static bool IsAnyElementsSelected();

private:
  static void BuildElementData(const GS::Array<API_Guid>& elemGuids, const GS::Array<API_PropertyDefinitionFilter>& filters, GS::Array<ElementData>& data);
  static bool GetElementProperties(const API_Guid& elemGuid, const GS::Array<API_PropertyDefinitionFilter>& filters, GS::Array<API_Property>& properties);
  static void GetElementsFromTypes(const GS::Array<API_ElemTypeID>& elemTypes, GS::Array<API_Guid>& elemGuids);
  static void FilterElementsByType(const GS::Array<API_ElemTypeID>& elemTypes, const GS::Array<API_Guid>& inputElemGuids, GS::Array<API_Guid>& outputGuids);
  static void GetSelectedElements(GS::Array<API_Guid>& elemGuids);
  static void GetElementTypesFromNames(const GS::Array<GS::UniString>& elemTypeNames, GS::Array<API_ElemTypeID>& elemTypes);
};