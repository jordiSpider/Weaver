#ifndef SELECTION_OPERATOR_H_
#define SELECTION_OPERATOR_H_


#include <memory>
#include <vector>
#include <algorithm>

#include "Exceptions/LineInfoException.h"
#include "Tools/Optimiser/Population.h"
#include "Tools/Optimiser/Individual.h"
#include "IBM/Maths/Random.h"



using SelectionOperatorType = std::unique_ptr<std::vector<Individual*>> (*)(const Population&, const bool);


std::unique_ptr<std::vector<Individual*>> rouletteMethod(const Population& population, const bool maximiseFitness);

class SelectionOperator
{
public:
    static SelectionOperatorType selectOperator(std::string selectionOperatorSelected);
};




#endif /* SELECTION_OPERATOR_H_ */