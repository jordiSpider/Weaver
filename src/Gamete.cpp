#include "Gamete.h"

Gamete::Gamete()
{

}

/// @brief Destroys the gamete and all the Chromosomes inside it, freeing up memory
Gamete::~Gamete()
{
    for (auto chromosome : chromosomes )
    {
        delete chromosome;
    }

    chromosomes.clear();
}

unsigned int Gamete::size()
{
    return chromosomes.size();
}

void Gamete::pushChromosome(Chromosome *newChromosome)
{
    chromosomes.push_back(newChromosome);
}

const Chromosome *Gamete::getRandomChromosome()
{
    Chromosome * result = nullptr;

    if( chromosomes.size() > 0 )
    {
        result = chromosomes.at(Random::randomIndex(chromosomes.size()));
    }

    return result;
}

void Gamete::deleteChromosomes()
{
    for (auto chromosome : chromosomes )
    {
        delete chromosome;
    }

    chromosomes.clear();
}


const Chromosome *Gamete::getChromosome(const unsigned int& position)
{
    Chromosome * result = nullptr;

    if( chromosomes.size() > position)
    {
        result = chromosomes.at(position);
    }

    return result;
}
