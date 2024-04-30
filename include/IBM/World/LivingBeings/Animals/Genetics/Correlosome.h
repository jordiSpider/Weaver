
#ifndef CORRELOSOME_H_
#define CORRELOSOME_H_

#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <vector>
#include <ostream>

#include "Allele.h"


class Correlosome
{
private:
	std::vector<const Allele*> alleles;

    friend class boost::serialization::access;

public:
	Correlosome(const unsigned int numberOfLociPerChromosome, const bool onlyReserve = true);
    ~Correlosome();

    // Getters
    inline const Allele* const getAllele(const unsigned int lociPosition) const { return alleles.at(lociPosition); }
    inline const size_t size() const { return alleles.size(); }

    // Setters
    inline void setAllele(const Allele* allele, const unsigned int lociPosition) { alleles.at(lociPosition) = allele; }
    inline void pushAllele(const Allele* allele) { alleles.emplace_back(allele); } 
    
    friend std::ostream& operator<<(std::ostream& os, const Correlosome& correlosome);

    /**
     * @brief Serialize the object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


#endif /* CORRELOSOME_H_ */
