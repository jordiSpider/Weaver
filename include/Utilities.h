#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "Types.h"
#include <string>
#include <fstream>

// Library nlohmann JSON
#include "nlohmann/json.h"

class OutputStreamConverter {
public:
    enum OutputStream {
        Terminal,
        Log
    };

    inline static OutputStream stringToEnumValue(const std::string &str) { return stringToEnum.at(str); }
private:
    static std::unordered_map<std::string, OutputStream> stringToEnum;
};

std::string getResultFolderName(const std::string& resultFolder);
fs::path obtainOutputDirectory(const nlohmann::json& configuration);
std::string createOutputFile(std::ofstream &file, fs::path filenameRoot, std::string filename, std::string extension, std::ios_base::openmode = std::ofstream::out);
std::string createOutputFile(std::ofstream &file, fs::path filenameRoot, std::string extension, date_type timeStep, unsigned int recordEach, std::ios_base::openmode = std::ofstream::out);
void setOutputStream(std::ofstream& logFile, const fs::path& outputDirectory, const std::string& outputStreamType);
nlohmann::json readConfigFile(fs::path configPath);
nlohmann::json readConfigFile(fs::path configPath, fs::path schemaPath);

#endif // UTILITIES_H_
