#ifndef FOLDER_H_
#define FOLDER_H_


#include <iostream>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <memory>

#include "Tools/Config/JsonFile.h"
#include "Tools/Config/VersionNumber.h"


class Folder
{
private:
    std::unordered_map<std::string, std::unique_ptr<Folder>> subfolders;
    std::unordered_map<std::string, JsonFile> json_files;

    VersionNumber version;
    bool inconsistentVersion;

public:
    Folder();
    Folder(std::filesystem::path folder_path);
    virtual ~Folder();

    unsigned int size();
    void save(std::filesystem::path new_root_folder_path);
    void refresh_version(VersionNumber new_version);
    VersionNumber& getVersion();
    std::unordered_map<std::string, std::unique_ptr<Folder>>& getSubFolders();
    std::unordered_map<std::string, JsonFile>& getJsonFiles();
};



#endif /* FOLDER_H_ */