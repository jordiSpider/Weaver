/**
 * @file Allele.h
 * @brief Defines the Allele class, representing a genetic allele with a unique identifier,
 *        a numerical value, and an alphabetic order for sorting.
 */

#ifndef ALLELE_H_
#define ALLELE_H_


#include <boost/serialization/export.hpp>

#include <ostream>

#include "App/Model/IBM/Maths/Random.h"


/**
 * @class Allele
 * @brief Represents a genetic allele with a value and an alphabetic order.
 *
 * The Allele class stores a numerical value representing the strength or effect of the allele,
 * and an alphabetic order used for ordering or comparison tasks in genetic algorithms or species genetics.
 */
class Allele
{
public:
     /**
     * @brief Default constructor. Initializes the allele with default values.
     */

     explicit Allele();

     /**
     * @brief Constructor with alphabetic order.
     * @param alphabeticOrder The alphabetic order of the allele.
     */
     explicit Allele(const unsigned int alphabeticOrder);
          
	/**
     * @brief Destructor for the Allele class.
     */
	~Allele();
	
	/**
     * @brief Get the numerical value of the allele.
     * @return The numerical value as a PreciseDouble.
     */
	const PreciseDouble& getValue() const;

     /**
     * @brief Modify the value of the allele by multiplying it by a given factor.
     * @param timesAlleles Factor to multiply the allele's value by.
     */
     void modifyValue(const PreciseDouble& timesAlleles);
	
	/**
     * @brief Get the alphabetic order of the allele.
     * @return The alphabetic order (unsigned int).
     */
	unsigned int getAlphabeticOrder() const;
	
	/**
     * @brief Overloaded output stream operator for easy printing of Allele objects.
     * @param os Output stream.
     * @param allele Allele object to print.
     * @return Reference to the modified output stream.
     */
	friend std::ostream& operator<<(std::ostream& os, const Allele& allele);

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
     PreciseDouble value; /**< Numerical value of the allele */
     unsigned int alphabeticOrder; /**< Alphabetic order for sorting alleles */
};

#endif /* ALLELE_H_ */
