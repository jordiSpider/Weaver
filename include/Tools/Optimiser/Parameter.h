#ifndef PARAMETER_H_
#define PARAMETER_H_


#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

#include "Tools/Optimiser/ParameterRange.h"
#include "Tools/Optimiser/ParameterRangeFactory.h"


class Parameter
{
private:
    const std::string name;
    const std::vector<std::string> folderPath;
    const std::string filename;
    const std::vector<std::string> jsonItemPath;
    std::vector<nlohmann::json> initialValues;
    const ParameterRange* const range;


    std::vector<std::string> obtainFolderPath(const std::string& name);
    std::string obtainFilename(const std::string& name);
    std::vector<std::string> obtainJsonItemPath(const std::string& name);

public:
    Parameter(const nlohmann::json& parameterInfo);

    Parameter(const Parameter& other);
    
    virtual ~Parameter();

    nlohmann::json generateRandomValue(const nlohmann::json& currentParameterValue) const;

    const std::string& getName() const;
    const std::vector<std::string>& getFolderPath() const;
    const std::string& getFilename() const;
    const std::vector<std::string>& getJsonItemPath() const;
    const std::vector<nlohmann::json>& getInitialValues() const;
};

#endif /* PARAMETER_H_ */