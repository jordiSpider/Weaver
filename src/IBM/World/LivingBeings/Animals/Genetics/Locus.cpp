/*
 * Loci.cpp
 *
 *  Created on: 30/10/2012
 *      Author: Gabi
 */

#include "IBM/World/LivingBeings/Animals/Genetics/Locus.h"

using namespace std;


Locus::Locus()
{

}

Locus::Locus(const unsigned int locusId, const unsigned int &numberOfAlleles)
{
	//double alleleSlice = 1.0 / numberOfAlleles;
	//randomUniform(alleleSlice * i, alleleSlice * i + alleleSlice)

	//DONE alleleSlice is not worth using.
	alleles.reserve(numberOfAlleles);
	for (unsigned int i = 0; i < numberOfAlleles; ++i)
	{
		alleles.emplace_back(locusId*numberOfAlleles+i, i);
	}
	//Randomizing the orders here due to the alleleSlice restriction.
	//If we just had ordered numbers, lower allele values would have lower orders and that is NOT what we want.
	//random_shuffle(alphabeticOrder.begin(), alphabeticOrder.end());
}

Locus::~Locus()
{
	
}

const std::vector<Allele>& Locus::getAlleles() const
{
	return alleles;
}


template<class Archive>
void Locus::serialize(Archive & ar, const unsigned int version)
{
	ar & alleles;
}

// Specialisation
template void Locus::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Locus::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Locus::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Locus::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
