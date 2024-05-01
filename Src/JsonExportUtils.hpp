#pragma once

#include "ElementData.hpp"
#include "JsonExportSettingsData.hpp"
#include "Thirdparty/json.hpp"

using json = nlohmann::json;

class JsonExportUtils {
public:
  JsonExportUtils() = delete; // prevent instantiation of this class

  static void RunExportProcess(const JsonExportSettingsData& settingsData);
  static void GetAvailableElementTypeNames(bool selectionOnly, GS::Array<GS::UniString>& elemTypeNames);
  static bool IsAnyElementsSelected();

private:
  static void BuildElementData(const GS::Array<API_Guid>& elemGuids, const GS::Array<API_PropertyDefinitionFilter>& filters, GS::Array<ElementData>& data);
  static bool GetElementProperties(const API_Guid& elemGuid, const GS::Array<API_PropertyDefinitionFilter>& filters, GS::Array<API_Property>& properties);
  static void GetElementsFromTypes(const GS::Array<API_ElemTypeID>& elemTypes, GS::Array<API_Guid>& elemGuids);
  static void FilterElementsByType(const GS::Array<API_ElemTypeID>& elemTypes, const GS::Array<API_Guid>& inputElemGuids, GS::Array<API_Guid>& outputGuids);
  static void GetSelectedElements(GS::Array<API_Guid>& elemGuids);
  static void GetElementTypesFromNames(const GS::Array<GS::UniString>& elemTypeNames, GS::Array<API_ElemTypeID>& elemTypes);
  static void RunExportToFile(const GS::UniString& filePath, const json& exportJson);
  static void RunExportToUrl(const GS::UniString& baseUrl, const json& exportJson);
};