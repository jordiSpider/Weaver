
#include "Tools/Optimiser/Optimiser.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;


fs::path Optimiser::initializeLog(fs::path outputFolder)
{
    cerr << TAB << "- Initialising log ... ";

    fs::path logFilePath = outputFolder / "optimization_log.json";
    ofstream logFile(logFilePath);
    json logContent;

    if(logFile.is_open()) 
	{
        logFile << logContent.dump(4) << endl;
		logFile.close();
    }

    cerr << "[DONE]" << endl << endl;

    return logFilePath;
}

fs::path Optimiser::createGenerationFolder(fs::path outputFolder, unsigned int generationNumber)
{
    cerr << TAB << "- Creating generation folder ... ";

    fs::path generationOutputFolder = outputFolder / fs::path("generation_" + to_string(generationNumber));
    fs::create_directories(generationOutputFolder);

    cerr << "[DONE]" << endl;

    return generationOutputFolder;
}

void Optimiser::printBestIndividual(const Individual& bestIndividual, vector<Parameter>& parameters)
{
    cerr << endl;
    cerr << TAB << "- Best individual:" << endl;
    bestIndividual.print(string(TAB) + TAB, parameters);
    cerr << endl;
}

bool Optimiser::achieveObjectiveFitness(const double& fitness, const double& fitnessObjective, bool maximiseFitness)
{
    return (maximiseFitness) ? fitness > fitnessObjective : fitness < fitnessObjective;
}

unique_ptr<vector<Individual*>> Optimiser::doSelectionOperator(SelectionOperatorType selectionOperator, const Population& population, const bool maximiseFitness)
{
    cerr << TAB << "- Selecting parents ... ";

    unique_ptr<vector<Individual*>> parents = selectionOperator(population, maximiseFitness);

    cerr << "[DONE]" << endl;

    return parents;
}

unique_ptr<oneapi::tbb::concurrent_vector<unique_ptr<Individual>>> Optimiser::doCrossoverOperator(CrossoverOperatorType crossoverOperator, unique_ptr<vector<Individual*>>& parents, const bool maximiseFitness)
{
    if (parents->size() % 2 != 0) {
        throwLineInfoException("Error: The number of parents must be even.");
    }

    cerr << TAB << "- Crossing parents ... ";

    unique_ptr<oneapi::tbb::concurrent_vector<unique_ptr<Individual>>> descendants = make_unique<oneapi::tbb::concurrent_vector<unique_ptr<Individual>>>();

    // oneapi::tbb::global_control global_limit(oneapi::tbb::global_control::max_allowed_parallelism, oneapi::tbb::this_task_arena::max_concurrency());
    
    // oneapi::tbb::parallel_for(oneapi::tbb::blocked_range<unsigned int>(0, static_cast<unsigned int>(parents->size()-1)),
    //     [&crossoverOperator, &parents, &descendants, &maximiseFitness](const oneapi::tbb::blocked_range<unsigned int>& r) {
    //         for(unsigned int i = r.begin(); i < r.end(); i += 2) {
    //             crossoverOperator(*parents->at(i), *parents->at(i + 1), *descendants, maximiseFitness);
    //         }
    //     }
    // );

    for(unsigned int i = 0; i < parents->size(); i += 2)
    {
        crossoverOperator(*parents->at(i), *parents->at(i + 1), *descendants, maximiseFitness);
    }

    cerr << "[DONE]" << endl;

    return descendants;
}

