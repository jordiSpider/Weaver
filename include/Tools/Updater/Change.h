#ifndef CHANGE_H_
#define CHANGE_H_


#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <regex>
#include <memory>

#include "Tools/Config/Config.h"
#include "Tools/Config/Folder.h"
#include "Exceptions/LineInfoException.h"



Folder* getFolder(Folder& root_folder, std::vector<std::string> &folder_path, const unsigned int finalIndex);
nlohmann::json* getJsonFileContent(Folder& root_folder, std::vector<std::string> &folder_path, std::string json_file);
nlohmann::json* getJsonItem(nlohmann::json &json_content, std::vector<std::string> &item_path, const unsigned int finalIndex);
void setJsonItem(nlohmann::json &json_content, std::vector<std::string> &item_path, nlohmann::json &value);
void removeJsonItem(nlohmann::json &json_content, std::vector<std::string> &item_path);
std::vector<std::vector<std::vector<std::string>>> matchJsonItemPath(nlohmann::json &json_content, std::vector<std::string> &regex_item_path, unsigned int initial_index = 0);
std::vector<std::string> replaceJsonItemPathMatch(std::vector<std::string> &regex_item_path, std::vector<std::vector<std::string>> &valuesToReplace);


class Change
{
public:
    Change();
    virtual ~Change();

    virtual void applyChange(Config &config)=0;
};


class AddFolderChange : public Change
{
private:
    std::vector<std::string> folder_path;

public:
    AddFolderChange(std::vector<std::string> &&folder_path);
    virtual ~AddFolderChange();

    void applyChange(Config &config);
};


class AddJsonFileChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::string json_file;

public:
    AddJsonFileChange(std::vector<std::string> &&folder_path, std::string &&json_file);
    virtual ~AddJsonFileChange();

    void applyChange(Config &config);
};


class AddJsonItemChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> item_path;
    nlohmann::json item_value;
    std::string json_file;

public:
    AddJsonItemChange(const std::vector<std::string> &folder_path, const std::vector<std::string> &item_path, const nlohmann::json &item_value, const std::string &json_file);
    AddJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path, nlohmann::json &&item_value, const std::string &&json_file);
    virtual ~AddJsonItemChange();

    void applyChange(Config &config);
};


class AddJsonItemAllFilesChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> item_path;
    nlohmann::json item_value;

public:
    AddJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path, nlohmann::json &&item_value);
    virtual ~AddJsonItemAllFilesChange();

    void applyChange(Config &config);
};


class RegexAddJsonItemChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> pattern_path;
    std::vector<std::string> item_regex_path;
    nlohmann::json item_value;
    std::string json_file;

public:
    RegexAddJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &pattern_path, std::vector<std::string> &item_regex_path, nlohmann::json &item_value, const std::string &json_file);
    RegexAddJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&pattern_path, std::vector<std::string> &&item_regex_path, nlohmann::json &&item_value, const std::string &&json_file);
    virtual ~RegexAddJsonItemChange();

    void applyChange(Config &config);
};


class RegexAddJsonItemAllFilesChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> pattern_path;
    std::vector<std::string> item_regex_path;
    nlohmann::json item_value;

public:
    RegexAddJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&pattern_path, std::vector<std::string> &&item_regex_path, nlohmann::json &&item_value);
    virtual ~RegexAddJsonItemAllFilesChange();

    void applyChange(Config &config);
};


class MoveFolderChange : public Change
{
private:
    std::vector<std::string> initial_folder_path;
    std::vector<std::string> final_folder_path;

public:
    MoveFolderChange(std::vector<std::string> &&initial_folder_path, std::vector<std::string> &&final_folder_path);
    virtual ~MoveFolderChange();

    void applyChange(Config &config);
};


class MoveJsonFileChange : public Change
{
private:
    std::vector<std::string> initial_folder_path;
    std::vector<std::string> final_folder_path;
    std::string initial_json_file;
    std::string final_json_file;

public:
    MoveJsonFileChange(std::vector<std::string> &&initial_folder_path, std::vector<std::string> &&final_folder_path, std::string &&initial_json_file, std::string &&final_json_file);
    virtual ~MoveJsonFileChange();

