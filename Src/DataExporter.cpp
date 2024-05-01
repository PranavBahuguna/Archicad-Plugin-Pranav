#include "DataExporter.hpp"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "ThirdParty/httplib.h"

#include <iostream>
#include <fstream>

/**
 * @brief Writes a json structure to file. Constructs a new file if one does not already exist and will
 * overwrite existing ones.
 * @param[in] exportJson The json to export
 * @param[in] filePath The path of the file to write to
 * @param[in] width The indent width for json elements
 * @returns True if file could be sucessfully opened
 */
bool DataExporter::ExportToFile(const json& exportJson, const std::string& filePath, int width)
{
  // Open a file
  std::ofstream outFile(filePath, std::ofstream::trunc);
  if (!outFile.is_open())
    return false;

  // Write json with given width
  outFile << std::setw(width) << exportJson << std::endl;
  outFile.close();
  return true;
}

/**
 * @brief Writes a json structure to a url
 * @param[in] exportJson The json to export
 * @param[in] linkPath The link to send a POST message to
 * @param[in] width The indent width for json elements
 * @returns True if the data could be successfully sent
 */
bool DataExporter::ExportToUrl(const json& exportJson, const std::string& baseUrl, int width, std::string& errorStr)
{
  httplib::Client cli(baseUrl);
  httplib::Result result = cli.Post("/post", exportJson.dump(width), "application/json");

  if (!result || result->status != httplib::StatusCode::OK_200)
  {
    errorStr = httplib::to_string(result.error());
    return false;
  }

  return true;
}
