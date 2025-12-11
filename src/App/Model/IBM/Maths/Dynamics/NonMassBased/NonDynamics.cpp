#include "App/Model/IBM/Maths/Dynamics/NonMassBased/NonDynamics.h"


using namespace std;
using json = nlohmann::json;



NonDynamics::NonDynamics()
    : NonMassBasedDynamics()
{

}

NonDynamics::~NonDynamics()
{

}


PreciseDouble NonDynamics::getInitialValue() const
{
    return 0.0;
}


PreciseDouble NonDynamics::getValue(const PreciseDouble&) const
{
    return 0.0;
}


void NonDynamics::update()
{
	
}

std::string NonDynamics::showInfo(const std::string& indentation) const
{
    std::ostringstream info;

    info << indentation << "- Type = Non Dynamics";

    return info.str();
}



BOOST_CLASS_EXPORT(NonDynamics)

template<class Archive>
void NonDynamics::serialize(Archive & ar, const unsigned int) {
    ar & boost::serialization::base_object<NonMassBasedDynamics>(*this);
}

// Specialisation
template void NonDynamics::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void NonDynamics::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void NonDynamics::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void NonDynamics::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
