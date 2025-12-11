/**
 * @file Locus.h
 * @brief Defines the Locus class, representing a genetic locus containing multiple alleles.
 */

#ifndef LOCI_H_
#define LOCI_H_


#include <vector>


#include <boost/serialization/vector.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Allele.h"


/**
 * @class Locus
 * @brief Represents a genetic locus, which contains a set of alleles.
 *
 * A locus stores multiple alleles. During chromosome formation or genetic operations,
 * one allele can be randomly chosen. This class provides accessors for alleles and
 * methods to modify allele values.
 */
class Locus
{
public:
     /**
     * @brief Default constructor. Initializes an empty locus.
     */
     Locus();

	/**
     * @brief Constructor with a specific number of alleles.
     * @param numberOfAlleles Number of alleles to create in the locus.
     */
	Locus(const size_t &numberOfAlleles);

	/**
     * @brief Destructor for the Locus class.
     */
	virtual ~Locus();

     /**
     * @brief Copy constructor.
     * @param other Locus to copy from.
     */
     Locus(const Locus& other);

     /**
     * @brief Assignment operator.
     * @param other Locus to assign from.
     * @return Reference to this Locus.
     */
     Locus& operator=(const Locus& other);

	/**
     * @brief Get a random allele from the locus.
     * @return Pointer to a randomly chosen allele.
     */
	const Allele* getAlleleRandomly() const;

     /**
     * @brief Get all alleles in the locus.
     * @return Const reference to the vector of alleles.
     */
     const std::vector<Allele>& getAlleles() const;

     /**
     * @brief Get the minimum allele value in the locus.
     * @return Const reference to the minimum allele value.
     */
     const PreciseDouble& getMinAlleleValue() const;

     /**
     * @brief Get the maximum allele value in the locus.
     * @return Const reference to the maximum allele value.
     */
     const PreciseDouble& getMaxAlleleValue() const;

     /**
     * @brief Modify the value of a specific allele by multiplying it with a factor.
     * @param affectedAllelesPosition Index of the allele to modify.
     * @param timesAlleles Factor to multiply the allele's value by.
     */
     void modifyAllele(const size_t &affectedAllelesPosition, const PreciseDouble& timesAlleles);

	/**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
     template<class Archive>
     void serialize(Archive & ar, const unsigned int version);

private:
     std::vector<Allele> alleles; /**< Vector of alleles contained in this locus */
     PreciseDouble minAlleleValue; /**< Minimum allele value within this locus */
     PreciseDouble maxAlleleValue; /**< Maximum allele value within this locus */
};

#endif /* LOCI_H_ */
