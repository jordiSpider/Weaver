
#include "Tools/Updater/Change.h"


using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;


Folder* getFolder(Folder& root_folder, vector<string> &folder_path, const unsigned int finalIndex)
{
    Folder* folder = &root_folder;
    
    for(unsigned int i = 0; i < finalIndex; i++)
    {
        folder = folder->getSubFolders()[folder_path[i]].get();
    }

    return folder;
}

json* getJsonFileContent(Folder& root_folder, vector<string> &folder_path, string json_file)
{
    Folder* folder = getFolder(root_folder, folder_path, folder_path.size());
    return &folder->getJsonFiles()[json_file].getContent();
}

json* getJsonItem(json &json_content, vector<string> &item_path, const unsigned int finalIndex)
{
    json* json_item = &json_content;

    for(unsigned int i = 0; i < finalIndex; i++)
    {
        if(json_item->is_array())
        {
            regex pattern("^item_(\\d+)$");

            smatch matches;

            if(regex_search(item_path[i], matches, pattern))
            {
                json_item = &(*json_item)[stoi(matches[1].str())];
            }
            else
            {
                throwLineInfoException("Error: To access a list you must create strings of the style: 'item_{index number}'.");
            }
        }
        else
        {
            json_item = &(*json_item)[item_path[i]];
        }
    }

    return json_item;
}

void setJsonItem(json &json_content, vector<string> &item_path, json &value)
{
    json* json_item = getJsonItem(json_content, item_path, item_path.size()-1);

    if(json_item->is_array())
    {
        regex pattern("^item_(\\d+)$");

        smatch matches;

        if(regex_search(item_path.back(), matches, pattern))
        {
            (*json_item)[stoi(matches[1].str())] = value;
        }
        else
        {
            throwLineInfoException("Error: To access a list you must create strings of the style: 'item_{index number}'.");
        }
    }
    else
    {
        (*json_item)[item_path.back()] = value;
    }
}

void removeJsonItem(json &json_content, vector<string> &item_path)
{
    json* json_item = getJsonItem(json_content, item_path, item_path.size()-1);

    if(json_item->is_array())
    {
        regex pattern("^item_(\\d+)$");

        smatch matches;

        if(regex_search(item_path.back(), matches, pattern))
        {
            json_item->erase(stoi(matches[1].str()));
        }
        else
        {
            throwLineInfoException("Error: To access a list you must create strings of the style: 'item_{index number}'.");
        }
    }
    else
    {
        json_item->erase(item_path.back());
    }
}

vector<vector<vector<string>>> matchJsonItemPath(json &json_content, vector<string> &regex_item_path, unsigned int initial_index)
{
    vector<vector<vector<string>>> item_path_list;

    if(initial_index != regex_item_path.size())
    {
        if(json_content.is_array())
        {
            for(unsigned int index = 0; index < json_content.size(); index++)
            {
                smatch matches;

                string index_str = to_string(index);
                if(regex_search(index_str, matches, regex(regex_item_path[initial_index])))
                {
                    vector<string> key_groups(matches.begin(), matches.end());

                    vector<vector<vector<string>>> key_paths = matchJsonItemPath(json_content[index], regex_item_path, initial_index+1);

                    for(vector<vector<string>> &elem : key_paths)
                    {
                        vector<vector<string>> new_elem = {key_groups};
                        new_elem.insert(new_elem.end(), elem.begin(), elem.end());

                        elem = new_elem;
                    }

                    item_path_list.insert(item_path_list.end(), key_paths.begin(), key_paths.end());
                }
            }
        }
        else
        {
            for(const auto& [key, value] : json_content.items())
            {
                smatch matches;

                if(regex_search(key, matches, regex(regex_item_path[initial_index])))
                {
                    vector<string> key_groups(matches.begin(), matches.end());

                    vector<vector<vector<string>>> key_paths = matchJsonItemPath(json_content[key], regex_item_path, initial_index+1);

                    for(vector<vector<string>> &elem : key_paths)
                    {
                        vector<vector<string>> new_elem = {key_groups};
                        new_elem.insert(new_elem.end(), elem.begin(), elem.end());

                        elem = new_elem;
                    }

                    item_path_list.insert(item_path_list.end(), key_paths.begin(), key_paths.end());
                }
            }
        }
    }
    else
    {
        item_path_list.push_back({{json_content.dump()}});
    }

    if(initial_index == 0 && item_path_list.size() == 0)
    {
        string regex;
        for(size_t i = 0; i < regex_item_path.size(); ++i) {
            regex += regex_item_path[i];
            
            if (i < regex_item_path.size() - 1) {
                regex += ".";
            }
        }

        throwLineInfoException("Error: No matches found.");
    }

    return item_path_list;
}

