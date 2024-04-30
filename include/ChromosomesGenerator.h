/*
 * ChromosomesGenerator.h
 *
 *  Created on: 9 Nov 2012
 *      Author: gabi
 */

#ifndef WEAVER_CHROMOSOMESGENERATOR_H_
#define WEAVER_CHROMOSOMESGENERATOR_H_

#include <vector>
#include "Locus.h"
#include "Chromosome.h"
#include "Correlosome.h"
#include "AnimalSpecies.h"
#include "Genome.h"
#include "Maths/Constants.h"
using namespace std;

class AnimalSpecies;
class Genome;

/**
 * This is the specification of the chromosomes generator. The generator contains
 * an array of locus. It can perform the following actions:
 * - Generate a new set of chromosomes, selecting the alleles one from each of
 * the locus in the array.
 * - Generate a new pair of gametes, mixing two specified chromosomes from one
 * parent with another two specified chromosomes form the other parent. This is
 * used in the reproduction process.
 */
class ChromosomesGenerator
{
private:
	vector<Locus*> loci;
	//TODO This has room for efficiency improvements. We could just save the final calculations of the actual positions.
	vector<int>* randomlyCreatedPositionsForCorrelosomes;
	AnimalSpecies * mySpecies;
	/**
	 * Provides a new Correlosome. Its alleles are chosen among the available set from each Loci.
	 * @return a new Chromosome.
	 */
	Correlosome* getCorrelosome();

public:
	ChromosomesGenerator();
	ChromosomesGenerator(AnimalSpecies * species);
	virtual ~ChromosomesGenerator();
	Genome* getNewGenome();
	vector<pair<Correlosome*, Correlosome*> >* getCorrelosomesFromChromosomes(vector<pair<Chromosome*, Chromosome*> >* homologousChromosomes);
};

#endif /* WEAVER_CHROMOSOMESGENERATOR_H_ */
