
#include "Tools/Config/Config.h"


using namespace std;
using json = nlohmann::json;
namespace fs = boost::filesystem;


Folder& Config::getRootFolder()
{
    return root_folder;
}

Folder* Config::getFolder(const vector<string> &folder_path, const unsigned int finalIndex)
{
    Folder* folder = &getRootFolder();
    
    for(unsigned int i = 0; i < finalIndex; i++)
    {
        folder = folder->getSubFolders()[folder_path[i]].get();
    }

    return folder;
}

JsonFile* Config::getFile(const vector<string> &folder_path, const string json_filename)
{
    Folder* folder = getFolder(folder_path, folder_path.size());

    return &folder->getJsonFiles()[json_filename];
}


vector<string> Config::replaceItemPathMatch(vector<string> &regex_item_path, vector<vector<string>> &valuesToReplace)
{
    regex match_pattern("<match_(\\d+)_(\\d+)>");
    regex regex_content_pattern("<regex_content>");

    vector<string> item_path;

    for(string &elem : regex_item_path)
    {
        string replace_elem = elem;

        sregex_iterator match_iter(replace_elem.begin(), replace_elem.end(), match_pattern);
        sregex_iterator end;

        while(match_iter != end) {
            smatch match = *match_iter;
            
            size_t pos = replace_elem.find(match[0].str());
            replace_elem.replace(pos, match[0].str().length(), valuesToReplace[stoi(match[1].str())][stoi(match[2].str())]);
            
            ++match_iter;
        }

        sregex_iterator regex_content_iter(replace_elem.begin(), replace_elem.end(), regex_content_pattern);

        while(regex_content_iter != end) {
            smatch match = *regex_content_iter;
            
            size_t pos = replace_elem.find(match[0].str());
            replace_elem.replace(pos, match[0].str().length(), json::parse(valuesToReplace.back().back()));
            
            ++regex_content_iter;
        }

        item_path.push_back(replace_elem);
    }

    return item_path;
}


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

const VersionNumber& Config::getVersion() const
{
    return version;
}


void Config::addFolder(vector<string> &&folder_path)
{
    if(folder_path.size() == 0)
    {
        throwLineInfoException("Error: The folder cannot be the root folder of the configuration.");
    }


    Folder* root_folder = getFolder(folder_path, folder_path.size()-1);
    root_folder->getSubFolders()[folder_path.back()] = make_unique<Folder>();
}


void Config::moveFolder(vector<string> &&initial_folder_path, vector<string> &&final_folder_path)
{
    if(initial_folder_path.size() == 0)
    {
        throwLineInfoException("Error: The initial folder cannot be the root folder of the configuration.");
    }

    if(final_folder_path.size() == 0)
    {
        throwLineInfoException("Error: The final folder cannot be the root folder of the configuration.");
    }


    Folder* initial_root_folder = getFolder(initial_folder_path, initial_folder_path.size()-1);
    Folder* final_root_folder = getFolder(final_folder_path, final_folder_path.size()-1);

    initial_root_folder->moveFolderTo(initial_folder_path.back(), final_root_folder, final_folder_path.back());
}


void Config::removeFolder(std::vector<std::string> &&folder_path)
{
    if(folder_path.size() == 0)
    {
        throwLineInfoException("Error: The folder cannot be the root folder of the configuration.");
    }


    Folder* root_folder = getFolder(folder_path, folder_path.size()-1);
    delete root_folder->getSubFolders()[folder_path.back()];
    root_folder->removeFolder(folder_path.back());
}


void Config::addFile(vector<string> &&folder_path, string &&json_file)
{
    if(json_file.size() == 0)
    {
        throwLineInfoException("Error: The name of the new json file must be specified.");
    }


    Folder* folder = getFolder(folder_path, folder_path.size());
    folder->addFile(json_file);
}


void Config::moveFile(vector<string> &&initial_folder_path, vector<string> &&final_folder_path, string &&initial_json_file, string &&final_json_file)
{
    if(initial_json_file.size() == 0)
    {
        throwLineInfoException("Error: The name of the initial json file must be specified.");
    }

    if(final_json_file.size() == 0)
    {
        throwLineInfoException("Error: The name of the final json file must be specified.");
    }


    Folder* initial_folder = getFolder(initial_folder_path, initial_folder_path.size());
    Folder* final_folder = getFolder(final_folder_path, final_folder_path.size());

    JsonFile* fileToMove = &initial_folder->getFile(initial_json_file);
    final_folder->setFile(final_json_file, *fileToMove);
    initial_folder->removeFile(initial_json_file);
}


