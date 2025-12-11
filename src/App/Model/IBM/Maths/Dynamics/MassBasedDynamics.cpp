#include "App/Model/IBM/Maths/Dynamics/MassBasedDynamics.h"


using namespace std;
using json = nlohmann::json;




MassBasedDynamics::MassBasedDynamics()
	: Dynamics()
{

}

MassBasedDynamics::~MassBasedDynamics()
{
	
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(MassBasedDynamics)

template <class Archive>
void MassBasedDynamics::serialize(Archive &ar, const unsigned int) 
{
	ar & boost::serialization::base_object<Dynamics>(*this);
}

// Specialisation
template void MassBasedDynamics::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void MassBasedDynamics::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void MassBasedDynamics::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void MassBasedDynamics::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
