/*
 * ChromosomesGenerator.cpp
 *
 *  Created on: 9 Nov 2012
 *      Author: gabi
 */

#include "ChromosomesGenerator.h"
#include "AnimalSpecies.h"
#include <deque>

/**
 *
 * @param n_loci Number of
 * @param n_alleles
 */

ChromosomesGenerator::ChromosomesGenerator(AnimalSpecies * species)
{
	this->mySpecies = species;

	loci.reserve(species->numberOfLociPerTrait);
	for (unsigned int i = 0; i < species->numberOfLociPerTrait; ++i)
	{
		loci.push_back(new Locus(species->getNumberOfAllelesPerLocus()));
	}

	randomlyCreatedPositionsForCorrelosomes = new vector<int>(species->getNumberOfTraits()*species->getNumberOfLociPerTrait());
	for(unsigned int i = 0; i < species->getNumberOfTraits()*species->getNumberOfLociPerTrait(); ++i)
	{
		randomlyCreatedPositionsForCorrelosomes->at(i) = i;
	}
	random_shuffle(randomlyCreatedPositionsForCorrelosomes->begin(), randomlyCreatedPositionsForCorrelosomes->end());
}

ChromosomesGenerator::~ChromosomesGenerator()
{
	for(auto elem : loci) {
		delete elem;
	}
	loci.clear();

	delete randomlyCreatedPositionsForCorrelosomes;
}

Correlosome* ChromosomesGenerator::getCorrelosome()
{
	vector<Allele*>* correlosomeAlleles = new vector<Allele*>();
	for (auto it = loci.begin(); it < loci.end(); it++)
	{
		correlosomeAlleles->push_back((*it)->getAlleleRandomly());
	}
	return new Correlosome(correlosomeAlleles);
}

Genome* ChromosomesGenerator::getNewGenome()
{
	vector<pair<Correlosome*, Correlosome*> >* homologousCorrelosomes = new vector<pair<Correlosome*, Correlosome*> > ;
	for (unsigned int i = 0; i < mySpecies->getNumberOfTraits(); ++i)
	{
		homologousCorrelosomes->push_back(make_pair(getCorrelosome(), getCorrelosome()));
	}

	vector<pair<Chromosome*, Chromosome*> >* homologousChromosomes = new vector<pair<Chromosome*, Chromosome*> >(mySpecies->getNumberOfChromosomes());
	for (unsigned int i = 0; i < mySpecies->getNumberOfChromosomes(); ++i)
	{
		homologousChromosomes->at(i) = make_pair(new Chromosome(mySpecies->getNumberOfLociPerChromosome()), new Chromosome(mySpecies->getNumberOfLociPerChromosome()));
	}
	//The vector is now ready for the new chromosomeAlleles to be inserted one by one.
	//This entire process will not create new Alleles, though. Hence, a big deal of memory use is saved.

	int positionFromCorrelosomes;
	int selectedTraitOnCorrelosome;
	int selectedLociOnCorrelosome;
	int linkedChromosome;
	int linkedLociOnChromosome;
	const Allele* selectedAllele;

	//Here we select the Alelle we have to link, and we create in the chromosomes a pointer for each allele contained in the correlosomes.
	//Notice that the Alleles are not being duplicated, but their pointers are. This saves some memory.
	//This makes initTraits much more easy and understandable, and permits the initialization based on the correlosomes alleles, rather than based on the pure chromosomes.
	//Also the correlosomes are temporary, as they will be cleared after initTraits.
	for(unsigned int i = 0; i < randomlyCreatedPositionsForCorrelosomes->size(); ++i)
	{
		positionFromCorrelosomes = randomlyCreatedPositionsForCorrelosomes->at(i);
		selectedTraitOnCorrelosome = positionFromCorrelosomes/mySpecies->getNumberOfLociPerTrait();
		selectedLociOnCorrelosome = positionFromCorrelosomes%mySpecies->getNumberOfLociPerTrait();

		linkedChromosome = i/mySpecies->getNumberOfLociPerChromosome();
		linkedLociOnChromosome = i%mySpecies->getNumberOfLociPerChromosome();

		selectedAllele = homologousCorrelosomes->at(selectedTraitOnCorrelosome).first->getAlleles()->at(selectedLociOnCorrelosome);
		homologousChromosomes->at(linkedChromosome).first->setAllele(selectedAllele, linkedLociOnChromosome);
		selectedAllele = homologousCorrelosomes->at(selectedTraitOnCorrelosome).second->getAlleles()->at(selectedLociOnCorrelosome);
		homologousChromosomes->at(linkedChromosome).second->setAllele(selectedAllele, linkedLociOnChromosome);
	}

	for (auto it = homologousCorrelosomes->begin(); it != homologousCorrelosomes->end(); ++it)
	{
		delete (*it).first;
		delete (*it).second;
	}
	homologousCorrelosomes->clear();
	delete homologousCorrelosomes;

	return new Genome(homologousChromosomes, mySpecies);
}

vector<pair<Correlosome*, Correlosome*> >* ChromosomesGenerator::getCorrelosomesFromChromosomes(vector<pair<Chromosome*, Chromosome*> >* homologousChromosomes)
{
	vector<pair<Correlosome*, Correlosome*> >* homologousCorrelosomes = new vector<pair<Correlosome*, Correlosome*> >(mySpecies->getNumberOfTraits());
	for (size_t i = 0; i < mySpecies->getNumberOfTraits(); ++i)
	{
		homologousCorrelosomes->at(i) = make_pair(new Correlosome(mySpecies->getNumberOfLociPerTrait()), new Correlosome(mySpecies->getNumberOfLociPerTrait()));
	}
	//The correlosome vector is now ready for the new alleles to be inserted one by one, chosen from the chromosomes.
	//This entire process will not create new Alleles, though. Hence, a big deal of memory use is saved.

	int positionFromCorrelosomes;
	int selectedTraitOnCorrelosome;
	int selectedLociOnCorrelosome;
	int linkedChromosome;
	int linkedLociOnChromosome;
	const Allele* selectedAllele;
	for(unsigned int i = 0; i < randomlyCreatedPositionsForCorrelosomes->size(); ++i)
	{
		positionFromCorrelosomes = randomlyCreatedPositionsForCorrelosomes->at(i);
		selectedTraitOnCorrelosome = positionFromCorrelosomes/mySpecies->getNumberOfLociPerTrait();
		selectedLociOnCorrelosome = positionFromCorrelosomes%mySpecies->getNumberOfLociPerTrait();

		linkedChromosome = i/mySpecies->getNumberOfLociPerChromosome();
		linkedLociOnChromosome = i%mySpecies->getNumberOfLociPerChromosome();

		selectedAllele = homologousChromosomes->at(linkedChromosome).first->getAlleles()->at(linkedLociOnChromosome);
		homologousCorrelosomes->at(selectedTraitOnCorrelosome).first->setAllele(selectedAllele, selectedLociOnCorrelosome);
		selectedAllele = homologousChromosomes->at(linkedChromosome).second->getAlleles()->at(linkedLociOnChromosome);
		homologousCorrelosomes->at(selectedTraitOnCorrelosome).second->setAllele(selectedAllele, selectedLociOnCorrelosome);
	}

	return homologousCorrelosomes;
}

