/*
 * Chromosome.h
 *
 *  Created on: 30/10/2012
 *      Author: Gabi
 */

#ifndef CHROMOSOME_H_
#define CHROMOSOME_H_

#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <fstream>
#include <vector>
#include <ostream>

#include "IBM/World/LivingBeings/Animals/Genetics/Allele.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Locus.h"


/**
 * @class Chromosome
 * @brief This is the specification for a chromosome. One chromosome just contains an
 * array of alleles and each of these was taken from the available alleles in a locus.
 */
class Chromosome
{
private:
	std::vector<const Allele*> alleles; /**< A vector containing all the alleles of this chromosome */

	friend class boost::serialization::access;

public:
	explicit Chromosome();
	/**
	 * Constructs a chromosome with the specified alleles.
	 * @param alleles the alleles of this Chromosome
	 */
	explicit Chromosome(const unsigned int numberOfLociPerChromosome);
	explicit Chromosome(const Chromosome &otherChromosome);
	inline Chromosome* clone() const { return new Chromosome(*this); }
	/**
	 * Destroys this Chromosome deleting its data.
	 */
	~Chromosome();

	// Getters
	inline const Allele* const getAllele(const unsigned int lociPosition) const { return alleles.at(lociPosition); }
	inline const size_t size() const { return alleles.size(); }

	// Setters
	inline void setAllele(const Allele* const allele, const unsigned int lociPosition) { alleles.at(lociPosition) = allele; }
	inline void pushAllele(const Allele* const allele) { alleles.emplace_back(allele); }
	
	void swapAlleles(const unsigned int lociPosition, Chromosome* const otherChromosome, const unsigned int otherLociPosition);

	/**
	 * Prints the specified Chromosome into the specified output stream.
	 * @param os the output stream where to print
	 * @param chromosome the Chromosome to be printed
	 * @return the updated output stream
	 */
	friend std::ostream& operator<<(std::ostream& os, const Chromosome &chromosome);

	/**
     * @brief Serialize the object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version, const std::vector<Locus> &loci);
};

namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, Chromosome* &chromosomePtr, const unsigned int version, const std::vector<Locus> &loci);
    }
}

#endif /* CHROMOSOME_H_ */
