/*
 * Chromosome.cpp
 *
 *  Created on: 30/10/2012
 *      Author: Gabi
 */

#include "Chromosome.h"

using namespace std;


Chromosome::Chromosome(int numberOfLociPerChromosome)
{
	alleles = new vector<Allele*>(numberOfLociPerChromosome);
	//The vector is ready for the new chromosomeAlleles to be inserted one by one.
	//This entire process will not create new Alleles, though.
}

Chromosome::Chromosome(const Chromosome* otherChromosome)
{
	alleles = new vector<Allele*>(otherChromosome->getAlleles()->size());
	for(unsigned int i = 0; i < otherChromosome->getAlleles()->size(); i++)
	{
		alleles->at(i) = otherChromosome->getAlleles()->at(i)->clone();
	}
}

Chromosome::~Chromosome()
{
	for (auto it = alleles->begin(); it != alleles->end(); ++it)
	{
		delete (*it);
	}
	
	alleles->clear();
	delete alleles; //Dinosaurs - reversed in dinosurs
}

void Chromosome::swapAlleles(const int& lociPosition, const Chromosome* otherChromosome, const int& otherLociPosition)
{
	Allele* alleleToSwap = this->alleles->at(lociPosition);
	this->alleles->at(lociPosition) = otherChromosome->alleles->at(otherLociPosition);
	otherChromosome->alleles->at(otherLociPosition) = alleleToSwap;
}

void Chromosome::setAllele(const Allele* allele, int lociPosition)
{
	delete this->alleles->at(lociPosition);
	this->alleles->at(lociPosition) = allele->clone();
}

Chromosome* Chromosome::clone() const
{
	return new Chromosome(this);
}

//TODO COMPROBAR QUE SE IMPRIMAN BIEN
ostream& operator<<(ostream& os, Chromosome& chromosome)
{
	for (unsigned i = 0; i < chromosome.alleles->size(); ++i)
	{
		os << chromosome.alleles->at(i) << "\t";
	}
	return os;
}
