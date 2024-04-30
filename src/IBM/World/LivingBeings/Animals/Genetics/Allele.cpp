/*
 * Allele.cpp
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#include "IBM/World/LivingBeings/Animals/Genetics/Allele.h"

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

template<class Archive>
void Allele::serialize(Archive & ar, const unsigned int version) {
	ar & alleleId; 
	ar & id; 
	ar & value; 
	ar & alphabeticOrder;
}
