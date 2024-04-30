#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>
#include <fstream>
// Library nlohmann JSON
#include <nlohmann/json.hpp>
#include <magic_enum.hpp>
#include <limits>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <regex>
#include <sstream>
#include <stack>
#include <set>
#include <string>
#include <chrono>
#include <boost/filesystem.hpp>

#include "Types.h"
#include "Exceptions/LineInfoException.h"
#include "ValidatorJSON.h"
#include "GlobalVariable.h"


std::string getResultFolderName(const std::string& baseName);
boost::filesystem::path obtainResultFolder(const std::string& baseName, boost::filesystem::path outputFolder);
std::string createOutputFile(std::ofstream &file, boost::filesystem::path filenameRoot, std::string filename, std::string extension, std::ios_base::openmode openMode = std::ofstream::out);
std::string createOutputFile(std::ofstream &file, boost::filesystem::path filenameRoot, std::string filename, std::string extension, date_type timeStep, unsigned int recordEach, std::ios_base::openmode openMode = std::ofstream::out);
nlohmann::json readConfigFile(boost::filesystem::path configPath);
nlohmann::json readConfigFile(boost::filesystem::path configPath, boost::filesystem::path schemaPath);
void saveConfigFile(boost::filesystem::path configPath, nlohmann::json fileContent);

/**
 * @brief Converts a project version string to a numeric representation.
 *
 * This function takes a project version string in the format "YY.MM.DD" and
 * converts it to a numeric representation (e.g., 23 * 10000 + 12 * 1000 + 04).
 *
 * @param versionString The project version string to be converted.
 * @return The numeric representation of the version string.
 *
 * @note This function assumes that the version string follows the "YY.MM.DD" format.
 * @warning Ensure that the version string is in the correct format; otherwise, the result may be inaccurate.
 */
unsigned int projectVersionStringToNumber(std::string versionString);

#endif // UTILITIES_H_
