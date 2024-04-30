
#include "Correlosome.h"

using namespace std;


Correlosome::Correlosome(const unsigned int numberOfLociPerChromosome, const bool onlyReserve)
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

//TODO COMPROBAR QUE SE IMPRIMAN BIEN
ostream& operator<<(ostream& os, const Correlosome& correlosome)
{
	for (size_t i = 0; i < correlosome.size(); ++i)
	{
		os << correlosome.getAllele(i) << "\t";
	}
	return os;
}
