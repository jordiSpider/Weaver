#ifndef MUTATION_OPERATOR_H_
#define MUTATION_OPERATOR_H_


#include <string>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>


#include "IBM/Maths/Random.h"
#include "Tools/Optimiser/Parameter.h"



using MutationOperatorType = void (*)(const std::vector<Parameter>&, const unsigned int, nlohmann::json&);


void simpleMutationMethod(const std::vector<Parameter>& parameters, const unsigned int parameterIndex, nlohmann::json& parameterValue);

class MutationOperator
{
public:
    static MutationOperatorType selectOperator(std::string mutationOperatorSelected);
};




#endif /* MUTATION_OPERATOR_H_ */