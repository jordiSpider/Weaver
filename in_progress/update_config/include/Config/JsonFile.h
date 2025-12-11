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

    nlohmann::json* getItem(const std::vector<std::string> &item_path, const unsigned int finalIndex);
    void setItem(const std::vector<std::string> &item_path, const nlohmann::json &value);
    void moveItem(std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path);
    void copyItem(std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path);
    void removeItem(std::vector<std::string> &item_path);

    std::vector<std::vector<std::vector<std::string>>> matchItemPath(nlohmann::json &json_content, std::vector<std::string> &regex_item_path, unsigned int initial_index);
    std::vector<std::vector<std::vector<std::string>>> matchItemPath(std::vector<std::string> &regex_item_path, unsigned int initial_index = 0);
};



#endif /* JSON_FILE_H_ */