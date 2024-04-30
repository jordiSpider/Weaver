
#include "Tools/Optimiser/Parameter.h"


using namespace std;
using json = nlohmann::json;



Parameter::Parameter(const json& parameterInfo)
    : name(parameterInfo["name"]), folderPath(obtainFolderPath(parameterInfo["name"])), filename(obtainFilename(parameterInfo["name"])), jsonItemPath(obtainJsonItemPath(parameterInfo["name"])),
      range(ParameterRangeFactory::createInstance(parameterInfo["range"]).release())
{
    try {
        initialValues = parameterInfo.at("initialValues");
    } catch (const json::out_of_range& e) {
        initialValues.reserve(parameterInfo["numberOfInitialValues"]);
        initialValues = *range->generateRandomValue(parameterInfo["numberOfInitialValues"], {});
    }
}

Parameter::Parameter(const Parameter& other)
    : name(other.getName()), folderPath(other.getFolderPath()), filename(other.getFilename()), jsonItemPath(other.getJsonItemPath()),
      initialValues(other.getInitialValues()), range(other.range->clone())
{

}

Parameter::~Parameter()
{
    delete range;
}


vector<string> Parameter::obtainFolderPath(const string& name)
{
    vector<string> folderPath;

    string filePathStr = name.substr(0, name.find(":"));

    const char separator = '/';
    int separatorPos = filePathStr.find(separator); 
    while(separatorPos != -1) 
    {
        folderPath.push_back(filePathStr.substr(0, separatorPos));
        filePathStr.erase(filePathStr.begin(), filePathStr.begin() + separatorPos + 1);
        separatorPos = filePathStr.find(separator);
    }

    return folderPath;
}

string Parameter::obtainFilename(const string& name)
{
    string filePathStr = name.substr(0, name.find(":"));

    const char separator = '/';
    int separatorPos = filePathStr.find(separator); 
    while(separatorPos != -1) 
    {
        filePathStr.erase(filePathStr.begin(), filePathStr.begin() + separatorPos + 1);
        separatorPos = filePathStr.find(separator);
    }
    
    return filePathStr;
}

vector<string> Parameter::obtainJsonItemPath(const string& name)
{
    vector<string> jsonItemPath;

    string jsonItemPathStr = name.substr(name.find(":") + 1, name.size());

    const char separator = '/';
    int separatorPos = jsonItemPathStr.find(separator); 
    while(separatorPos != -1) 
    {
        jsonItemPath.push_back(jsonItemPathStr.substr(0, separatorPos));
        jsonItemPathStr.erase(jsonItemPathStr.begin(), jsonItemPathStr.begin() + separatorPos + 1);
        separatorPos = jsonItemPathStr.find(separator);
    }
    jsonItemPath.push_back(jsonItemPathStr);

    return jsonItemPath;
}

json Parameter::generateRandomValue(const json& currentParameterValue) const
{
    return range->generateRandomValue(1, {currentParameterValue})->at(0);
}

const string& Parameter::getName() const
{
    return name;
}

const vector<string>& Parameter::getFolderPath() const
{
    return folderPath;
}

const std::string& Parameter::getFilename() const
{
    return filename;
}

const vector<string>& Parameter::getJsonItemPath() const
{
    return jsonItemPath;
}

const vector<json>& Parameter::getInitialValues() const
{
    return initialValues;
}
