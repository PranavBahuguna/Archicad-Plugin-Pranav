#include "APIEnvir.h"
#include "JsonExportDialog.hpp"

static const GSResID AddOnInfoID = ID_ADDON_INFO;
static const Int32 AddOnNameID = 1;
static const Int32 AddOnDescriptionID = 2;

static const short AddOnMenuID = ID_ADDON_MENU;
static const Int32 AddOnCommandID = 1;

static GSErrCode MenuCommandHandler(const API_MenuParams* menuParams)
{
  switch (menuParams->menuItemRef.menuResID) {
  case AddOnMenuID:
    switch (menuParams->menuItemRef.itemIndex) {
    case AddOnCommandID:
      JsonExportDialog().Invoke();
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