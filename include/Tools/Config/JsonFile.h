#ifndef JSON_FILE_H_
#define JSON_FILE_H_


#include <unordered_map>
#include <boost/filesystem.hpp>
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
    JsonFile(boost::filesystem::path json_file_path);
    virtual ~JsonFile();

    void save(boost::filesystem::path new_file_path);
    void refresh_version(VersionNumber new_version);
    nlohmann::json& getContent();
    VersionNumber& getVersion();
};



#endif /* JSON_FILE_H_ */