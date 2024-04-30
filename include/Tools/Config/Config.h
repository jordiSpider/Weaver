#ifndef CONFIG_H_
#define CONFIG_H_


#include <iostream>
#include <cstdlib>
#include <boost/filesystem.hpp>

#include "Tools/Config/Folder.h"
#include "Tools/Config/VersionNumber.h"


class Config
{
private:
    Folder root_folder;
    VersionNumber version;

public:
    Config(boost::filesystem::path config_path);
    virtual ~Config();

    void save(boost::filesystem::path new_config_path, const bool overwrite);
    void refresh_version(VersionNumber new_version);
    Folder& getRootFolder();
    const VersionNumber& getVersion() const;
};

#endif /* CONFIG_H_ */