/**
 * @file Correlosome.h
 * @brief Defines the Correlosome class used in the genetic model.
 * 
 * This file contains the definition of the Correlosome class, which represents
 * a special type of chromosome (correlosome) composed of multiple loci. Each
 * locus contains an Allele. The class provides functionality to access,
 * modify, and manipulate alleles within the correlosome.
 */

#ifndef CORRELOSOME_H_
#define CORRELOSOME_H_


#include <vector>
#include <ostream>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Allele.h"


/**
 * @class Correlosome
 * @brief Represents a correlosome in the genetic model.
 * 
 * A Correlosome is similar to a chromosome but may be used to represent a
 * correlated set of loci in genetics. It consists of a sequence of loci,
 * each containing an allele. This class provides methods to access and modify
 * alleles and to output the correlosome to a stream.
 */
class Correlosome
{
public:
    /**
     * @brief Constructor.
     * 
     * Initializes a correlosome with the specified number of loci.
     * Optionally, it can only reserve memory without initializing alleles.
     * 
     * @param numberOfLociPerChromosome Number of loci to allocate.
     * @param onlyReserve If true, only reserves memory for alleles without initializing them.
     */
	Correlosome(const size_t numberOfLociPerChromosome, const bool onlyReserve = true);
    
    /**
     * @brief Destructor.
     */
    ~Correlosome();

    /**
     * @brief Returns the allele at the specified loci position.
     * 
     * @param lociPosition Index of the locus.
     * @return Pointer to the allele at the given position.
     */
    const Allele* getAllele(const size_t lociPosition) const;

    /**
     * @brief Returns the number of loci in the correlosome.
     * @return Number of alleles (loci) in the correlosome.
     */
    size_t size() const;

    /**
     * @brief Sets the allele at a specific loci position.
     * 
     * @param allele Pointer to the allele to assign.
     * @param lociPosition Index of the locus to modify.
     */
    void setAllele(const Allele* allele, const size_t lociPosition);

    /**
     * @brief Adds an allele at the end of the correlosome.
     * 
     * @param allele Pointer to the allele to append.
     */
    void pushAllele(const Allele* allele);
    
    /**
     * @brief Stream output operator.
     * 
     * Prints the correlosome to an output stream.
     * 
     * @param os Output stream.
     * @param correlosome Correlosome to print.
     * @return Reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Correlosome& correlosome);

private:
    /// Vector containing pointers to the alleles of the correlosome.
	std::vector<const Allele*> alleles;
};


#endif /* CORRELOSOME_H_ */