vector<string> replaceJsonItemPathMatch(vector<string> &regex_item_path, vector<vector<string>> &valuesToReplace)
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



Change::Change()
{

}

Change::~Change()
{

}


AddFolderChange::AddFolderChange(vector<string> &&folder_path)
    : Change(), folder_path(folder_path)
{
    if(folder_path.size() == 0)
    {
        throwLineInfoException("Error: The folder cannot be the root folder of the configuration.");
    }
}

AddFolderChange::~AddFolderChange()
{

}

void AddFolderChange::applyChange(Config &config)
{
    Folder* root_folder = getFolder(config.getRootFolder(), folder_path, folder_path.size()-1);
    root_folder->getSubFolders()[folder_path.back()] = make_unique<Folder>();
}


AddJsonFileChange::AddJsonFileChange(vector<string> &&folder_path, string &&json_file)
    : Change(), folder_path(folder_path), json_file(json_file)
{
    if(json_file.size() == 0)
    {
        throwLineInfoException("Error: The name of the new json file must be specified.");
    }
}

AddJsonFileChange::~AddJsonFileChange()
{

}

void AddJsonFileChange::applyChange(Config &config)
{
    Folder* root_folder = getFolder(config.getRootFolder(), folder_path, folder_path.size());
    root_folder->getJsonFiles()[json_file] = JsonFile();
}


AddJsonItemChange::AddJsonItemChange(const vector<string> &folder_path, const vector<string> &item_path, const json &item_value, const string &json_file)
    : Change(), folder_path(folder_path), item_path(item_path), item_value(item_value), json_file(json_file)
{
    
}

AddJsonItemChange::AddJsonItemChange(vector<string> &&folder_path, vector<string> &&item_path, json &&item_value, const string &&json_file)
    : Change(), folder_path(folder_path), item_path(item_path), item_value(item_value), json_file(json_file)
{
    
}

AddJsonItemChange::~AddJsonItemChange()
{

}

void AddJsonItemChange::applyChange(Config &config)
{
    json* json_content = getJsonFileContent(config.getRootFolder(), folder_path, json_file);
    setJsonItem(*json_content, item_path, item_value);
}


AddJsonItemAllFilesChange::AddJsonItemAllFilesChange(vector<string> &&folder_path, vector<string> &&item_path, json &&item_value)
    : Change(), folder_path(folder_path), item_path(item_path), item_value(item_value)
{
    
}

AddJsonItemAllFilesChange::~AddJsonItemAllFilesChange()
{

}

void AddJsonItemAllFilesChange::applyChange(Config &config)
{
    Folder* folder = getFolder(config.getRootFolder(), folder_path, folder_path.size());

    for(auto& [fileName, file] : folder->getJsonFiles())
    {
        AddJsonItemChange file_change = AddJsonItemChange(folder_path, item_path, item_value, fileName);
        file_change.applyChange(config);
    }
}


RegexAddJsonItemChange::RegexAddJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &pattern_path, std::vector<std::string> &item_regex_path, nlohmann::json &item_value, const std::string &json_file)
    : Change(), folder_path(folder_path), pattern_path(pattern_path), item_regex_path(item_regex_path), item_value(item_value), json_file(json_file)
{
    
}

RegexAddJsonItemChange::RegexAddJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&pattern_path, std::vector<std::string> &&item_regex_path, nlohmann::json &&item_value, const std::string &&json_file)
    : Change(), folder_path(folder_path), pattern_path(pattern_path), item_regex_path(item_regex_path), item_value(item_value), json_file(json_file)
{
    
}

RegexAddJsonItemChange::~RegexAddJsonItemChange()
{

}

void RegexAddJsonItemChange::applyChange(Config &config)
{
    json* json_content = getJsonFileContent(config.getRootFolder(), folder_path, json_file);
    
    vector<vector<vector<string>>> matches = matchJsonItemPath(*json_content, pattern_path);
    
    for(vector<vector<string>>& match : matches)
    {
        vector<string> item_path = replaceJsonItemPathMatch(item_regex_path, match);
        
        AddJsonItemChange file_change = AddJsonItemChange(folder_path, item_path, item_value, json_file);
        file_change.applyChange(config);
    }
}


RegexAddJsonItemAllFilesChange::RegexAddJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&pattern_path, std::vector<std::string> &&item_regex_path, nlohmann::json &&item_value)
    : Change(), folder_path(folder_path), pattern_path(pattern_path), item_regex_path(item_regex_path), item_value(item_value)
{
    
}

