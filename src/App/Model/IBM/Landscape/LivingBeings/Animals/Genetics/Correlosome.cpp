
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Correlosome.h"

using namespace std;


Correlosome::Correlosome(const size_t numberOfLociPerChromosome, const bool onlyReserve)
{
	if(onlyReserve) {
		alleles.reserve(numberOfLociPerChromosome);
	}
	else {
		alleles.resize(numberOfLociPerChromosome);
	}
}

Correlosome::~Correlosome()
{
	// Nothing to delete dynamically
}

const Allele* Correlosome::getAllele(const size_t lociPosition) const 
{ 
	return alleles.at(lociPosition); 
}

size_t Correlosome::size() const 
{ 
	return alleles.size(); 
}

void Correlosome::setAllele(const Allele* allele, const size_t lociPosition) 
{ 
	alleles.at(lociPosition) = allele; 
}

void Correlosome::pushAllele(const Allele* allele) 
{ 
	alleles.emplace_back(allele); 
} 

//TODO COMPROBAR QUE SE IMPRIMAN BIEN
ostream& operator<<(ostream& os, const Correlosome& correlosome)
{
	for (size_t i = 0; i < correlosome.size(); ++i)
	{
		os << correlosome.getAllele(i) << "\t";
	}
	return os;
}
