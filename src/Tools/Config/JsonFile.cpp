
#include "Tools/Config/JsonFile.h"


using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;


JsonFile::JsonFile()
{
    
}

JsonFile::JsonFile(fs::path json_file_path)
{
    content = readConfigFile(json_file_path);
    version = VersionNumber(content["version"]);
}

JsonFile::~JsonFile()
{

}

void JsonFile::save(fs::path new_file_path)
{
    saveConfigFile(new_file_path, content);
}

void JsonFile::refresh_version(VersionNumber new_version)
{
    version = new_version;
    content["version"] = new_version.toString();
}

json& JsonFile::getContent()
{
    return content;
}

VersionNumber& JsonFile::getVersion()
{
    return version;
}
