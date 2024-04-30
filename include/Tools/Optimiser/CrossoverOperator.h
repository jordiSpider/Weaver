#ifndef CROSSOVER_OPERATOR_H_
#define CROSSOVER_OPERATOR_H_


#include <memory>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <oneapi/tbb/concurrent_vector.h>

#include "Exceptions/LineInfoException.h"
#include "Tools/Optimiser/Population.h"
#include "Tools/Optimiser/Individual.h"
#include "IBM/Maths/Random.h"



using CrossoverOperatorType = void (*)(const Individual&, const Individual&, oneapi::tbb::concurrent_vector<std::unique_ptr<Individual>>&, const bool);


void chiasmsMethod(const Individual& firstParent, const Individual& secondParent, oneapi::tbb::concurrent_vector<std::unique_ptr<Individual>>& descendants, const bool maximiseFitness);

class CrossoverOperator
{
public:
    static CrossoverOperatorType selectOperator(std::string crossoverOperatorSelected);
};




#endif /* CROSSOVER_OPERATOR_H_ */