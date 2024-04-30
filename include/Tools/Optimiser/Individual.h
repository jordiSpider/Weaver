#ifndef INDIVIDUAL_H_
#define INDIVIDUAL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <oneapi/tbb/parallel_for.h>
#include <oneapi/tbb/task_group.h>

#include "Tools/Config/Config.h"
#include "Tools/Optimiser/Parameter.h"
#include "Tools/Updater/Change.h"
#include "Misc/Utilities.h"
#include "Misc/Types.h"
#include "Tools/Optimiser/EvaluationFunction.h"
#include "Tools/Optimiser/MutationOperator.h"
#include "Misc/GlobalVariable.h"



class Individual
{
private:
    std::vector<nlohmann::json> parametersValue;
    double fitness;
    std::string configPath;
    std::string resultPath;
    nlohmann::json resultInfo;

public:
    Individual(const bool maximiseFitness);
    Individual(const std::vector<nlohmann::json>& parametersValue, const bool maximiseFitness);
    virtual ~Individual();

    std::filesystem::path createConfig(std::filesystem::path baseConfigPath, std::filesystem::path output_folder, const std::vector<Parameter>& parametersInfo, unsigned int index);
    void setInfo(std::filesystem::path newConfigPath, std::filesystem::path newResultFolderPath, EvaluationFunctionType evaluationFunction);
    bool isBetterThan(const Individual& other, const bool maximiseFitness) const;
    void print(const std::string&& indentation, std::vector<Parameter>& parameters) const;
    void mutate(const std::vector<Parameter>& parameters, MutationOperatorType mutationOperator);

    const double& getFitness() const;
    const std::string& getConfigPath() const;
    const std::string& getResultPath() const;
    const nlohmann::json& getResultInfo() const;
    const std::vector<nlohmann::json>& getParametersValue() const;
    const nlohmann::json getInfo(const std::vector<Parameter>& parameters) const;

    bool operator<(const Individual& other) const;
    bool operator>(const Individual& other) const;
};

#endif /* INDIVIDUAL_H_ */