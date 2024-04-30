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

#include "Types.h"
#include "Exceptions/LineInfoException.h"
#include "ValidatorJSON.h"
#include "GlobalVariable.h"


std::string getResultFolderName(const std::string& baseName);
fs::path obtainResultFolder(const std::string& baseName, fs::path outputFolder);
std::string createOutputFile(std::ofstream &file, fs::path filenameRoot, std::string filename, std::string extension, std::ios_base::openmode openMode = std::ofstream::out);
std::string createOutputFile(std::ofstream &file, fs::path filenameRoot, std::string filename, std::string extension, date_type timeStep, unsigned int recordEach, std::ios_base::openmode openMode = std::ofstream::out);
nlohmann::json readConfigFile(fs::path configPath, fs::path schemaPath);

#endif // UTILITIES_H_
