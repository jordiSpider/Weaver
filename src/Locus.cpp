/*
 * Loci.cpp
 *
 *  Created on: 30/10/2012
 *      Author: Gabi
 */

#include "Locus.h"
#include <algorithm>
using namespace std;

Locus::Locus(int numberOfAlleles)
{
	//double alleleSlice = 1.0 / numberOfAlleles;
	//randomUniform(alleleSlice * i, alleleSlice * i + alleleSlice)

	//DONE alleleSlice is not worth using.
	alleles.reserve(numberOfAlleles);
	for (int i = 0; i < numberOfAlleles; ++i)
	{
		float alleleValue = (float)Random::randomUniform();
		if(::isnan(alleleValue))
		{
			cout << alleleValue << " --> NAN RANDOM VALUE??" << endl;
		}
		if(alleleValue == 0)
		{
			cout << alleleValue << " --> ZERO RANDOM VALUE??" << endl;
		}
		alleles.push_back(new Allele(alleleValue, i));
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

/**
 * Retrieves A COPY OF one random allele from the available set within this loci.
 * @return A COPY OF one random allele from the available set within this loci.
 */

Allele* Locus::getAlleleRandomly()
{
	int randomIndexForAllele = Random::randomIndex(alleles.size());
	Allele* alleleToClone = alleles.at(randomIndexForAllele);
	Allele* randomlyGeneratedAllele = alleleToClone->clone();
	return randomlyGeneratedAllele;
}