    void applyChange(Config &config);
};


class MoveJsonItemChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> initial_item_path;
    std::vector<std::string> final_item_path;
    std::string json_file;

public:
    MoveJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_file);
    MoveJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_file);
    virtual ~MoveJsonItemChange();

    void applyChange(Config &config);
};


class MoveJsonItemAllFilesChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> initial_item_path;
    std::vector<std::string> final_item_path;

public:
    MoveJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path);
    virtual ~MoveJsonItemAllFilesChange();

    void applyChange(Config &config);
};


class RegexMoveJsonItemChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> item_regex_path;
    std::vector<std::string> initial_item_path;
    std::vector<std::string> final_item_path;
    std::string json_file;

public:
    RegexMoveJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &item_regex_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_file);
    RegexMoveJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_file);
    virtual ~RegexMoveJsonItemChange();

    void applyChange(Config &config);
};


class RegexMoveJsonItemAllFilesChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> item_regex_path;
    std::vector<std::string> initial_item_path;
    std::vector<std::string> final_item_path;

public:
    RegexMoveJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path);
    virtual ~RegexMoveJsonItemAllFilesChange();

    void applyChange(Config &config);
};


class CopyJsonItemChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> initial_item_path;
    std::vector<std::string> final_item_path;
    std::string json_file;

public:
    CopyJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_file);
    CopyJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_file);
    virtual ~CopyJsonItemChange();

    void applyChange(Config &config);
};


class CopyJsonItemAllFilesChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> initial_item_path;
    std::vector<std::string> final_item_path;

public:
    CopyJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path);
    virtual ~CopyJsonItemAllFilesChange();

    void applyChange(Config &config);
};


class RegexCopyJsonItemChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> item_regex_path;
    std::vector<std::string> initial_item_path;
    std::vector<std::string> final_item_path;
    std::string json_file;

public:
    RegexCopyJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &item_regex_path, std::vector<std::string> &initial_item_path, std::vector<std::string> &final_item_path, const std::string &json_file);
    RegexCopyJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path, const std::string &&json_file);
    virtual ~RegexCopyJsonItemChange();

    void applyChange(Config &config);
};


class RegexCopyJsonItemAllFilesChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> item_regex_path;
    std::vector<std::string> initial_item_path;
    std::vector<std::string> final_item_path;

public:
    RegexCopyJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string>&& item_regex_path, std::vector<std::string> &&initial_item_path, std::vector<std::string> &&final_item_path);
    virtual ~RegexCopyJsonItemAllFilesChange();

    void applyChange(Config &config);
};


class RemoveFolderChange : public Change
{
private:
    std::vector<std::string> folder_path;

public:
    RemoveFolderChange(std::vector<std::string> &&folder_path);
    virtual ~RemoveFolderChange();

    void applyChange(Config &config);
};


class RemoveJsonFileChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::string json_file;

public:
    RemoveJsonFileChange(std::vector<std::string> &&folder_path, const std::string &&json_file);
    virtual ~RemoveJsonFileChange();

    void applyChange(Config &config);
};


class RemoveJsonItemChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> item_path;
    std::string json_file;

public:
    RemoveJsonItemChange(std::vector<std::string> &folder_path, std::vector<std::string> &item_path, const std::string &json_file);
    RemoveJsonItemChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path, const std::string &&json_file);
    virtual ~RemoveJsonItemChange();

    void applyChange(Config &config);
};


class RemoveJsonItemAllFilesChange : public Change
{
private:
    std::vector<std::string> folder_path;
    std::vector<std::string> item_path;

public:
    RemoveJsonItemAllFilesChange(std::vector<std::string> &&folder_path, std::vector<std::string> &&item_path);
    virtual ~RemoveJsonItemAllFilesChange();

    void applyChange(Config &config);
};


#endif /* CHANGE_H_ */