RegexAddJsonItemAllFilesChange::~RegexAddJsonItemAllFilesChange()
{

}

void RegexAddJsonItemAllFilesChange::applyChange(Config &config)
{
    Folder* folder = getFolder(config.getRootFolder(), folder_path, folder_path.size());

    for(auto& [fileName, file] : folder->getJsonFiles())
    {
        RegexAddJsonItemChange file_change = RegexAddJsonItemChange(folder_path, pattern_path, item_regex_path, item_value, fileName);
        file_change.applyChange(config);
    }
}


MoveFolderChange::MoveFolderChange(std::vector<std::string> &&initial_folder_path, std::vector<std::string> &&final_folder_path)
    : Change(), initial_folder_path(initial_folder_path), final_folder_path(final_folder_path)
{
    if(initial_folder_path.size() == 0)
    {
        throwLineInfoException("Error: The initial folder cannot be the root folder of the configuration.");
    }

    if(final_folder_path.size() == 0)
    {
        throwLineInfoException("Error: The final folder cannot be the root folder of the configuration.");
    }
}

MoveFolderChange::~MoveFolderChange()
{

}

void MoveFolderChange::applyChange(Config &config)
{
    Folder* initial_root_folder = getFolder(config.getRootFolder(), initial_folder_path, initial_folder_path.size()-1);

    Folder* final_root_folder = getFolder(config.getRootFolder(), final_folder_path, final_folder_path.size()-1);

    final_root_folder->getSubFolders()[final_folder_path.back()] = move(initial_root_folder->getSubFolders()[initial_folder_path.back()]);
    initial_root_folder->getSubFolders().erase(initial_folder_path.back());
}


MoveJsonFileChange::MoveJsonFileChange(std::vector<std::string> &&initial_folder_path, std::vector<std::string> &&final_folder_path, std::string &&initial_json_file, std::string &&final_json_file)
    : Change(), initial_folder_path(initial_folder_path), final_folder_path(final_folder_path), initial_json_file(initial_json_file), final_json_file(final_json_file)
{
    if(initial_json_file.size() == 0)
    {
        throwLineInfoException("Error: The name of the initial json file must be specified.");
    }

    if(final_json_file.size() == 0)
    {
        throwLineInfoException("Error: The name of the final json file must be specified.");
    }
}

MoveJsonFileChange::~MoveJsonFileChange()
{

}

void MoveJsonFileChange::applyChange(Config &config)
{
    Folder* initial_folder = getFolder(config.getRootFolder(), initial_folder_path, initial_folder_path.size());
    Folder* final_folder = getFolder(config.getRootFolder(), final_folder_path, final_folder_path.size());

    final_folder->getJsonFiles()[final_json_file] = initial_folder->getJsonFiles()[initial_json_file];
    initial_folder->getJsonFiles().erase(initial_json_file);
}


MoveJsonItemChange::MoveJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_file)
    : Change(), folder_path(folder_path), initial_item_path(initial_item_path), final_item_path(final_item_path), json_file(json_file)
{
    if(json_file.size() == 0)
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
}

MoveJsonItemChange::MoveJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_file)
    : Change(), folder_path(folder_path), initial_item_path(initial_item_path), final_item_path(final_item_path), json_file(json_file)
{
    if(json_file.size() == 0)
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
}

MoveJsonItemChange::~MoveJsonItemChange()
{

}

void MoveJsonItemChange::applyChange(Config &config)
{
    json* json_content = getJsonFileContent(config.getRootFolder(), folder_path, json_file);

    json* initial_item = getJsonItem(*json_content, initial_item_path, initial_item_path.size());
    setJsonItem(*json_content, final_item_path, *initial_item);
    removeJsonItem(*json_content, initial_item_path);
}


MoveJsonItemAllFilesChange::MoveJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path)
    : Change(), folder_path(folder_path), initial_item_path(initial_item_path), final_item_path(final_item_path)
{
    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }
}

MoveJsonItemAllFilesChange::~MoveJsonItemAllFilesChange()
{

}

void MoveJsonItemAllFilesChange::applyChange(Config &config)
{
    Folder* folder = getFolder(config.getRootFolder(), folder_path, folder_path.size());

    for(auto& [fileName, file] : folder->getJsonFiles())
    {
        MoveJsonItemChange file_change = MoveJsonItemChange(folder_path, initial_item_path, final_item_path, fileName);
        file_change.applyChange(config);
    }
}


