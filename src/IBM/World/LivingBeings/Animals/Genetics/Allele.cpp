/*
 * Allele.cpp
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#include "IBM/World/LivingBeings/Animals/Genetics/Allele.h"

using namespace std;



Allele::Allele()
	: id(0), value(0.0), alphabeticOrder(0)
{
	
}

Allele::Allele(const unsigned int alleleId, const unsigned int alphabeticOrder) 
	: id(alleleId), value(Random::randomUniform()), alphabeticOrder(alphabeticOrder) 
{

}

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
void Allele::serialize(Archive & ar, const unsigned int version)
{
	ar & id; 
	ar & value; 
	ar & alphabeticOrder;
}

// Specialisation
template void Allele::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Allele::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Allele::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Allele::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
