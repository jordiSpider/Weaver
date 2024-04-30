/*
 * Chromosome.cpp
 *
 *  Created on: 30/10/2012
 *      Author: Gabi
 */

#include "IBM/World/LivingBeings/Animals/Genetics/Chromosome.h"

using namespace std;


Chromosome::Chromosome()
{

}

Chromosome::Chromosome(const unsigned int numberOfLociPerChromosome)
{
	alleles.reserve(numberOfLociPerChromosome);
	//The vector is ready for the new chromosomeAlleles to be inserted one by one.
	//This entire process will not create new Alleles, though.
}

Chromosome::Chromosome(const Chromosome &otherChromosome)
{
	this->alleles.reserve(otherChromosome.size());
	for(unsigned int i = 0; i < otherChromosome.size(); i++)
	{
		this->pushAllele(otherChromosome.getAllele(i));
	}
}

Chromosome::~Chromosome()
{
	// Nothing to delete dynamically
}

void Chromosome::swapAlleles(const unsigned int lociPosition, Chromosome* const otherChromosome, const unsigned int otherLociPosition)
{
	const Allele* const alleleToSwap = this->getAllele(lociPosition);
	this->setAllele(otherChromosome->getAllele(otherLociPosition), lociPosition);
	otherChromosome->setAllele(alleleToSwap, otherLociPosition);
}

//TODO COMPROBAR QUE SE IMPRIMAN BIEN
ostream& operator<<(ostream& os, const Chromosome &chromosome)
{
	for(unsigned int i = 0; i < chromosome.size(); ++i)
	{
		os << chromosome.getAllele(i) << "\t";
	}
	return os;
}

template<class Archive>
void Chromosome::serialize(Archive & ar, const unsigned int version, const vector<Locus> &loci) {
	unsigned int numberOfAlleles;
	if(Archive::is_loading::value) 
	{
		ar & numberOfAlleles;
		alleles.resize(numberOfAlleles);
	}
	else
	{
		numberOfAlleles = alleles.size();
		ar & numberOfAlleles;
	}

	for(unsigned int i = 0; i < numberOfAlleles; i++)
	{
		unsigned int alleleAlphabeticOrder;
		if(Archive::is_loading::value) 
		{
			ar & alleleAlphabeticOrder;
			alleles[i] = &loci[i].getAlleles().at(alleleAlphabeticOrder);
		}
		else
		{
			alleleAlphabeticOrder = alleles[i]->getAlphabeticOrder();
			ar & alleleAlphabeticOrder;
		}
	}
}


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, Chromosome* &chromosomePtr, const unsigned int version, const vector<Locus> &loci) {
			if(Archive::is_loading::value) 
			{
				chromosomePtr = new Chromosome();
			}

			chromosomePtr->serialize(ar, version, loci);
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, Chromosome*&, const unsigned int, const vector<Locus> &);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, Chromosome*&, const unsigned int, const vector<Locus> &);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, Chromosome*&, const unsigned int, const vector<Locus> &);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, Chromosome*&, const unsigned int, const vector<Locus> &);
    }
}
