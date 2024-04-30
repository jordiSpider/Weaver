
#include "Tools/Optimiser/Individual.h"


using namespace std;
using json = nlohmann::json;
namespace fs = boost::filesystem;




Individual::Individual(const bool maximiseFitness)
    : fitness((maximiseFitness) ? NEG_DBL_MAX : DBL_MAX)
{

}

Individual::Individual(const vector<json>& parametersValue, const bool maximiseFitness)
    : parametersValue(parametersValue), fitness((maximiseFitness) ? NEG_DBL_MAX : DBL_MAX)
{

}

Individual::~Individual()
{

}

fs::path Individual::createConfig(fs::path baseConfigPath, fs::path output_folder, const vector<Parameter>& parametersInfo, unsigned int index)
{
    Config config(baseConfigPath);

    for(unsigned int i = 0; i < parametersValue.size(); i++)
    {
        Change* change = new AddJsonItemChange(parametersInfo[i].getFolderPath(), parametersInfo[i].getJsonItemPath(), parametersValue[i], parametersInfo[i].getFilename());
        change->applyChange(config);
        delete change;
    }

    string indexStr = string(numeric_limits<unsigned int>::digits10 + 1 - to_string(index).length(), '0') + to_string(index);

    fs::path config_path = output_folder / (baseConfigPath.filename().string() + "_" + indexStr);
    config.save(config_path, false);

    return config_path;
}

void Individual::setInfo(fs::path newConfigPath, fs::path newResultFolderPath, EvaluationFunctionType evaluationFunction)
{
    configPath = newConfigPath.string();
    resultPath = newResultFolderPath.string();
    resultInfo = readConfigFile((newResultFolderPath / "optimization_result.json").string());

    fitness = evaluationFunction(resultInfo);
}

bool Individual::isBetterThan(const Individual& other, const bool maximiseFitness) const
{
    return (maximiseFitness) ? *this > other : *this < other;
}

void Individual::print(const string&& indentation, vector<Parameter>& parameters) const
{
    cerr << indentation << "- Fitness: " << getFitness() << endl;
    cerr << indentation << "- Config path: " << getConfigPath() << endl;
    cerr << indentation << "- Modifications: {" << endl;
    for(unsigned int parameterIndex = 0; parameterIndex < parameters.size(); parameterIndex++)
    {
        cerr << indentation << TAB << parameters[parameterIndex].getName() << ": " << getParametersValue()[parameterIndex] << endl;
    }
    cerr << indentation << "}" << endl;
    cerr << indentation << "- Result path: " << getResultPath() << endl;
    cerr << indentation << "- Result info: " << getResultInfo() << endl;
}

void Individual::mutate(const vector<Parameter>& parameters, MutationOperatorType mutationOperator)
{
    // oneapi::tbb::task_group tg;

    // for(unsigned int index = 0; index < parametersValue.size(); index++)
    // {
    //     tg.run([&mutationOperator, &parameters, &index, &parameterValue = parametersValue[index]] {
    //             mutationOperator(parameters, index, parameterValue);
    //         }
    //     );
    // }

    // tg.wait();

    // oneapi::tbb::parallel_for(oneapi::tbb::blocked_range<unsigned int>(0, static_cast<unsigned int>(parametersValue.size()-1)),
    //     [&](const oneapi::tbb::blocked_range<unsigned int>& r) {
    //         for(unsigned int index = r.begin(); index < r.end(); ++index) 
    //         {
    //             mutationOperator(parameters, index, parametersValue[index]);
    //         }
    //     }
    // );

    for(unsigned int index = 0; index < parametersValue.size(); index++)
    {
        mutationOperator(parameters, index, parametersValue[index]);
    }
}

const double& Individual::getFitness() const
{
    return fitness;
}

const string& Individual::getConfigPath() const
{
    return configPath;
}

const string& Individual::getResultPath() const
{
    return resultPath;
}

const json& Individual::getResultInfo() const
{
    return resultInfo;
}

const vector<json>& Individual::getParametersValue() const
{
    return parametersValue;
}

const json Individual::getInfo(const vector<Parameter>& parameters) const
{
    json individualInfo;

    individualInfo["config"] = getConfigPath();

    individualInfo["modifications"] = json();

    for(unsigned int parameterIndex = 0; parameterIndex < parameters.size(); parameterIndex++)
    {
        individualInfo["modifications"][parameters[parameterIndex].getName()] = getParametersValue()[parameterIndex];
    }

    individualInfo["resultPath"] = getResultPath();
    individualInfo["resultInfo"] = getResultInfo();
    
    individualInfo["fitness"] = getFitness();

    return individualInfo;
}

bool Individual::operator<(const Individual& other) const
{
    return this->getFitness() < other.getFitness();
}

bool Individual::operator>(const Individual& other) const
{
    return this->getFitness() > other.getFitness();
}
