
#include "Tools/Optimiser/CrossoverOperator.h"


using namespace std;
using json = nlohmann::json;


// Crossover operator
// Crosses pairs of parents via chiasmus.
// 
// @param parents Pair of parents.
// @return Pair of children.
void chiasmsMethod(const Individual& firstParent, const Individual& secondParent, oneapi::tbb::concurrent_vector<unique_ptr<Individual>>& descendants, const bool maximiseFitness)
{
    // Define the number of chiasms
    const unsigned int numberOfChiasms = 1;

    if(firstParent.getParametersValue().size() > 1)
    {
        // Random selection of chiasms
        unique_ptr<vector<unsigned int>> chiasmsPoints = Random::createUniqueIndicesVector(1, firstParent.getParametersValue().size(), numberOfChiasms);

        // Sequential crossing of chiasms

        vector<json> firstChildValue;
        firstChildValue.reserve(firstParent.getParametersValue().size());
        vector<json> secondChildValue;
        secondChildValue.reserve(firstParent.getParametersValue().size());

        unsigned int previousPoint = 0;

        for(unsigned int index = 0; index < chiasmsPoints->size(); index++)
        {
            if(index % 2 != 0)
            {
                firstChildValue.insert(firstChildValue.cend(), firstParent.getParametersValue().cbegin() + previousPoint, firstParent.getParametersValue().cbegin() + chiasmsPoints->at(index));
                secondChildValue.insert(secondChildValue.cend(), secondParent.getParametersValue().cbegin() + previousPoint, secondParent.getParametersValue().cbegin() + chiasmsPoints->at(index));
            }
            else
            {
                firstChildValue.insert(firstChildValue.cend(), secondParent.getParametersValue().cbegin() + previousPoint, secondParent.getParametersValue().cbegin() + chiasmsPoints->at(index));
                secondChildValue.insert(secondChildValue.cend(), firstParent.getParametersValue().cbegin() + previousPoint, firstParent.getParametersValue().cbegin() + chiasmsPoints->at(index));
            }

            previousPoint = chiasmsPoints->at(index);
        }

        if(chiasmsPoints->size() % 2 != 0)
        {
            firstChildValue.insert(firstChildValue.cend(), secondParent.getParametersValue().cbegin() + previousPoint, secondParent.getParametersValue().cend());
            secondChildValue.insert(secondChildValue.cend(), firstParent.getParametersValue().cbegin() + previousPoint, firstParent.getParametersValue().cend());
        }
        else
        {
            firstChildValue.insert(firstChildValue.cend(), firstParent.getParametersValue().cbegin() + previousPoint, firstParent.getParametersValue().cend());
            secondChildValue.insert(secondChildValue.cend(), secondParent.getParametersValue().cbegin() + previousPoint, secondParent.getParametersValue().cend());
        }

        // Creation of new individuals
        descendants.push_back(make_unique<Individual>(firstChildValue, maximiseFitness));
        descendants.push_back(make_unique<Individual>(secondChildValue, maximiseFitness));
    }
    else
    {
        if(Random::randomDoubleInRange(0.0, 1.0) < 0.5)
        {
            descendants.push_back(make_unique<Individual>(firstParent.getParametersValue(), maximiseFitness));
            descendants.push_back(make_unique<Individual>(secondParent.getParametersValue(), maximiseFitness));
        }
        else
        {
            descendants.push_back(make_unique<Individual>(secondParent.getParametersValue(), maximiseFitness));
            descendants.push_back(make_unique<Individual>(firstParent.getParametersValue(), maximiseFitness));
        }
    }
}

CrossoverOperatorType CrossoverOperator::selectOperator(string crossoverOperatorSelected)
{
    if(crossoverOperatorSelected == "Chiasms")
    {
        return &chiasmsMethod;
    }
    else
    {
        throwLineInfoException("Error: Unrecognised crossover operator.");
    }
}
