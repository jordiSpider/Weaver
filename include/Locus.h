/*
 * Loci.h
 *
 *  Created on: 30/10/2012
 *      Author: Gabi
 */

#ifndef LOCI_H_
#define LOCI_H_

#include <vector>

#include "Allele.h"
#include "Maths/Random.h"


/**
 * This is the specification for a locus. One locus just contains a
 * vector of alleles. One of them will be chosen in the process of
 * making new chromosomes.
 */
class Locus
{
private:
	std::vector<const Allele*> alleles;
public:
	Locus(const unsigned int &numberOfAlleles);
	virtual ~Locus();
	inline const Allele* const getAlleleRandomly() const { return alleles.at(Random::randomIndex(alleles.size())); };
};

#endif /* LOCI_H_ */
