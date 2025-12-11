#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Chromosome.h"

using namespace std;


Chromosome::Chromosome()
{

}

Chromosome::Chromosome(const size_t numberOfLociPerChromosome)
{
	alleles.reserve(numberOfLociPerChromosome);
	//The vector is ready for the new chromosomeAlleles to be inserted one by one.
	//This entire process will not create new Alleles, though.
}

Chromosome::Chromosome(const Chromosome &otherChromosome)
{
	alleles.reserve(otherChromosome.size());
	for(size_t i = 0; i < otherChromosome.size(); i++)
	{
		pushAllele(otherChromosome.getAllele(i));
	}
}

Chromosome* Chromosome::clone() const 
{ 
	return new Chromosome(*this); 
}

Chromosome::~Chromosome()
{
	// Nothing to delete dynamically
}

const Allele* Chromosome::getAllele(const size_t lociPosition) const 
{ 
	return alleles.at(lociPosition); 
}

size_t Chromosome::size() const 
{ 
	return alleles.size(); 
}

void Chromosome::setAllele(const Allele* const allele, const size_t lociPosition) 
{ 
	alleles.at(lociPosition) = allele; 
}

void Chromosome::pushAllele(const Allele* const allele) 
{ 
	alleles.emplace_back(allele); 
}

void Chromosome::swapAlleles(const size_t lociPosition, Chromosome* const otherChromosome, const size_t otherLociPosition)
{
	const Allele* const alleleToSwap = getAllele(lociPosition);
	setAllele(otherChromosome->getAllele(otherLociPosition), lociPosition);
	otherChromosome->setAllele(alleleToSwap, otherLociPosition);
}

//TODO COMPROBAR QUE SE IMPRIMAN BIEN
ostream& operator<<(ostream& os, const Chromosome &chromosome)
{
	os << *chromosome.getAllele(0);

	for(size_t i = 1; i < chromosome.size(); ++i)
	{
		os << "\t" << *chromosome.getAllele(i);
	}

	return os;
}



BOOST_CLASS_EXPORT(Chromosome)

template<class Archive>
void Chromosome::serialize(Archive & ar, const unsigned int) {
	ar & alleles;
}

// Specialisation
template void Chromosome::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Chromosome::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Chromosome::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Chromosome::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
