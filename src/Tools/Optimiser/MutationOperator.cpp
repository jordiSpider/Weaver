
#include "Tools/Optimiser/MutationOperator.h"



using namespace std;
using json = nlohmann::json;


// Mutation operator
// Mutation of one of an individual's parameters.
// 
// @param parameter Parameter to mutate.
// @return New parameter.
void simpleMutationMethod(const vector<Parameter>& parameters, const unsigned int parameterIndex, json& parameterValue)
{
    // Define mutation probability
    const double mutationProbability = 0.3;

    // Decide whether or not to apply mutation
    if(Random::randomDoubleInRange(0.0, 1.0) < mutationProbability)
    {
        parameterValue = parameters[parameterIndex].generateRandomValue(parameterValue);
    }
}

MutationOperatorType MutationOperator::selectOperator(string mutationOperatorSelected)
{
    if(mutationOperatorSelected == "Simple")
    {
        return &simpleMutationMethod;
    }
    else
    {
        throwLineInfoException("Error: Unrecognised mutation operator.");
    }
}
