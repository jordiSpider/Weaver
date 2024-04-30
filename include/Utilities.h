#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "Types.h"
#include "LineInfoException.h"
#include "ValidatorJSON.h"
#include "Output.h"
#include <string>
#include <fstream>

// Library nlohmann JSON
#include <nlohmann/json.hpp>

#include <magic_enum.hpp>

#define FMT_HEADER_ONLY
#include <fmt/core.h>


std::string getResultFolderName(const std::string& resultFolder);
fs::path obtainResultFolder(const nlohmann::json& configuration, fs::path outputFolder);
std::string createOutputFile(std::ofstream &file, fs::path filenameRoot, std::string filename, std::string extension, std::ios_base::openmode openMode = std::ofstream::out);
std::string createOutputFile(std::ofstream &file, fs::path filenameRoot, std::string filename, std::string extension, date_type timeStep, unsigned int recordEach, std::ios_base::openmode openMode = std::ofstream::out);
nlohmann::json readConfigFile1(fs::path configPath, fs::path WeaverFolder);
nlohmann::json readConfigFile(fs::path configPath, fs::path schemaPath);

#endif // UTILITIES_H_