void Optimiser::doMutationOperator(const vector<Parameter>& parameters, MutationOperatorType mutationOperator, unique_ptr<oneapi::tbb::concurrent_vector<unique_ptr<Individual>>>& descendants)
{
    cerr << TAB << "- Mutating descendants ... ";

    // oneapi::tbb::global_control global_limit(oneapi::tbb::global_control::max_allowed_parallelism, oneapi::tbb::this_task_arena::max_concurrency());

    // oneapi::tbb::task_group tg;

    // for(unsigned int index = 0; index < descendants->size(); index++)
    // {
    //     tg.run([&mutationOperator, &parameters, &descendants, &index] {
    //             descendants->at(index)->mutate(parameters, mutationOperator);
    //         }
    //     );
    // }

    // tg.wait();

    // oneapi::tbb::parallel_for(oneapi::tbb::blocked_range<unsigned int>(0, static_cast<unsigned int>(descendants->size()-1)),
    //     [&mutationOperator, &parameters, &descendants](const oneapi::tbb::blocked_range<unsigned int>& r) {
    //         for(unsigned int index = r.begin(); index < r.end(); index++) {
    //             descendants->at(index)->mutate(parameters, mutationOperator);
    //         }
    //     }
    // );

    for(unsigned int index = 0; index < descendants->size(); index++)
    {
        descendants->at(index)->mutate(parameters, mutationOperator);
    }

    cerr << "[DONE]" << endl;
}

unique_ptr<vector<Individual*>> Optimiser::doReplacementOperator(ReplacementOperatorType replacementOperator, Population& currentPopulation, oneapi::tbb::concurrent_vector<unique_ptr<Individual>>& descendants)
{
    cerr << TAB << "- Replacing population ... ";

    unique_ptr<vector<Individual*>> newIndividuals = replacementOperator(currentPopulation, descendants);

    cerr << "[DONE]" << endl;

    return newIndividuals;
}

void Optimiser::copyDirectory(const fs::path& source, const fs::path& destination) {
    if(fs::is_directory(source) && fs::exists(source))
    {
        if (!fs::exists(destination)) {
            fs::create_directories(destination);
        }

        for (const auto& entry : fs::directory_iterator(source)) {
            fs::path destinationEntry = destination / entry.path().filename();

            if(fs::is_directory(entry.path())) 
            {
                copyDirectory(entry.path(), destinationEntry);
            } 
            else 
            {
                fs::copy_file(entry.path(), destinationEntry, fs::copy_options::overwrite_existing);
            }
        }
    } 
    else 
    {
        throwLineInfoException("The source directory does not exist or is not a valid directory.");
    }
}

void Optimiser::saveBestIndividual(fs::path logFilePath, const Individual& bestIndividual, const vector<Parameter>& parameters, fs::path outputFolder)
{
    cerr << TAB << "- Saving best individual ... ";

    json logContent = readConfigFile(logFilePath);

    logContent["bestIndividual"] = bestIndividual.getInfo(parameters);

    saveConfigFile(logFilePath, logContent);

    fs::path configPath = outputFolder / "best_individual" / "config";
    fs::create_directories(configPath);
    copyDirectory(bestIndividual.getConfigPath(), configPath);

    fs::path resultPath = outputFolder / "best_individual" / "result";
    fs::create_directories(resultPath);
    copyDirectory(bestIndividual.getResultPath(), resultPath);

    cerr << "[DONE]" << endl;
}

void Optimiser::runOptimiser(const json& run_params, const bool overwrite)
{
    vector<Parameter> parameters;
    parameters.reserve(run_params["parameters"].size());

    for(const json& parameter : run_params["parameters"])
    {
        parameters.emplace_back(parameter);
    }

    runOptimiser(run_params["baseConfigPath"], run_params["outputFolder"], run_params["maxNumberOfGenerations"], run_params["numberOfCores"], parameters, run_params["burn_in"], run_params["maximiseFitness"], run_params["evaluationFunctionSelected"], run_params["selectionOperatorSelected"], run_params["crossoverOperatorSelected"], run_params["mutationOperatorSelected"], run_params["replacementOperatorSelected"], run_params["fitnessObjective"], overwrite);
}

