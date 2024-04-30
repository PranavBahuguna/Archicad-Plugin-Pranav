#pragma once

#include "ACAPinc.h"
#include "JsonExportDialog.hpp"
#include "JsonExporter.hpp"

class JsonExportUtils {
public:
  JsonExportUtils() = delete; // prevent instantiation of this class

  static void OpenExportDialog();
  static void GetAvailableElementTypeNames(bool selectionOnly, GS::Array<GS::UniString>& elemTypeNames);
  static bool IsAnyElementsSelected();

private:
  static void StartExport(const JsonExportSettingsData& data);
  static void BuildElementData(const GS::Array<API_Guid>& elemGuids, GS::Array<ElementData>& data);
  static void BuildElementTypePropertiesMap(const GS::Array<API_ElemTypeID>& elemTypes, const GS::Array<API_PropertyDefinitionFilter>& filters, ElementTypePropertiesMap& elemTypePropertiesMap);
  static void GetElementsFromTypes(const GS::Array<API_ElemTypeID>& elemTypes, GS::Array<API_Guid>& elemGuids);
  static void GetSelectedElements(GS::Array<API_Guid>& elemGuids);
  static void GetElementTypesFromNames(const GS::Array<GS::UniString>& elemTypeNames, GS::Array<API_ElemTypeID>& elemTypes);
};