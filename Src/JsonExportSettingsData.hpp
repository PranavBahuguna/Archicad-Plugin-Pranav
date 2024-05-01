#pragma once

#include "ACAPinc.h"

/**
 * @brief Describes the data required for implementing element parsing and export
 */
struct JsonExportSettingsData
{
  GS::UniString filePath;
  GS::UniString baseUrl;
  bool exportToFile;
  bool exportToUrl;
  GS::Array<API_PropertyDefinitionFilter> propertyDefinitionFilters;
  GS::Array<GS::UniString> elemTypeNames;
  bool selectedOnly;
};