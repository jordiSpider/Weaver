
#include "Tools/Config/Config.h"


using namespace std;
namespace fs = filesystem;

Config::Config(fs::path config_path)
    : root_folder(config_path), version(root_folder.getVersion())
{
    if(getVersion() == VersionNumber())
    {
        cerr << "Error: Configuration files are not in the same version." << endl;
        exit(EXIT_FAILURE);
    }
}

Config::~Config()
{

}

void Config::save(fs::path new_config_path, const bool overwrite)
{
    if(fs::exists(new_config_path))
    {
        if(overwrite)
        {
            fs::remove_all(new_config_path);
        }
        else
        {
            throwLineInfoException("Error: The '" + new_config_path.string() + "' directory already exists.");
        }
    }

    fs::create_directory(new_config_path);

    root_folder.save(new_config_path);
}

void Config::refresh_version(VersionNumber new_version)
{
    version = new_version;
    root_folder.refresh_version(new_version);
}

Folder& Config::getRootFolder()
{
    return root_folder;
}

const VersionNumber& Config::getVersion() const
{
    return version;
}
