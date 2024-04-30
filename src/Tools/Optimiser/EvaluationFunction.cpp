
#include "Tools/Optimiser/EvaluationFunction.h"


using namespace std;
using json = nlohmann::json;


double amplitudeFunction(const nlohmann::json& info)
{
    json selectedAnimalSpecies = info["animalSpecies"]["Consumer_fast_asexual"];

    double minX = selectedAnimalSpecies["amplitude"]["min"];
    double maxX = selectedAnimalSpecies["amplitude"]["max"];

    double amplitude = (maxX-minX)/(maxX+minX);

    return amplitude;
}

EvaluationFunctionType EvaluationFunction::selectFunction(std::string evaluationFunctionSelected)
{
    if(evaluationFunctionSelected == "Amplitude")
    {
        return &amplitudeFunction;
    }
    else
    {
        throwLineInfoException("Error: Unrecognised evaluation function.");
    }
}
