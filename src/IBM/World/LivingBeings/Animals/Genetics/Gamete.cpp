#include "IBM/World/LivingBeings/Animals/Genetics/Gamete.h"

using namespace std;


Gamete::Gamete(const unsigned int &numberOfChromosomes)
{
    chromosomes.reserve(numberOfChromosomes);
}

/// @brief Destroys the gamete and all the Chromosomes inside it, freeing up memory
Gamete::~Gamete()
{
    chromosomes.clear();
}

template<class Archive>
void Gamete::serialize(Archive & ar, const unsigned int version) {
    ar & chromosomes;
}
