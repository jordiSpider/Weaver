#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "Types.h"
#include "LineInfoException.h"
#include "ValidatorJSON.h"
#include "Output.h"
#include <string>
#include <fstream>

// Library nlohmann JSON
#include "nlohmann/json.h"

#include "magic_enum/magic_enum.h"



std::string getResultFolderName(const std::string& resultFolder);
fs::path obtainResultFolder(const nlohmann::json& configuration, fs::path outputFolder);
std::string createOutputFile(FILE** file, fs::path filenameRoot, std::string filename, std::string extension, std::string openMode = "w");
std::string createOutputFile(FILE** file, fs::path filenameRoot, std::string filename, std::string extension, date_type timeStep, unsigned int recordEach, std::string openMode = "w");
bool isFileOpen(FILE* file);
nlohmann::json readConfigFile1(fs::path configPath, fs::path WeaverFolder);
nlohmann::json readConfigFile(fs::path configPath, fs::path schemaPath);

#endif // UTILITIES_H_
