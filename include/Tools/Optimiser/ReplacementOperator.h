#ifndef REPLACEMENT_OPERATOR_H_
#define REPLACEMENT_OPERATOR_H_


#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <oneapi/tbb/concurrent_vector.h>
#include <oneapi/tbb/parallel_for.h>

#include "Tools/Optimiser/Population.h"
#include "Tools/Optimiser/Individual.h"
#include "IBM/Maths/Random.h"



using ReplacementOperatorType = std::unique_ptr<std::vector<Individual*>> (*)(Population&, oneapi::tbb::concurrent_vector<std::unique_ptr<Individual>>&);


std::unique_ptr<std::vector<Individual*>> elitismMethod(Population& currentPopulation, oneapi::tbb::concurrent_vector<std::unique_ptr<Individual>>& descendants);

class ReplacementOperator
{
public:
    static ReplacementOperatorType selectOperator(std::string replacementOperatorSelected);
};




#endif /* REPLACEMENT_OPERATOR_H_ */