
#include "Tools/Optimiser/Population.h"


using namespace std;
using json = nlohmann::json;
namespace fs = boost::filesystem;


void Population::combineRecursive(const vector<vector<json>>& input, vector<vector<json>>& output, unsigned int currentIndex, vector<json> currentCombination) {
    if(currentIndex == input.size())
    {
        output.push_back(currentCombination);
        return;
    }

    for(const auto& element : input[currentIndex]) 
    {
        vector<json> newCombination = currentCombination;
        newCombination.push_back(element);
        combineRecursive(input, output, currentIndex + 1, newCombination);
    }
}

unique_ptr<vector<vector<json>>> Population::combine(const vector<vector<json>>& input) {
    unique_ptr<vector<vector<json>>> output = make_unique<vector<vector<json>>>();

    unsigned int totalCombinations = 1;
    for(const vector<json>& elem : input)
    {
        totalCombinations *= elem.size();
    }
    output->reserve(totalCombinations);

    combineRecursive(input, *output, 0, {});

    return output;
}

Population::Population(const vector<Parameter>& parameters, const unsigned int generationNumber, const bool maximiseFitness)
    : name("population_" + to_string(generationNumber))
{
    vector<vector<json>> matrix;
    matrix.reserve(parameters.size());

    for(const Parameter& param : parameters)
    {
        matrix.push_back(param.getInitialValues());
    }

    unique_ptr<vector<vector<json>>> combinations = combine(matrix);

    individuals.reserve(combinations->size());

    for(const vector<json>& combi : *combinations) 
    {
        individuals.push_back(new Individual(combi, maximiseFitness));
    }
}

Population::Population(vector<Individual*> individuals, const unsigned int generationNumber)
    : name("population_" + to_string(generationNumber)), individuals(individuals)
{

}

Population::~Population()
{
    for(Individual* elem : individuals)
    {
        delete elem;
    }
}

void Population::evaluateIndividual(fs::path& configPath, fs::path& resultFolder, unsigned int& burn_in)
{
    #ifdef DEBUG
    fs::path program = fs::canonical(fs::path("Weaver_d.out"));
    #else
    fs::path program = fs::canonical(fs::path("Weaver.out"));
    #endif

    string arguments = "-I " + configPath.string() + " -O " + resultFolder.string() + " --optimization-burn-in " + to_string(burn_in) + " --silent";

    string command = program.string() + " " + arguments;

    int result = system(command.c_str());
}

Individual Population::evaluate(const vector<Parameter>& parameters, unsigned int numberOfCores, fs::path baseConfigPath, fs::path generationOutputFolder, fs::path logFilePath, EvaluationFunctionType evaluationFunction, unsigned int burn_in, const Individual& bestIndividual, const bool maximiseFitness)
{
    cerr << TAB << "- Evaluating population ... ";

    fs::path populationConfigsPath = generationOutputFolder / "configs";
    fs::create_directories(populationConfigsPath);

    vector<fs::path> configPathsVector;
    configPathsVector.reserve(individuals.size());

    for(unsigned int index = 0; index < individuals.size(); index++)
    {
        configPathsVector.push_back(individuals[index]->createConfig(baseConfigPath, populationConfigsPath, parameters, index));
    }

    fs::path resultFolder = generationOutputFolder / "results";
    fs::create_directories(resultFolder);


    oneapi::tbb::global_control global_limit(oneapi::tbb::global_control::max_allowed_parallelism, numberOfCores);

    oneapi::tbb::task_group tg;

    for(unsigned int index = 0; index < configPathsVector.size(); index++)
    {
        tg.run([&population = *this, &configPath = configPathsVector[index], &resultFolder, &burn_in] {
                population.evaluateIndividual(configPath, resultFolder, burn_in);
            }
        );
    }

    tg.wait();


    vector<fs::path> resultPathsVector;
    resultPathsVector.reserve(configPathsVector.size());
    for(const auto& entry : fs::directory_iterator(resultFolder))
    {
        resultPathsVector.push_back(fs::absolute(entry.path()));
    }

    sort(resultPathsVector.begin(), resultPathsVector.end());

    const Individual* newBestIndividual = &bestIndividual;
    for(unsigned int index = 0; index < individuals.size(); index++)
    {
        individuals[index]->setInfo(configPathsVector[index], resultPathsVector[index], evaluationFunction);

        if(individuals[index]->isBetterThan(*newBestIndividual, maximiseFitness))
        {
            newBestIndividual = individuals[index];
        }
    }

    cerr << "[DONE]" << endl;

    return *newBestIndividual;
}

void Population::updateLog(const fs::path& logFilePath, vector<Parameter>& parameters) const
{
    cerr << TAB << "- Updating log ... ";

    json logContent = readConfigFile(logFilePath);

    for(unsigned int individualIndex = 0; individualIndex < individuals.size(); individualIndex++)
    {
        string individualName = "individual_" + to_string(individualIndex);
        logContent[name][individualName] = individuals[individualIndex]->getInfo(parameters);
    }

    saveConfigFile(logFilePath, logContent);

    cerr << "[DONE]" << endl;
}

const vector<Individual*>& Population::getIndividuals() const
{
    return individuals;
}

vector<Individual*>& Population::getMutableIndividuals()
{
    return individuals;
}
