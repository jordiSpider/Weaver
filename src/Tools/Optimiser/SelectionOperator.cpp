
#include "Tools/Optimiser/SelectionOperator.h"


using namespace std;


// Selection operator
// Selects the parents of the next population using the roulette method.
// 
// @param population Current population.
// @param maximiseFitness Indicates whether the objective is to maximise fitness.
// @return List of selected parents.
unique_ptr<vector<Individual*>> rouletteMethod(const Population& population, const bool maximiseFitness)
{
    // Define the number of parents to be selected.
    // In this case it is equal to the population size.
    unsigned int numberOfParents = population.getIndividuals().size();

    // The fitness of each individual in the population is obtained.
    vector<double> populationFitness;
    populationFitness.reserve(population.getIndividuals().size());

    transform(population.getIndividuals().begin(), population.getIndividuals().end(), back_inserter(populationFitness),
                   [](const Individual*const& individual) { return individual->getFitness(); });

    double totalFitness = accumulate(populationFitness.begin(), populationFitness.end(), 0.0);

    // The probability of being selected is calculated for each individual.
    vector<double> probabilities;
    probabilities.reserve(populationFitness.size());

    transform(populationFitness.begin(), populationFitness.end(), back_inserter(probabilities),
                   [totalFitness](double fitness) { return fitness / totalFitness; });

    unique_ptr<vector<unsigned int>> selection;
    if(maximiseFitness)
    {
        selection = Random::rouletteSelection(probabilities, numberOfParents);
    }
    else
    {
        // If the objective is to minimise the fitness, the complement of the probabilities is calculated.
        vector<double> supplementaryProbability;
        supplementaryProbability.reserve(probabilities.size());

        transform(probabilities.begin(), probabilities.end(), back_inserter(supplementaryProbability),
                   [totalFitness](double probability) { return 1 - probability; });
        
        selection = Random::rouletteSelection(supplementaryProbability, numberOfParents);
    }

    // Select parents using the roulette method.
    unique_ptr<vector<Individual*>> parents = make_unique<vector<Individual*>>();
    parents->reserve(selection->size());

    for(unsigned int index : *selection)
    {
        parents->push_back(population.getIndividuals()[index]);
    }

    return parents;
}

SelectionOperatorType SelectionOperator::selectOperator(string selectionOperatorSelected)
{
    if(selectionOperatorSelected == "Roulette")
    {
        return &rouletteMethod;
    }
    else
    {
        throwLineInfoException("Error: Unrecognised selection operator.");
    }
}
