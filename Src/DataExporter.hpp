#pragma once

#include "Thirdparty/json.hpp"

using json = nlohmann::json;

class DataExporter {
public:
  DataExporter() = delete; // prevent instantiation of this class

  static bool ExportToFile(const json& exportJson, const std::string& filePath, int width, std::string& errorStr);
  static bool ExportToUrl(const json& exportJson, const std::string& baseUrl, int width, std::string& errorStr);
};
