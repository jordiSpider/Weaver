#ifndef JSON_FILE_H_
#define JSON_FILE_H_


#include <unordered_map>
#include <filesystem>
#include <string>
#include <nlohmann/json.hpp>

#include "Misc/Utilities.h"
#include "Tools/Config/VersionNumber.h"


class JsonFile
{
private:
    nlohmann::json content;
    VersionNumber version;

public:
    JsonFile();
    JsonFile(std::filesystem::path json_file_path);
    virtual ~JsonFile();

    void save(std::filesystem::path new_file_path);
    void refresh_version(VersionNumber new_version);
    nlohmann::json& getContent();
    VersionNumber& getVersion();
};



#endif /* JSON_FILE_H_ */