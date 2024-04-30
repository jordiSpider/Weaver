#ifndef OPTIMISER_H_
#define OPTIMISER_H_


#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <oneapi/tbb/global_control.h>
#include <oneapi/tbb/task_arena.h>
#include <oneapi/tbb/parallel_for.h>
#include <oneapi/tbb/concurrent_vector.h>
#include <oneapi/tbb/task_group.h>

#include "Exceptions/LineInfoException.h"
#include "Tools/Optimiser/Individual.h"
#include "Tools/Optimiser/Parameter.h"
#include "Tools/Optimiser/Population.h"
#include "Tools/Optimiser/EvaluationFunction.h"
#include "Tools/Optimiser/SelectionOperator.h"
#include "Tools/Optimiser/CrossoverOperator.h"
#include "Tools/Optimiser/MutationOperator.h"
#include "Tools/Optimiser/ReplacementOperator.h"
#include "IBM/Maths/Random.h"
#include "Misc/GlobalVariable.h"



// boost::filesystem::path createGenerationFolder(boost::filesystem::path outputFolder, unsigned int generationNumber);

// void runWeaverOptimiser(boost::filesystem::path baseConfigPath, boost::filesystem::path outputFolder,
//     unsigned int numCores, unsigned int maxNumberOfGenerations
// );



class Optimiser
{
private:
    static boost::filesystem::path initializeLog(boost::filesystem::path outputFolder);
    static boost::filesystem::path createGenerationFolder(boost::filesystem::path outputFolder, unsigned int generationNumber);
    static void printBestIndividual(const Individual& bestIndividual, std::vector<Parameter>& parameters);
    static bool achieveObjectiveFitness(const double& fitness, const double& fitnessObjective, bool maximiseFitness);
    static std::unique_ptr<std::vector<Individual*>> doSelectionOperator(SelectionOperatorType selectionOperator, const Population& population, const bool maximiseFitness);
    static std::unique_ptr<oneapi::tbb::concurrent_vector<std::unique_ptr<Individual>>> doCrossoverOperator(CrossoverOperatorType crossoverOperator, std::unique_ptr<std::vector<Individual*>>& parents, const bool maximiseFitness);
    static void doMutationOperator(const std::vector<Parameter>& parameters, MutationOperatorType mutationOperator, std::unique_ptr<oneapi::tbb::concurrent_vector<std::unique_ptr<Individual>>>& descendants);
    static std::unique_ptr<std::vector<Individual*>> doReplacementOperator(ReplacementOperatorType replacementOperator, Population& currentPopulation, oneapi::tbb::concurrent_vector<std::unique_ptr<Individual>>& descendants);
    static void copyDirectory(const boost::filesystem::path& source, const boost::filesystem::path& destination);
    static void saveBestIndividual(boost::filesystem::path logFilePath, const Individual& bestIndividual, const std::vector<Parameter>& parameters, boost::filesystem::path outputFolder);

public:
    static void runOptimiser(const nlohmann::json& run_params, const bool overwrite);
    static void runOptimiser(boost::filesystem::path baseConfigPath, boost::filesystem::path outputFolder, unsigned int maxNumberOfGenerations, unsigned int numberOfCores, std::vector<Parameter>& parameters, unsigned int burn_in, bool maximiseFitness, std::string evaluationFunctionSelected, std::string selectionOperatorSelected, std::string crossoverOperatorSelected, std::string mutationOperatorSelected, std::string replacementOperatorSelected, const double& fitnessObjective, const bool overwrite);

    // static void initOptimiser();
    // template<typename T>
    // static void addParameterFixedValue(std::string& name, std::vector<T>& values, std::vector<T>& valueRange);

    // Optimiser();
    // virtual ~Optimiser();

    
    // void addParameterRandomValue();
    // void runWeaverOptimization();
};




#endif /* OPTIMISER_H_ */