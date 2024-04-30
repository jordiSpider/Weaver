#ifndef GAMETES_H_
#define GAMETES_H_

#include "Chromosome.h"
#include "Maths/Random.h"
#include <vector>

using namespace std;

class Gamete
{
private:
    vector<Chromosome *> chromosomes;

public:
    Gamete();
    ~Gamete();
    unsigned int size();
    void pushChromosome(Chromosome *newChromosome);
    const Chromosome * getRandomChromosome();
    const Chromosome * getChromosome(const unsigned int& position);
    void deleteChromosomes();
};

#endif
