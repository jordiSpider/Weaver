
#include "Correlosome.h"

using namespace std;


// Colocar un warning indicando que el vector que se le pasa se asigna directamente, sin realizar copia
// Por lo tanto el vector que se le pase debe haver sido creado previamente, y ya la clase Correlosome
// se encargará de destruirlo.
Correlosome::Correlosome(vector<Allele*>* correlosomeAlleles)
{
    this->alleles = correlosomeAlleles;
}

Correlosome::Correlosome(const unsigned int& numberOfLociPerChromosome)
{
	alleles = new vector<Allele*>(numberOfLociPerChromosome);
}

Correlosome::Correlosome(Correlosome* otherCorrelosome)
{
	this->alleles = new vector<Allele*>(otherCorrelosome->getAlleles()->size());
	for(size_t i = 0; i < otherCorrelosome->getAlleles()->size(); i++)
	{
		this->alleles->at(i) = otherCorrelosome->getAlleles()->at(i)->clone();
	}
}

Correlosome::~Correlosome()
{
	for (auto it = alleles->begin(); it != alleles->end(); ++it)
	{
		delete (*it);
	}
	
	alleles->clear();
	delete alleles; //Dinosaurs - reversed in dinosurs
}

void Correlosome::setAllele(const Allele* allele, int lociPosition)
{
	delete this->alleles->at(lociPosition);
	this->alleles->at(lociPosition) = allele->clone();
}

Correlosome* Correlosome::clone()
{
	return new Correlosome(this);
}

//TODO COMPROBAR QUE SE IMPRIMAN BIEN
ostream& operator<<(ostream& os, Correlosome& correlosome)
{
	for (size_t i = 0; i < correlosome.alleles->size(); ++i)
	{
		os << correlosome.alleles->at(i) << "\t";
	}
	return os;
}
