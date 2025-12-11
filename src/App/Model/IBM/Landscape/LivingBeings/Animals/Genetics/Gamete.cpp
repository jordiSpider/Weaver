#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Gamete.h"

using namespace std;


Gamete::Gamete(const size_t &numberOfChromosomes)
{
    chromosomes.reserve(numberOfChromosomes);
}

/// @brief Destroys the gamete and all the Chromosomes inside it, freeing up memory
Gamete::~Gamete()
{
    chromosomes.clear();
}

size_t Gamete::size() const 
{ 
    return chromosomes.size(); 
}

void Gamete::pushChromosome(Chromosome *newChromosome) 
{ 
    chromosomes.emplace_back(newChromosome); 
}

Chromosome* Gamete::getChromosome(const size_t& position) 
{ 
    return chromosomes.at(position); 
}
