#pragma once

#include "ACAPinc.h"
#include "JsonExportDialog.hpp"

namespace JsonExportUtils::details
{
  void GetElementTypes(const GS::Array<GS::UniString>& elemTypeNames, GS::Array<API_ElemTypeID>& elemTypes);
  bool IsAnyElementsSelected();
  void GetSelectedElements(GS::Array<API_Guid>& elemGuids);
  void GetAllElementTypeNames(GS::Array<GS::UniString>& elemTypes);
  void GetElementTypeNamesFromSelection(GS::Array<GS::UniString> &elemTypes);
  void StartExport(const JsonExportSettingsData& data);
  void OpenExportDialog();
};