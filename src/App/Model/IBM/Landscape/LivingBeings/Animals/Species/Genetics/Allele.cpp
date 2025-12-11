#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Allele.h"

using namespace std;



Allele::Allele()
	: value(0.0), alphabeticOrder(0)
{
	
}

Allele::Allele(const unsigned int alphabeticOrder) 
	: value(Random::randomUniform()), alphabeticOrder(alphabeticOrder) 
{

}

Allele::~Allele() {
	// Nothing to delete dynamically
}

const PreciseDouble& Allele::getValue() const 
{ 
	return value;
}

void Allele::modifyValue(const PreciseDouble& timesAlleles)
{
	value *= timesAlleles;
}

unsigned int Allele::getAlphabeticOrder() const 
{ 
	return alphabeticOrder; 
}

//TODO Agregar el ID en esta impresion
ostream& operator<<(ostream& os, const Allele& allele)
{
	os << allele.getValue() << "\t" << allele.getAlphabeticOrder();
	return os;
}


BOOST_CLASS_EXPORT(Allele)

template<class Archive>
void Allele::serialize(Archive & ar, const unsigned int)
{
	ar & value; 
	ar & alphabeticOrder;
}

// Specialisation
template void Allele::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Allele::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Allele::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Allele::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
