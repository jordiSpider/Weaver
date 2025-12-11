#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Locus.h"

using namespace std;


Locus::Locus()
{

}

Locus::Locus(const size_t &numberOfAlleles)
{
	//double alleleSlice = 1.0 / numberOfAlleles;
	//randomUniform(alleleSlice * i, alleleSlice * i + alleleSlice)

	minAlleleValue = 1.0;
	maxAlleleValue = 0.0;

	//DONE alleleSlice is not worth using.
	alleles.reserve(numberOfAlleles);
	for (unsigned int i = 0; i < numberOfAlleles; ++i)
	{
		alleles.emplace_back(i);

		minAlleleValue = fmin(minAlleleValue, alleles.back().getValue());
		maxAlleleValue = fmax(maxAlleleValue, alleles.back().getValue());
	}
	//Randomizing the orders here due to the alleleSlice restriction.
	//If we just had ordered numbers, lower allele values would have lower orders and that is NOT what we want.
	//random_shuffle(alphabeticOrder.begin(), alphabeticOrder.end());
}

Locus::~Locus()
{
	
}

Locus::Locus(const Locus &other)
    : alleles(other.getAlleles()), minAlleleValue(other.getMinAlleleValue()),
	  maxAlleleValue(other.getMaxAlleleValue())
{
    
}

Locus& Locus::operator=(const Locus& other)
{
    if (this != &other) {
        alleles = other.alleles;
		minAlleleValue = other.minAlleleValue;
		maxAlleleValue = other.maxAlleleValue;
    }
    return *this;
}

const Allele* Locus::getAlleleRandomly() const 
{ 
	return &alleles.at(Random::randomIndex(alleles.size())); 
}

const std::vector<Allele>& Locus::getAlleles() const
{
	return alleles;
}

const PreciseDouble& Locus::getMinAlleleValue() const
{
	return minAlleleValue;
}

const PreciseDouble& Locus::getMaxAlleleValue() const
{
	return maxAlleleValue;
}

void Locus::modifyAllele(const size_t &affectedAllelesPosition, const PreciseDouble& timesAlleles)
{
	alleles[affectedAllelesPosition].modifyValue(timesAlleles);

	minAlleleValue = fmin(minAlleleValue, alleles[affectedAllelesPosition].getValue());
	maxAlleleValue = fmax(maxAlleleValue, alleles[affectedAllelesPosition].getValue());
}


template<class Archive>
void Locus::serialize(Archive & ar, const unsigned int)
{
	ar & alleles;

	ar & minAlleleValue;
	ar & maxAlleleValue;
}

// // Specialisation
template void Locus::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Locus::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Locus::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Locus::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
