
#include "Tools/Config/Folder.h"


using namespace std;
namespace fs = boost::filesystem;


Folder::Folder()
{
    
}

Folder::Folder(fs::path folder_path)
{
    for(const auto& elem : fs::directory_iterator(folder_path))
    {
        if(fs::is_directory(elem))
        {
            subfolders[elem.path().filename().string()] = make_unique<Folder>(elem.path());
        }
        else
        {
            if(elem.path().extension() == ".json")
            {
                json_files[elem.path().filename().string()] = JsonFile(elem);
            }
        }
    }

    bool initialised = false;
    version = VersionNumber();
    inconsistentVersion = false;

    
    for(auto& [folderName, folder] : getSubFolders())
    {
        if(folder->size() > 0)
        {
            if(!initialised)
            {
                initialised = true;
                version = folder->getVersion();
            }
            
            if(getVersion() != folder->getVersion())
            {
                version = VersionNumber();
            }
        }
    }

    for(auto& [fileName, file] : getJsonFiles())
    {
        if(!initialised)
        {
            initialised = true;
            version = file.getVersion();
        }

        if(getVersion() != file.getVersion())
        {
            version = VersionNumber();
        }
    }
}

Folder::~Folder()
{
    
}

unsigned int Folder::size()
{
    unsigned int numberOfElements = 0;

    for(auto& [folderName, folder] : getSubFolders())
    {
        numberOfElements += folder->size();
    }

    numberOfElements += getJsonFiles().size();

    return numberOfElements;
}

void Folder::save(fs::path new_root_folder_path)
{
    if(getSubFolders().size() + getJsonFiles().size() > 0)
    {
        for(auto& [folderName, folder] : getSubFolders())
        {
            fs::path folder_path = new_root_folder_path / folderName;
            fs::create_directory(folder_path);
            folder->save(folder_path);
        }

        for(auto& [fileName, file] : getJsonFiles())
        {
            fs::path file_path = new_root_folder_path / fileName;
            file.save(file_path);
        }
    }
    else
    {
        fs::path gitkeep_path = new_root_folder_path / ".gitkeep";
        ofstream gitkeep_file(gitkeep_path);
        gitkeep_file.close();
    }
}

void Folder::refresh_version(VersionNumber new_version)
{
    version = new_version;

    for(auto& [folderName, folder] : getSubFolders())
    {
        folder->refresh_version(new_version);
    }

    for(auto& [fileName, file] : getJsonFiles())
    {
        file.refresh_version(new_version);
    }
}

unordered_map<string, unique_ptr<Folder>>& Folder::getSubFolders()
{
    return subfolders;
}

unique_ptr<Folder>& Folder::getSubFolder(const string& foldername)
{
    return getSubFolders()[foldername];
}

unordered_map<string, JsonFile>& Folder::getJsonFiles()
{
    return json_files;
}

JsonFile& Folder::getFile(const string& filename)
{
    return getJsonFiles()[filename];
}

VersionNumber& Folder::getVersion()
{
    return version;
}


void Folder::setFolder(const string& foldername, unique_ptr<Folder>& newFolder)
{
    getSubFolders()[foldername] = move(newFolder);
}

void Folder::removeFolder(const string& foldername)
{
    getSubFolders().erase(foldername);
}


void Folder::addFile(string &json_file)
{
    getJsonFiles()[json_file] = JsonFile();
}

void Folder::setFile(const string& filename, JsonFile& newFile)
{
    getJsonFiles()[filename] = newFile;
}

void Folder::removeFile(const string& filename)
{
    getJsonFiles().erase(filename);
}
