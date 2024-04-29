#pragma once

#include "ACAPinc.h"
#include "JsonExportDialog.hpp"
#include "JsonExporter.hpp"

namespace JsonExportUtils
{
  void GetAvailableElementTypeNames(bool selectionOnly, GS::Array<GS::UniString>& elemTypeNames);
  void GetElementTypesFromNames(const GS::Array<GS::UniString>& elemTypeNames, GS::Array<API_ElemType>& elemTypes);
  bool IsAnyElementsSelected();
  void GetSelectedElements(GS::Array<API_Guid>& elemGuids);
  void GetElementsFromTypes(const GS::Array<API_ElemType>& elemTypes, GS::Array<API_Guid>& elemGuids);
  void BuildElementData(const GS::Array<API_Guid>& elemGuids, GS::Array<NewElementData>& data);
  void BuildElementTypePropertiesMap(const GS::Array<API_ElemType>& elemTypes, const GS::Array<API_PropertyDefinitionFilter>& filters, ElementTypePropertiesMap& elemTypePropertiesMap);
  void StartExport(const JsonExportSettingsData& data);
  void OpenExportDialog();
};