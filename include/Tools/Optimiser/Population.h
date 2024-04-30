#ifndef POPULATION_H_
#define POPULATION_H_


#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <oneapi/tbb/global_control.h>
#include <oneapi/tbb/parallel_for.h>
#include <oneapi/tbb/task_group.h>
#include <cstdlib>

#include "Tools/Optimiser/Individual.h"
#include "Tools/Optimiser/Parameter.h"
#include "Tools/Optimiser/EvaluationFunction.h"
#include "Misc/GlobalVariable.h"



class Population
{
private:
    static void combineRecursive(const std::vector<std::vector<nlohmann::json>>& input, std::vector<std::vector<nlohmann::json>>& output, unsigned int currentIndex, std::vector<nlohmann::json> currentCombination);
    static std::unique_ptr<std::vector<std::vector<nlohmann::json>>> combine(const std::vector<std::vector<nlohmann::json>>& input);

    std::string name;
    std::vector<Individual*> individuals;

public:
    Population(const std::vector<Parameter>& parameters, const unsigned int generationNumber, const bool maximiseFitness);
    Population(std::vector<Individual*> individuals, const unsigned int generationNumber);
    virtual ~Population();

    Individual evaluate(const std::vector<Parameter>& parameters, unsigned int numberOfCores, std::filesystem::path baseConfigPath, std::filesystem::path generationOutputFolder, std::filesystem::path logFilePath, EvaluationFunctionType evaluationFunction, unsigned int burn_in, const Individual& bestIndividual, const bool maximiseFitness);
    void updateLog(const std::filesystem::path& logFilePath, std::vector<Parameter>& parameters) const;

    const std::vector<Individual*>& getIndividuals() const;
    std::vector<Individual*>& getMutableIndividuals();

    void evaluateIndividual(std::filesystem::path& configPath, std::filesystem::path& resultFolder, unsigned int& burn_in);
};

#endif /* POPULATION_H_ */