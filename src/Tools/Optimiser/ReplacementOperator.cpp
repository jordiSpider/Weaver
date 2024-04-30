
#include "Tools/Optimiser/ReplacementOperator.h"


using namespace std;
using json = nlohmann::json;


// Replacement operator
// Replacement by elitism of the current population by the new population.
// 
// @param oldPopulation Current population.
// @param descendants List of new individuals.
// @return New population.
unique_ptr<vector<Individual*>> elitismMethod(Population& currentPopulation, oneapi::tbb::concurrent_vector<unique_ptr<Individual>>& descendants)
{
    // Define percentage of elitism.
    const double elitismPercentage = 0.2;

    // Define the size of the new population.
    const unsigned int populationSize = currentPopulation.getIndividuals().size();

    // The fitness of each individual in the current population is obtained.
    vector<pair<unsigned int, double>> oldPopulationFitness;
    oldPopulationFitness.reserve(populationSize);

    for(unsigned int index = 0; index < populationSize; index++)
    {
        oldPopulationFitness.emplace_back(index, currentPopulation.getIndividuals().at(index)->getFitness());
    }
    
    // Order individuals in descending order of their fitness.
    sort(oldPopulationFitness.begin(), oldPopulationFitness.end(), [](const pair<unsigned int, double>& a, const pair<unsigned int, double>& b) {
        return a.second > b.second;
    });

    // Calculate the point at which elitism ends
    unsigned int elitismPoint = round(populationSize * elitismPercentage);

    // New population is created by a combination of elitism and random selection of descendants.
    unique_ptr<vector<Individual*>> newIndividuals = make_unique<vector<Individual*>>();
    newIndividuals->reserve(populationSize);

    transform(oldPopulationFitness.begin(), oldPopulationFitness.begin()+elitismPoint, back_inserter(*newIndividuals),
                   [&currentPopulation](const pair<unsigned int, double>& elem) {
            Individual* selectedIndividual = currentPopulation.getMutableIndividuals().at(elem.first);
            currentPopulation.getMutableIndividuals().erase(currentPopulation.getMutableIndividuals().begin()+elem.first);
            return selectedIndividual; 
        }
    );

    unique_ptr<vector<unsigned int>> selectedIndices = Random::createUniqueIndicesVector(0, descendants.size(), populationSize-elitismPoint);
    transform(selectedIndices->begin(), selectedIndices->end(), back_inserter(*newIndividuals),
                   [&descendants](const unsigned int& index) {
            Individual* selectedIndividual = descendants.at(index).release();
            return selectedIndividual; 
        }
    );

    return newIndividuals;
}

ReplacementOperatorType ReplacementOperator::selectOperator(string replacementOperatorSelected)
{
    if(replacementOperatorSelected == "Elitism")
    {
        return &elitismMethod;
    }
    else
    {
        throwLineInfoException("Error: Unrecognised replacement operator.");
    }
}
