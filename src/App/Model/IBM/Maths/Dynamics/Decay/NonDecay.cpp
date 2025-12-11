#include "App/Model/IBM/Maths/Dynamics/Decay/NonDecay.h"


using namespace std;






NonDecay::NonDecay()
    : Decay()
{

}

NonDecay::~NonDecay()
{
	
}


PreciseDouble NonDecay::apply(const PreciseDouble& currentValue) const
{
    return currentValue;
}

std::string NonDecay::showInfo(const std::string& indentation) const
{
    ostringstream info;

    info << indentation << "- Disabled";

    return info.str();
}


BOOST_CLASS_EXPORT(NonDecay)

template <class Archive>
void NonDecay::serialize(Archive &ar, const unsigned int) 
{
	ar & boost::serialization::base_object<Decay>(*this);
}

// Specialisation
template void NonDecay::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void NonDecay::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void NonDecay::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void NonDecay::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
