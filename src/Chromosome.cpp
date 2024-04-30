/*
 * Chromosome.cpp
 *
 *  Created on: 30/10/2012
 *      Author: Gabi
 */

#include "Chromosome.h"

using namespace std;


Chromosome::Chromosome(const unsigned int numberOfLociPerChromosome)
{
	alleles.reserve(numberOfLociPerChromosome);
	//The vector is ready for the new chromosomeAlleles to be inserted one by one.
	//This entire process will not create new Alleles, though.
}

Chromosome::Chromosome(const Chromosome &otherChromosome)
{
	this->alleles.reserve(otherChromosome.size());
	for(unsigned int i = 0; i < otherChromosome.size(); i++)
	{
		this->pushAllele(otherChromosome.getAllele(i));
	}
}

Chromosome::~Chromosome()
{
	// Nothing to delete dynamically
}

void Chromosome::swapAlleles(const unsigned int lociPosition, Chromosome* const otherChromosome, const unsigned int otherLociPosition)
{
	const Allele* const alleleToSwap = this->getAllele(lociPosition);
	this->setAllele(otherChromosome->getAllele(otherLociPosition), lociPosition);
	otherChromosome->setAllele(alleleToSwap, otherLociPosition);
}

//TODO COMPROBAR QUE SE IMPRIMAN BIEN
ostream& operator<<(ostream& os, const Chromosome &chromosome)
{
	for(unsigned int i = 0; i < chromosome.size(); ++i)
	{
		os << chromosome.getAllele(i) << "\t";
	}
	return os;
}
