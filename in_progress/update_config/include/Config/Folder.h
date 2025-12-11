#ifndef FOLDER_H_
#define FOLDER_H_


#include <iostream>
#include <cstdlib>
#include <fstream>
#include <boost/filesystem.hpp>
#include <unordered_map>
#include <string>
#include <memory>

#include "Tools/Config/JsonFile.h"
#include "Tools/Config/VersionNumber.h"


class Folder
{
private:
    std::unordered_map<std::string, Folder*> subfolders;
    std::unordered_map<std::string, JsonFile> json_files;

    VersionNumber version;
    bool inconsistentVersion;

public:
    Folder();
    Folder(boost::filesystem::path folder_path);
    virtual ~Folder();

    unsigned int size();
    void save(boost::filesystem::path new_root_folder_path);
    void refresh_version(VersionNumber new_version);
    VersionNumber& getVersion();
    std::unordered_map<std::string, Folder*>& getSubFolders();
    std::unordered_map<std::string, JsonFile>& getJsonFiles();
    JsonFile& getFile(const std::string& filename);

    void setFolder(const std::string& foldername, Folder* newFolder);
    void removeFolder(const std::string& foldername);
    void moveFolderTo(const std::string& foldername, Folder* final_root_folder, const std::string& final_foldername);

    void addFile(std::string &json_file);
    void setFile(const std::string& filename, JsonFile& newFile);
    void removeFile(const std::string& filename);
};



#endif /* FOLDER_H_ */