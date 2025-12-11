/**
 * @file Chromosome.h
 * @brief Defines the Chromosome class used in the genetic model.
 * 
 * This file contains the definition of the Chromosome class, which represents
 * a chromosome composed of multiple loci. Each locus contains an Allele.
 * The class provides functionality to access, modify, and manipulate alleles,
 * including cloning chromosomes and swapping alleles between chromosomes.
 */

#ifndef CHROMOSOME_H_
#define CHROMOSOME_H_


#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>


#include <vector>
#include <ostream>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Allele.h"


/**
 * @class Chromosome
 * @brief Represents a chromosome in the genetic model.
 * 
 * A chromosome is composed of a sequence of loci, where each locus contains
 * an allele. The Chromosome class allows for allele access, modification,
 * adding new alleles, and genetic operations such as allele swapping
 * between chromosomes. Chromosomes can also be cloned.
 */
class Chromosome
{
public:
	/**
     * @brief Default constructor.
     * 
     * Creates an empty chromosome with no loci.
     */
	explicit Chromosome();

	/**
     * @brief Constructor specifying the number of loci.
     * 
     * @param numberOfLociPerChromosome Number of loci to initialize in the chromosome.
     * Allocates space for the specified number of loci but does not assign alleles.
     */
	explicit Chromosome(const size_t numberOfLociPerChromosome);

	/**
     * @brief Copy constructor.
     * 
     * @param otherChromosome Chromosome to copy from.
     * Performs a shallow copy of the alleles vector.
     */
	explicit Chromosome(const Chromosome &otherChromosome);

	/**
     * @brief Creates a clone of this chromosome.
     * 
     * @return Pointer to a new Chromosome object that is a copy of this chromosome.
     */
	Chromosome* clone() const;

	/**
     * @brief Destructor.
     */
	~Chromosome();

	/**
     * @brief Returns the allele at the specified loci position.
     * 
     * @param lociPosition Index of the locus.
     * @return Pointer to the allele at the given position.
     */
	const Allele* getAllele(const size_t lociPosition) const;

	/**
     * @brief Returns the number of loci in the chromosome.
     * @return Number of alleles (loci) in the chromosome.
     */
	size_t size() const;

	/**
     * @brief Sets the allele at a specific loci position.
     * 
     * @param allele Pointer to the allele to assign.
     * @param lociPosition Index of the locus to modify.
     */
	void setAllele(const Allele* const allele, const size_t lociPosition);

	/**
     * @brief Adds an allele at the end of the chromosome.
     * 
     * @param allele Pointer to the allele to append.
     */
	void pushAllele(const Allele* const allele);
	
	/**
     * @brief Swaps alleles between this chromosome and another chromosome.
     * 
     * @param lociPosition Index of the locus in this chromosome.
     * @param otherChromosome Pointer to the other chromosome.
     * @param otherLociPosition Index of the locus in the other chromosome.
     */
	void swapAlleles(const size_t lociPosition, Chromosome* const otherChromosome, const size_t otherLociPosition);

	/**
     * @brief Stream output operator.
     * 
     * Prints the chromosome to an output stream.
     * 
     * @param os Output stream.
     * @param chromosome Chromosome to print.
     * @return Reference to the output stream.
     */
	friend std::ostream& operator<<(std::ostream& os, const Chromosome &chromosome);

	/**
     * @brief Serializes the chromosome for persistence.
     * 
     * @tparam Archive Serialization archive type.
     * @param ar Archive instance.
     * @param version Serialization version.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

private:
	/// Vector containing pointers to the alleles of the chromosome.
	std::vector<const Allele*> alleles; 
};

#endif /* CHROMOSOME_H_ */
