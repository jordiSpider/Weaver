/*
 * Allele.cpp
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#include "Allele.h"

using namespace std;


unsigned int Allele::alleleId = 0;

Allele::Allele(const double &value, const unsigned int alphabeticOrder) 
	: id(alleleId++), value(value), alphabeticOrder(alphabeticOrder) {}

Allele::~Allele() {
	// Nothing to delete dynamically
}

//TODO Agregar el ID en esta impresion
ostream& operator<<(ostream& os, const Allele& allele)
{
	os << allele.getValue();
	return os;
}
