#include "APIEnvir.h"
#include "DataExporter.hpp"
#include "JsonExportDialog.hpp"
#include "JsonParser.hpp"
#include "JsonExportUtils.hpp"

static const GSResID AddOnInfoID = ID_ADDON_INFO;
static const Int32 AddOnNameID = 1;
static const Int32 AddOnDescriptionID = 2;

static const short AddOnMenuID = ID_ADDON_MENU;
static const Int32 AddOnCommandID = 1;

static const int JsonIndentWidth = 2;

static void RunExportToFile(const GS::UniString& filePath, const json& exportJson)
{
  std::string filePathStr = filePath.ToCStr();
  if (DataExporter::ExportToFile(exportJson, filePathStr, JsonIndentWidth))
  {
    GS::UniString alertText = "Data sucessfully written to " + filePath;
    DGAlert(DG_INFORMATION, "Export to File", "", alertText, "OK");
  }
  else
  {
    GS::UniString alertText = "Error: could not write data to " + filePath;
    DGAlert(DG_ERROR, "Export to File", "", alertText, "OK");
  }
}

static void RunExportToUrl(const GS::UniString& baseUrl, const json& exportJson)
{
  std::string baseUrlStr = baseUrl.ToCStr();
  std::string errorStr;
  if (DataExporter::ExportToUrl(exportJson, baseUrlStr, JsonIndentWidth, errorStr))
  {
    GS::UniString alertText = "Data sucessfully exported to " + baseUrl;
    DGAlert(DG_INFORMATION, "Export to URL", "", alertText, "OK");
  }
  else
  {
    DGAlert(DG_ERROR, "Export to URL", "", GS::UniString(errorStr), "OK");
  }
}

static void StartExport(const JsonExportSettingsData& settingsData)
{
  // Build element data dialog settings data
  GS::Array<ElementData> elemData;
  JsonExportUtils::BuildData(
    settingsData.elemTypeNames,
    settingsData.propertyDefinitionFilters,
    settingsData.selectedOnly,
    elemData);

  // Parse json from the data and properties map
  json resultJson;
  JsonParser::Parse(elemData, resultJson);

  // Export to file
  if (settingsData.exportToFile)
    RunExportToFile(settingsData.filePath, resultJson);

  // Export to url
  if (settingsData.exportToLink)
    RunExportToUrl(settingsData.baseUrl, resultJson);
}

static void RunJsonExportDialog()
{
  JsonExportDialog jsonExportDialog;
  if (jsonExportDialog.Invoke())
  {
    auto settingsData = jsonExportDialog.GetSettingsData();
    StartExport(settingsData);
  }
}

static GSErrCode MenuCommandHandler(const API_MenuParams* menuParams)
{
  switch (menuParams->menuItemRef.menuResID) {
  case AddOnMenuID:
    switch (menuParams->menuItemRef.itemIndex) {
    case AddOnCommandID:
      RunJsonExportDialog();
      break;
    }
    break;
  }
  return NoError;
}

API_AddonType CheckEnvironment(API_EnvirParams* envir)
{
  RSGetIndString(&envir->addOnInfo.name, AddOnInfoID, AddOnNameID, ACAPI_GetOwnResModule());
  RSGetIndString(&envir->addOnInfo.description, AddOnInfoID, AddOnDescriptionID, ACAPI_GetOwnResModule());

  return APIAddon_Normal;
}

GSErrCode RegisterInterface(void)
{
#ifdef ServerMainVers_2700
  return ACAPI_MenuItem_RegisterMenu(AddOnMenuID, 0, MenuCode_Tools, MenuFlag_Default);
#else
  return ACAPI_Register_Menu(AddOnMenuID, 0, MenuCode_Tools, MenuFlag_Default);
#endif
}

GSErrCode Initialize(void)
{
#ifdef ServerMainVers_2700
  return ACAPI_MenuItem_InstallMenuHandler(AddOnMenuID, MenuCommandHandler);
#else
  return ACAPI_Install_MenuHandler(AddOnMenuID, MenuCommandHandler);
#endif
}

GSErrCode FreeData(void)
{
  return NoError;
}