void Config::removeFile(vector<string> &&folder_path, const string &&json_file)
{
    if(json_file.size() == 0)
    {
        throwLineInfoException("Error: The name of the json file must be specified.");
    }


    Folder* folder = getFolder(folder_path, folder_path.size());
    folder->removeFile(json_file);
}


void Config::removeJsonItem(vector<string> &folder_path, vector<string> &item_path, const string &json_filename)
{
    if(json_filename.size() == 0)
    {
        throwLineInfoException("Error: The name of the json file must be specified.");
    }

    if(item_path.size() == 0)
    {
        throwLineInfoException("Error: The item cannot be the root of the json content.");
    }


    JsonFile* json_file = getFile(folder_path, json_filename);
    json_file->removeItem(item_path);
}

void Config::removeJsonItem(vector<string> &&folder_path, vector<string> &&item_path, const string &json_filename)
{
    removeJsonItem(folder_path, item_path, json_filename);
}

void Config::removeJsonItem(vector<string> &&folder_path, vector<string> &&item_path, const string &&json_filename)
{
    removeJsonItem(folder_path, item_path, json_filename);
}

void Config::removeJsonItemAllFiles(vector<string> &&folder_path, vector<string> &&item_path)
{
    if(item_path.size() == 0)
    {
        throwLineInfoException("Error: The item cannot be the root of the json content.");
    }


    Folder* folder = getFolder(folder_path, folder_path.size());

    for(auto& [filename, file] : folder->getJsonFiles())
    {
        removeJsonItem(folder_path, item_path, filename);
    }
}


void Config::addJsonItem(const vector<string> &folder_path, const vector<string> &item_path, const json &item_value, const string &json_filename)
{
    JsonFile* json_file = getFile(folder_path, json_filename);
    json_file->setItem(item_path, item_value);
}

void Config::addJsonItem(vector<string> &&folder_path, vector<string> &&item_path, json &&item_value, const string &json_filename)
{
    addJsonItem(folder_path, item_path, item_value, json_filename);
}

void Config::addJsonItem(vector<string> &&folder_path, vector<string> &&item_path, json &&item_value, const string &&json_filename)
{
    addJsonItem(folder_path, item_path, item_value, json_filename);
}

void Config::addJsonItemAllFiles(vector<string> &&folder_path, vector<string> &&item_path, json &&item_value)
{
    Folder* folder = getFolder(folder_path, folder_path.size());

    for(auto& [filename, file] : folder->getJsonFiles())
    {
        addJsonItem(folder_path, item_path, item_value, filename);
    }
}

void Config::regexAddJsonItem(vector<string> &folder_path, vector<string> &pattern_path, vector<string> &item_regex_path, json &item_value, const string &json_filename)
{
    JsonFile* json_file = getFile(folder_path, json_filename);

    vector<vector<vector<string>>> matches = json_file->matchItemPath(pattern_path);

    for(vector<vector<string>>& match : matches)
    {
        vector<string> item_path = replaceItemPathMatch(item_regex_path, match);

        addJsonItem(folder_path, item_path, item_value, json_filename);
    }
}

void Config::regexAddJsonItem(vector<string> &&folder_path, vector<string> &&pattern_path, vector<string> &&item_regex_path, json &&item_value, const string &&json_filename)
{
    regexAddJsonItem(folder_path, pattern_path, item_regex_path, item_value, json_filename);
}

void Config::regexAddJsonItemAllFiles(vector<string> &&folder_path, vector<string> &&pattern_path, vector<string> &&item_regex_path, json &&item_value)
{
    Folder* folder = getFolder(folder_path, folder_path.size());

    for(auto& [filename, file] : folder->getJsonFiles())
    {
        regexAddJsonItem(folder_path, pattern_path, item_regex_path, item_value, filename);
    }
}


void Config::moveJsonItem(vector<string> &folder_path, vector<string> &initial_item_path, vector<string> &final_item_path, const string &json_filename)
{
    if(json_filename.size() == 0)
    {
        throwLineInfoException("Error: The name of the json file must be specified.");
    }

    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }


    JsonFile* json_file = getFile(folder_path, json_filename);
    json_file->moveItem(initial_item_path, final_item_path);
}

void Config::moveJsonItem(vector<string> &&folder_path, vector<string> &&initial_item_path, vector<string> &&final_item_path, const string &json_filename)
{
    moveJsonItem(folder_path, initial_item_path, final_item_path, json_filename);
}

void Config::moveJsonItem(vector<string> &&folder_path, vector<string> &&initial_item_path, vector<string> &&final_item_path, const string &&json_filename)
{
    moveJsonItem(folder_path, initial_item_path, final_item_path, json_filename);
}

