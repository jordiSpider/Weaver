#ifndef CONFIG_H_
#define CONFIG_H_


#include <iostream>
#include <cstdlib>
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>

#include "Tools/Config/Folder.h"
#include "Tools/Config/VersionNumber.h"


class Config
{
private:
    Folder root_folder;
    VersionNumber version;

    Folder& getRootFolder();
    JsonFile* getFile(const std::vector<std::string> &folder_path, const std::string json_filename);

    std::vector<std::string> replaceItemPathMatch(std::vector<std::string> &regex_item_path, std::vector<std::vector<std::string>> &valuesToReplace);

public:
    Config(boost::filesystem::path config_path);
    virtual ~Config();

    void save(boost::filesystem::path new_config_path, const bool overwrite);
    void refresh_version(VersionNumber new_version);
    const VersionNumber& getVersion() const;

    Folder* getFolder(const std::vector<std::string> &folder_path, const unsigned int finalIndex);

    void addFolder(std::vector<std::string> &&folder_path);

    void moveFolder(std::vector<std::string> &&initial_folder_path, std::vector<std::string> &&final_folder_path);

    void removeFolder(std::vector<std::string> &&folder_path);

    void addFile(std::vector<std::string> &&folder_path, std::string &&json_file);

    void moveFile(std::vector<std::string> &&initial_folder_path, std::vector<std::string> &&final_folder_path, std::string &&initial_json_file, std::string &&final_json_file);

    void removeFile(std::vector<std::string> &&folder_path, const std::string &&json_file);

    void removeJsonItem(std::vector<std::string> &folder_path, std::vector<std::string> &item_path, const std::string &json_filename);
    void removeJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path, const std::string &json_filename);
    void removeJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path, const std::string &&json_filename);
    void removeJsonItemAllFiles(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path);

    void addJsonItem(const std::vector<std::string> &folder_path, const std::vector<std::string> &item_path, const nlohmann::json &item_value, const std::string &json_filename);
    void addJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path, nlohmann::json &&item_value, const std::string &json_filename);
    void addJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path, nlohmann::json &&item_value, const std::string &&json_filename);
    void addJsonItemAllFiles(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path, nlohmann::json &&item_value);
    void regexAddJsonItem(std::vector<std::string> &folder_path, std::vector<std::string> &pattern_path, std::vector<std::string> &item_regex_path, nlohmann::json &item_value, const std::string &json_filename);
    void regexAddJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&pattern_path, std::vector<std::string> &&item_regex_path, nlohmann::json &&item_value, const std::string &&json_filename);
    void regexAddJsonItemAllFiles(std::vector<std::string> &&folder_path, std::vector<std::string> &&pattern_path, std::vector<std::string> &&item_regex_path, nlohmann::json &&item_value);

    void moveJsonItem(std::vector<std::string> &folder_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_filename);
    void moveJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &json_filename);
    void moveJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_filename);
    void moveJsonItemAllFiles(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path);
    void regexMoveJsonItem(std::vector<std::string> &folder_path, std::vector<std::string> &item_regex_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_filename);
    void regexMoveJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_filename);
    void regexMoveJsonItemAllFiles(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path);

    void copyJsonItem(std::vector<std::string> &folder_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_filename);
    void copyJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &json_filename);
    void copyJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_filename);
    void copyJsonItemAllFiles(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path);
    void regexCopyJsonItem(std::vector<std::string> &folder_path, std::vector<std::string> &item_regex_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_filename);
    void regexCopyJsonItem(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_filename);
    void regexCopyJsonItemAllFiles(std::vector<std::string> &&folder_path, std::vector<std::string>&& item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path);
};

#endif /* CONFIG_H_ */