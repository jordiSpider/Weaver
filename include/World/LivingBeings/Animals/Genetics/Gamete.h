#ifndef GAMETES_H_
#define GAMETES_H_

#include <vector>

#include "Chromosome.h"
#include "Maths/Random.h"


class Gamete
{
private:
    std::vector<Chromosome *> chromosomes;

public:
    Gamete(const unsigned int &numberOfChromosomes);
    ~Gamete();
    inline unsigned int size() const { return chromosomes.size(); }
    inline void pushChromosome(Chromosome *newChromosome) { chromosomes.emplace_back(newChromosome); }
    inline Chromosome* getChromosome(const unsigned int& position) { return chromosomes.at(position); }
};

#endif
