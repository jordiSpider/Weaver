
#include "Tools/Config/JsonFile.h"


using namespace std;
namespace fs = boost::filesystem;
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


json* JsonFile::getItem(const vector<string> &item_path, const unsigned int finalIndex)
{
    json* json_item = &getContent();

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

void JsonFile::setItem(const vector<string> &item_path, const json &value)
{
    json* json_item = getItem(item_path, item_path.size()-1);

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

void JsonFile::moveItem(vector<string> &initial_item_path, vector<string> &final_item_path)
{
    json* initial_item = getItem(initial_item_path, initial_item_path.size());
    setItem(final_item_path, *initial_item);
    removeItem(initial_item_path);
}

void JsonFile::copyItem(vector<string> &initial_item_path, vector<string> &final_item_path)
{
    json* initial_item = getItem(initial_item_path, initial_item_path.size());
    setItem(final_item_path, *initial_item);
}

void JsonFile::removeItem(std::vector<std::string> &item_path)
{
    json* json_item = getItem(item_path, item_path.size()-1);

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


vector<vector<vector<string>>> JsonFile::matchItemPath(json &json_content, vector<string> &regex_item_path, unsigned int initial_index)
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

                    vector<vector<vector<string>>> key_paths = matchItemPath(json_content[index], regex_item_path, initial_index+1);

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

                    vector<vector<vector<string>>> key_paths = matchItemPath(json_content[key], regex_item_path, initial_index+1);

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

vector<vector<vector<string>>> JsonFile::matchItemPath(vector<string> &regex_item_path, unsigned int initial_index)
{
    return matchItemPath(getContent(), regex_item_path, initial_index);
}
