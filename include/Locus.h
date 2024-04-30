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
#include "SimulationConstants.h"

using namespace std;

/**
 * This is the specification for a locus. One locus just contains a
 * vector of alleles. One of them will be chosen in the process of
 * making new chromosomes.
 */
class Locus
{
private:
	vector<Allele*> alleles;
public:
	Locus(int numberOfAlleles);
	virtual ~Locus();
	Allele* getAlleleRandomly();
};

#endif /* LOCI_H_ */
