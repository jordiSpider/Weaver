/*
 * Loci.cpp
 *
 *  Created on: 30/10/2012
 *      Author: Gabi
 */

#include "Locus.h"

using namespace std;


Locus::Locus(const unsigned int &numberOfAlleles)
{
	//double alleleSlice = 1.0 / numberOfAlleles;
	//randomUniform(alleleSlice * i, alleleSlice * i + alleleSlice)

	//DONE alleleSlice is not worth using.
	alleles.reserve(numberOfAlleles);
	for (unsigned int i = 0; i < numberOfAlleles; ++i)
	{
		alleles.emplace_back(new Allele(Random::randomUniform(), i));
	}
	//Randomizing the orders here due to the alleleSlice restriction.
	//If we just had ordered numbers, lower allele values would have lower orders and that is NOT what we want.
	//random_shuffle(alphabeticOrder.begin(), alphabeticOrder.end());
}

Locus::~Locus()
{
	for (auto it = alleles.begin() ; it != alleles.end(); ++it)
	{
	   delete (*it);
	}
	alleles.clear();
}