void Config::moveJsonItemAllFiles(vector<string> &&folder_path, vector<string> &&initial_item_path, vector<string> &&final_item_path)
{
    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }


    Folder* folder = getFolder(folder_path, folder_path.size());

    for(auto& [filename, file] : folder->getJsonFiles())
    {
        moveJsonItem(folder_path, initial_item_path, final_item_path, filename);
    }
}

void Config::regexMoveJsonItem(vector<string> &folder_path, vector<string> &item_regex_path, vector<string> &initial_item_path, vector<string> &final_item_path, const string &json_filename)
{
    if(json_filename.size() == 0)
    {
        throwLineInfoException("Error: The name of the json file must be specified.");
    }

    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }


    JsonFile* json_file = getFile(folder_path, json_filename);

    vector<vector<vector<string>>> matches = json_file->matchItemPath(item_regex_path);

    for(vector<vector<string>>& match : matches)
    {
        vector<string> match_initial_item_path = replaceItemPathMatch(initial_item_path, match);
        vector<string> match_final_item_path = replaceItemPathMatch(final_item_path, match);

        moveJsonItem(folder_path, match_initial_item_path, match_final_item_path, json_filename);
    }
}

void Config::regexMoveJsonItem(vector<string> &&folder_path, vector<string> &&item_regex_path, vector<string> &&initial_item_path, vector<string> &&final_item_path, const string &&json_filename)
{
    regexMoveJsonItem(folder_path, item_regex_path, initial_item_path, final_item_path, json_filename);
}

void Config::regexMoveJsonItemAllFiles(vector<string> &&folder_path, vector<string> &&item_regex_path, vector<string> &&initial_item_path, vector<string> &&final_item_path)
{
    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }


    Folder* folder = getFolder(folder_path, folder_path.size());

    for(auto& [filename, file] : folder->getJsonFiles())
    {
        regexMoveJsonItem(folder_path, item_regex_path, initial_item_path, final_item_path, filename);
    }
}


void Config::copyJsonItem(vector<string> &folder_path, vector<string> &initial_item_path, vector<string> &final_item_path, const string &json_filename)
{
    if(json_filename.size() == 0)
    {
        throwLineInfoException("Error: The name of the json file must be specified.");
    }

    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }


    JsonFile* json_file = getFile(folder_path, json_filename);
    json_file->copyItem(initial_item_path, final_item_path);
}

void Config::copyJsonItem(vector<string> &&folder_path, vector<string> &&initial_item_path, vector<string> &&final_item_path, const string &json_filename)
{
    copyJsonItem(folder_path, initial_item_path, final_item_path, json_filename);
}

void Config::copyJsonItem(vector<string> &&folder_path, vector<string> &&initial_item_path, vector<string> &&final_item_path, const string &&json_filename)
{
    copyJsonItem(folder_path, initial_item_path, final_item_path, json_filename);
}

void Config::copyJsonItemAllFiles(vector<string> &&folder_path, vector<string> &&initial_item_path, vector<string> &&final_item_path)
{
    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }


    Folder* folder = getFolder(folder_path, folder_path.size());

    for(auto& [filename, file] : folder->getJsonFiles())
    {
        copyJsonItem(folder_path, initial_item_path, final_item_path, filename);
    }
}

void Config::regexCopyJsonItem(vector<string> &folder_path, vector<string> &item_regex_path, vector<string> &initial_item_path, vector<string> &final_item_path, const string &json_filename)
{
    if(json_filename.size() == 0)
    {
        throwLineInfoException("Error: The name of the json file must be specified.");
    }

    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }


    JsonFile* json_file = getFile(folder_path, json_filename);

    vector<vector<vector<string>>> matches = json_file->matchItemPath(item_regex_path);

    for(vector<vector<string>>& match : matches)
    {
        vector<string> match_initial_item_path = replaceItemPathMatch(initial_item_path, match);
        vector<string> match_final_item_path = replaceItemPathMatch(final_item_path, match);

        copyJsonItem(folder_path, match_initial_item_path, match_final_item_path, json_filename);
    }
}

void Config::regexCopyJsonItem(vector<string> &&folder_path, vector<string> &&item_regex_path, vector<string> &&initial_item_path, vector<string> &&final_item_path, const string &&json_filename)
{
    regexCopyJsonItem(folder_path, item_regex_path, initial_item_path, final_item_path, json_filename);
}

void Config::regexCopyJsonItemAllFiles(vector<string> &&folder_path, vector<string>&& item_regex_path, vector<string> &&initial_item_path, vector<string> &&final_item_path)
{
    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }


    Folder* folder = getFolder(folder_path, folder_path.size());

    for(auto& [filename, file] : folder->getJsonFiles())
    {
        regexCopyJsonItem(folder_path, item_regex_path, initial_item_path, final_item_path, filename);
    }
}