RegexMoveJsonItemChange::RegexMoveJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &item_regex_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_file)
    : Change(), folder_path(folder_path), item_regex_path(item_regex_path), initial_item_path(initial_item_path), final_item_path(final_item_path), json_file(json_file)
{
    if(json_file.size() == 0)
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
}

RegexMoveJsonItemChange::RegexMoveJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_file)
    : Change(), folder_path(folder_path), item_regex_path(item_regex_path), initial_item_path(initial_item_path), final_item_path(final_item_path), json_file(json_file)
{
    if(json_file.size() == 0)
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
}

RegexMoveJsonItemChange::~RegexMoveJsonItemChange()
{

}

void RegexMoveJsonItemChange::applyChange(Config &config)
{
    json* json_content = getJsonFileContent(config.getRootFolder(), folder_path, json_file);

    vector<vector<vector<string>>> matches = matchJsonItemPath(*json_content, item_regex_path);
    
    for(vector<vector<string>>& match : matches)
    {
        vector<string> match_initial_item_path = replaceJsonItemPathMatch(initial_item_path, match);
        vector<string> match_final_item_path = replaceJsonItemPathMatch(final_item_path, match);
        
        MoveJsonItemChange file_change = MoveJsonItemChange(folder_path, match_initial_item_path, match_final_item_path, json_file);
        file_change.applyChange(config);
    }
}


RegexMoveJsonItemAllFilesChange::RegexMoveJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path)
    : Change(), folder_path(folder_path), item_regex_path(item_regex_path), initial_item_path(initial_item_path), final_item_path(final_item_path)
{
    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }
}

RegexMoveJsonItemAllFilesChange::~RegexMoveJsonItemAllFilesChange()
{

}

void RegexMoveJsonItemAllFilesChange::applyChange(Config &config)
{
    Folder* folder = getFolder(config.getRootFolder(), folder_path, folder_path.size());

    for(auto& [fileName, file] : folder->getJsonFiles())
    {
        RegexMoveJsonItemChange file_change = RegexMoveJsonItemChange(folder_path, item_regex_path, initial_item_path, final_item_path, fileName);
        file_change.applyChange(config);
    }
}


CopyJsonItemChange::CopyJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_file)
    : Change(), folder_path(folder_path), initial_item_path(initial_item_path), final_item_path(final_item_path), json_file(json_file)
{
    if(json_file.size() == 0)
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
}

CopyJsonItemChange::CopyJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_file)
    : Change(), folder_path(folder_path), initial_item_path(initial_item_path), final_item_path(final_item_path), json_file(json_file)
{
    if(json_file.size() == 0)
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
}

CopyJsonItemChange::~CopyJsonItemChange()
{

}

void CopyJsonItemChange::applyChange(Config &config)
{
    json* json_content = getJsonFileContent(config.getRootFolder(), folder_path, json_file);

    json* initial_item = getJsonItem(*json_content, initial_item_path, initial_item_path.size());
    setJsonItem(*json_content, final_item_path, *initial_item);
}


CopyJsonItemAllFilesChange::CopyJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path)
    : Change(), folder_path(folder_path), initial_item_path(initial_item_path), final_item_path(final_item_path)
{
    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }
}

CopyJsonItemAllFilesChange::~CopyJsonItemAllFilesChange()
{

}

void CopyJsonItemAllFilesChange::applyChange(Config &config)
{
    Folder* folder = getFolder(config.getRootFolder(), folder_path, folder_path.size());

    for(auto& [fileName, file] : folder->getJsonFiles())
    {
        CopyJsonItemChange file_change = CopyJsonItemChange(folder_path, initial_item_path, final_item_path, fileName);
        file_change.applyChange(config);
    }
}


RegexCopyJsonItemChange::RegexCopyJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &item_regex_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_file)
    : Change(), folder_path(folder_path), item_regex_path(item_regex_path), initial_item_path(initial_item_path), final_item_path(final_item_path), json_file(json_file)
{
    if(json_file.size() == 0)
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
}

RegexCopyJsonItemChange::RegexCopyJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_file)
    : Change(), folder_path(folder_path), item_regex_path(item_regex_path), initial_item_path(initial_item_path), final_item_path(final_item_path), json_file(json_file)
{
    if(json_file.size() == 0)
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
}

RegexCopyJsonItemChange::~RegexCopyJsonItemChange()
{

}

