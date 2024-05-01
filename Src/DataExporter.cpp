#include "DataExporter.hpp"

//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "ThirdParty/httplib.h"

#include <iostream>
#include <fstream>

/**
 * @brief Writes a json structure to file. Constructs a new file if one does not already exist and will
 * overwrite existing ones.
 * @param[in] exportJson The json to export
 * @param[in] filePath The path of the file to write to
 * @param[in] width The indent width for json elements
 * @param[out] errorStr Error message output if export was unsuccessful
 * @returns True if file could be sucessfully opened
 */
bool DataExporter::ExportToFile(const json& exportJson, const std::string& filePath, int width, std::string& errorStr)
{
  try
  {
    // Open a file and write JSON to it
    std::ofstream outFile(filePath, std::ofstream::trunc);
    if (!outFile.is_open())
    {
      errorStr = "";
      return false;
    }

    outFile << std::setw(width) << exportJson << std::endl;
    outFile.close();
  }
  catch (std::exception& e)
  {
    errorStr = e.what();
    return false;
  }
  return true;
}

/**
 * @brief Writes a json structure to a url
 * @param[in] exportJson The json to export
 * @param[in] linkPath The link to send a POST message to
 * @param[in] width The indent width for json elements
 * @param[out] errorStr Error message output if export was unsuccessful
 * @returns True if the data could be successfully sent
 */
bool DataExporter::ExportToUrl(const json& exportJson, const std::string& baseUrl, int width, std::string& errorStr)
{
  // Clip last slash in case it was left on
  std::string baseUrlStr = baseUrl;
  if (baseUrlStr.back() == '/')
    baseUrlStr = baseUrlStr.substr(0, baseUrl.size() - 1);

  try
  {
    // Open a connection and post JSON to the url endpoint
    httplib::Client cli(baseUrlStr);
    httplib::Result result = cli.Post("/post", exportJson.dump(width), "application/json");

    if (!result || result->status != httplib::StatusCode::OK_200)
    {
      errorStr = httplib::to_string(result.error());
      return false;
    }
  }
  catch (std::exception& e)
  {
    errorStr = e.what();
    return false;
  }
  return true;
}
