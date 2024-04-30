/*
 * Genome.h
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#ifndef GENOME_H_
#define GENOME_H_

#include "Locus.h"
#include "Chromosome.h"
#include "Correlosome.h"
#include "Gamete.h"
#include "Maths/Random.h"
#include "LineInfoException.h"


class Genome
{
private:
	static constexpr unsigned int NUMBER_GAMETES_TO_GENERATE = 4;

	std::vector<std::pair<Chromosome*, Chromosome*> > homologousChromosomes;
	std::vector<std::pair<Correlosome*, Correlosome*> > homologousCorrelosomes;
	const unsigned int numberOfLociPerChromosome;
	const unsigned int numberOfChiasmasPerChromosome;
	void createHomologousCorrelosomesFromChromosomes(const std::vector<int> &randomlyCreatedPositionsForChromosomes);

public:
	Genome(Gamete* const firstParentGamete, Gamete* const secondParentGamete, const std::vector<int> &randomlyCreatedPositionsForChromosomes, 
		   const unsigned int& numberOfLociPerChromosome, const unsigned int& numberOfChiasmasPerChromosome);
	Genome(const std::vector<Locus*> &loci, const std::vector<int> &randomlyCreatedPositionsForChromosomes, const int &numberOfChromosomes,
		   const unsigned int& numberOfLociPerChromosome, const unsigned int& numberOfChiasmasPerChromosome);
	Genome(const Genome& otherGenome, const std::vector<int> &randomlyCreatedPositionsForChromosomes);
	virtual ~Genome();

	//4 vectors (cells) of 20 chromosomes each. The animal will select one out of those 4 cells.
	Gamete* getGametesFromMeiosis(const unsigned int &indexSelectedGamete);
	inline Gamete * getRandomGameteFromMeiosis() { return getGametesFromMeiosis(Random::randomIndex(NUMBER_GAMETES_TO_GENERATE)); }
	Gamete * cloneFirstGameteFromHaploid() const;
	Gamete * cloneSecondGameteFromHaploid() const;
	inline const std::vector<std::pair<Chromosome*, Chromosome*> >& getHomologousChromosomes() const { return homologousChromosomes; }
	inline const std::vector<std::pair<Correlosome*, Correlosome*> >& getHomologousCorrelosomes() const { return homologousCorrelosomes; }
	void deleteHomologousCorrelosomes();
	inline Genome* clone(const std::vector<int> &randomlyCreatedPositionsForChromosomes) const { return new Genome(*this, randomlyCreatedPositionsForChromosomes); }
};

#endif /* GENOME_H_ */
