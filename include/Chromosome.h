/*
 * Chromosome.h
 *
 *  Created on: 30/10/2012
 *      Author: Gabi
 */

#ifndef CHROMOSOME_H_
#define CHROMOSOME_H_

#include "Locus.h"
#include "Allele.h"
#include <vector>
#include <ostream>

/**
 * This is the specification for a chromosome. One chromosome just contains an
 * array of alleles and each of these was taken from the available alleles in a locus.
 */
class Chromosome
{
private:
	std::vector<Allele*>* alleles;
public:
	/**
	 * Constructs a chromosome with the specified alleles.
	 * @param alleles the alleles of this Chromosome
	 */
	Chromosome(int numberOfLociPerChromosome);
	Chromosome(const Chromosome* otherChromosome);

	/**
	 * Destroys this Chromosome deleting its data.
	 */
	virtual ~Chromosome();

	inline const std::vector<Allele*>* getAlleles() const { return alleles; }
	void swapAlleles(const int& lociPosition, const Chromosome* otherChromosome, const int& otherLociPosition);
	void setAllele(const Allele* allele, int lociPosition);
	Chromosome* clone() const;

	/**
	 * Prints the specified Chromosome into the specified output stream.
	 * @param os the output stream where to print
	 * @param chromosome the Chromosome to be printed
	 * @return the updated output stream
	 */
	friend std::ostream& operator<<(std::ostream& os, Chromosome& chromosome);
};

#endif /* CHROMOSOME_H_ */
