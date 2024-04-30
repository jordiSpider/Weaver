/*
 * Allele.cpp
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#include "IBM/World/LivingBeings/Animals/Genetics/Allele.h"

using namespace std;


unsigned int Allele::alleleId = 0;

Allele::Allele()
	: id(0), value(0.0), alphabeticOrder(0)
{
	alleleId++;
}

Allele::Allele(const double &value, const unsigned int alphabeticOrder) 
	: id(alleleId++), value(value), alphabeticOrder(alphabeticOrder) 
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



namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, Allele* &allelePtr, const unsigned int version) {
            Allele allele;
            
			// Deserialization
            if (Archive::is_loading::value) {
                ar & allele;

                allelePtr = new Allele(allele);
            } // Serialization
            else {
                allele = *allelePtr;

                ar & allele;
            }
        }

		// Specialisation
		template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, Allele*&, const unsigned int);
		template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, Allele*&, const unsigned int);

		template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, Allele*&, const unsigned int);
		template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, Allele*&, const unsigned int);
    }
}
