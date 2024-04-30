/*
 * Loci.h
 *
 *  Created on: 30/10/2012
 *      Author: Gabi
 */

#ifndef LOCI_H_
#define LOCI_H_

#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>
#include <ostream>

#include "Allele.h"
#include "IBM/Maths/Random.h"
#include "Misc/SerializableSpecializations.h"


/**
 * @class Locus
 * @brief This is the specification for a locus. One locus contains a vector of alleles.
 *        One of them will be chosen in the process of making new chromosomes.
 */
class Locus
{
private:
	std::vector<const Allele*> alleles; /**< Vector of alleles for this locus */

public:
     Locus();

	/**
     * @brief Constructor for Locus.
     * @param numberOfAlleles The number of alleles at this locus.
     */
	Locus(const unsigned int &numberOfAlleles);

	/**
     * @brief Destructor for Locus.
     */
	virtual ~Locus();

	/**
     * @brief Get a random allele from the locus.
     * @return A pointer to a randomly chosen allele.
     */
	inline const Allele* const getAlleleRandomly() const { return alleles.at(Random::randomIndex(alleles.size())); };

     const std::vector<const Allele*>& getAlleles() const;

	/**
     * @brief Serialize the Locus object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
     template<class Archive>
     void serialize(Archive & ar, const unsigned int version);
};

#endif /* LOCI_H_ */
