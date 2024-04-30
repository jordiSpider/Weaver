/*
 * Genome.h
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#ifndef GENOME_H_
#define GENOME_H_

#include "Chromosome.h"
#include "Correlosome.h"
#include "ChromosomesGenerator.h"
#include "AnimalSpecies.h"
#include "Gamete.h"

class Species;

class Genome
{
private:
	vector<pair<Chromosome*, Chromosome*> >* homologousChromosomes;
	vector<pair<Correlosome*, Correlosome*> >* homologousCorrelosomes;
	Species* mySpecies;

public:
	Genome(vector<pair<Chromosome*, Chromosome*> >* inheritedChromosomes, Species* mySpecies);
	Genome(const Genome* otherGenome);
	virtual ~Genome();

	//4 vectors (cells) of 20 chromosomes each. The animal will select one out of those 4 cells.
	vector<Gamete*>* getGametesFromMeiosis();
	Gamete * getRandomGameteFromMeiosis();
	Gamete * cloneFirstGameteFromHaploid() const;
	Gamete * cloneSecondGameteFromHaploid() const;
	const vector<pair<Chromosome*, Chromosome*> >* getHomologousChromosomes() const;
	Genome* clone() const;
	const vector<pair<Correlosome*, Correlosome*> >* getHomologousCorrelosomes() const;
	void createHomologousCorrelosomes();
	void deleteHomologousCorrelosomes();
	Species* getSpecies() const {	return mySpecies; }
};

#endif /* GENOME_H_ */