void Optimiser::runOptimiser(fs::path baseConfigPath, fs::path outputFolder, unsigned int maxNumberOfGenerations, unsigned int numberOfCores, vector<Parameter>& parameters, unsigned int burn_in, bool maximiseFitness, string evaluationFunctionSelected, string selectionOperatorSelected, string crossoverOperatorSelected, string mutationOperatorSelected, string replacementOperatorSelected, const double& fitnessObjective, const bool overwrite)
{
    Random::initRandomGenerator(0);

    if(fs::exists(outputFolder))
    {
        if(overwrite)
        {
            fs::remove_all(outputFolder);
        }
        else
        {
            throwLineInfoException("Error: The output directory already exists.");
        }
    }

    fs::create_directories(outputFolder);


    cerr << "##############################" << endl << endl;

    fs::path logFilePath = initializeLog(outputFolder);


    Individual bestIndividual(maximiseFitness);


    EvaluationFunctionType evaluationFunction = EvaluationFunction::selectFunction(evaluationFunctionSelected);

    SelectionOperatorType selectionOperator = SelectionOperator::selectOperator(selectionOperatorSelected);

    CrossoverOperatorType crossoverOperator = CrossoverOperator::selectOperator(crossoverOperatorSelected);

    MutationOperatorType mutationOperator = MutationOperator::selectOperator(mutationOperatorSelected);

    ReplacementOperatorType replacementOperator = ReplacementOperator::selectOperator(replacementOperatorSelected);


    unsigned int currentGeneration = 1;

    cerr << "######################" << endl;
    cerr << "## Generation " << currentGeneration << "/" << maxNumberOfGenerations << endl << endl;

    fs::path generationOutputFolder = createGenerationFolder(outputFolder, currentGeneration);

    cerr << TAB << "- Initialising first population ... ";

    unique_ptr<Population> currentPopulation = make_unique<Population>(parameters, currentGeneration, maximiseFitness);

    cerr << "[DONE]" << endl;

    bestIndividual = currentPopulation->evaluate(parameters, numberOfCores, baseConfigPath, generationOutputFolder, logFilePath, evaluationFunction, burn_in, bestIndividual, maximiseFitness);

    currentPopulation->updateLog(logFilePath, parameters);

    printBestIndividual(bestIndividual, parameters);

    cerr << "######################" << endl << endl;

    currentGeneration += 1;

    while(currentGeneration <= maxNumberOfGenerations && !achieveObjectiveFitness(bestIndividual.getFitness(), fitnessObjective, maximiseFitness))
    {
        cerr << "######################" << endl;
        cerr << "## Generation " << currentGeneration << "/" << maxNumberOfGenerations << endl << endl;

        unique_ptr<vector<Individual*>> parents = doSelectionOperator(selectionOperator, *currentPopulation, maximiseFitness);

        unique_ptr<oneapi::tbb::concurrent_vector<unique_ptr<Individual>>> descendants = doCrossoverOperator(crossoverOperator, parents, maximiseFitness);

        doMutationOperator(parameters, mutationOperator, descendants);

        fs::path generationOutputFolder = createGenerationFolder(outputFolder, currentGeneration);

        bestIndividual = currentPopulation->evaluate(parameters, numberOfCores, baseConfigPath, generationOutputFolder, logFilePath, evaluationFunction, burn_in, bestIndividual, maximiseFitness);

        unique_ptr<vector<Individual*>> newIndividuals = doReplacementOperator(replacementOperator, *currentPopulation, *descendants);
        currentPopulation = make_unique<Population>(*newIndividuals, currentGeneration);

        currentPopulation->updateLog(logFilePath, parameters);

        printBestIndividual(bestIndividual, parameters);

        currentGeneration += 1;

        cerr << "######################" << endl << endl;
    }

    saveBestIndividual(logFilePath, bestIndividual, parameters, outputFolder);

    cerr << TAB << "- Closing log ... [DONE]" << endl << endl;

    cerr << "## Finished optimization ##" << endl << endl;

    cerr << "##############################" << endl;
}