void RegexCopyJsonItemChange::applyChange(Config &config)
{
    json* json_content = getJsonFileContent(config.getRootFolder(), folder_path, json_file);

    vector<vector<vector<string>>> matches = matchJsonItemPath(*json_content, item_regex_path);
    
    for(vector<vector<string>>& match : matches)
    {
        vector<string> match_initial_item_path = replaceJsonItemPathMatch(initial_item_path, match);
        vector<string> match_final_item_path = replaceJsonItemPathMatch(final_item_path, match);
        
        CopyJsonItemChange file_change = CopyJsonItemChange(folder_path, match_initial_item_path, match_final_item_path, json_file);
        file_change.applyChange(config);
    }
}


RegexCopyJsonItemAllFilesChange::RegexCopyJsonItemAllFilesChange(std::vector<std::string> &&folder_path, vector<string>&& item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path)
    : Change(), folder_path(folder_path), item_regex_path(item_regex_path), initial_item_path(initial_item_path), final_item_path(final_item_path)
{
    if(initial_item_path.size() == 0)
    {
        throwLineInfoException("Error: The initial item cannot be the root of the json content.");
    }

    if(final_item_path.size() == 0)
    {
        throwLineInfoException("Error: The final item cannot be the root of the json content.");
    }
}

RegexCopyJsonItemAllFilesChange::~RegexCopyJsonItemAllFilesChange()
{

}

void RegexCopyJsonItemAllFilesChange::applyChange(Config &config)
{
    Folder* folder = getFolder(config.getRootFolder(), folder_path, folder_path.size());

    for(auto& [fileName, file] : folder->getJsonFiles())
    {
        RegexCopyJsonItemChange file_change = RegexCopyJsonItemChange(folder_path, item_regex_path, initial_item_path, final_item_path, fileName);
        file_change.applyChange(config);
    }
}


RemoveFolderChange::RemoveFolderChange(std::vector<std::string> &&folder_path)
    : Change(), folder_path(folder_path)
{
    if(folder_path.size() == 0)
    {
        throwLineInfoException("Error: The folder cannot be the root folder of the configuration.");
    }
}

RemoveFolderChange::~RemoveFolderChange()
{

}

void RemoveFolderChange::applyChange(Config &config)
{
    Folder* root_folder = getFolder(config.getRootFolder(), folder_path, folder_path.size()-1);
    root_folder->getSubFolders().erase(folder_path.back());
}


RemoveJsonFileChange::RemoveJsonFileChange(std::vector<std::string> &&folder_path, const std::string &&json_file)
    : Change(), folder_path(folder_path), json_file(json_file)
{
    if(json_file.size() == 0)
    {
        throwLineInfoException("Error: The name of the json file must be specified.");
    }
}

RemoveJsonFileChange::~RemoveJsonFileChange()
{

}

void RemoveJsonFileChange::applyChange(Config &config)
{
    Folder* folder = getFolder(config.getRootFolder(), folder_path, folder_path.size());
    folder->getJsonFiles().erase(json_file);
}


RemoveJsonItemChange::RemoveJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &item_path, const std::string &json_file)
    : Change(), folder_path(folder_path), item_path(item_path), json_file(json_file)
{
    if(json_file.size() == 0)
    {
        throwLineInfoException("Error: The name of the json file must be specified.");
    }

    if(item_path.size() == 0)
    {
        throwLineInfoException("Error: The item cannot be the root of the json content.");
    }
}

RemoveJsonItemChange::RemoveJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path, const std::string &&json_file)
    : Change(), folder_path(folder_path), item_path(item_path), json_file(json_file)
{
    if(json_file.size() == 0)
    {
        throwLineInfoException("Error: The name of the json file must be specified.");
    }

    if(item_path.size() == 0)
    {
        throwLineInfoException("Error: The item cannot be the root of the json content.");
    }
}

RemoveJsonItemChange::~RemoveJsonItemChange()
{

}

void RemoveJsonItemChange::applyChange(Config &config)
{
    json* json_content = getJsonFileContent(config.getRootFolder(), folder_path, json_file);
    removeJsonItem(*json_content, item_path);
}


RemoveJsonItemAllFilesChange::RemoveJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path)
    : Change(), folder_path(folder_path), item_path(item_path)
{
    if(item_path.size() == 0)
    {
        throwLineInfoException("Error: The item cannot be the root of the json content.");
    }
}

RemoveJsonItemAllFilesChange::~RemoveJsonItemAllFilesChange()
{

}

void RemoveJsonItemAllFilesChange::applyChange(Config &config)
{
    Folder* folder = getFolder(config.getRootFolder(), folder_path, folder_path.size());

    for(auto& [fileName, file] : folder->getJsonFiles())
    {
        RemoveJsonItemChange file_change = RemoveJsonItemChange(folder_path, item_path, fileName);
        file_change.applyChange(config);
    }
}
