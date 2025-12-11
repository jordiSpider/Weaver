#include "App/Model/IBM/Maths/Dynamics/NonMassBasedDynamics.h"

#include "App/Model/IBM/Maths/Dynamics/NonMassBased/CycleDynamics.h"
#include "App/Model/IBM/Maths/Dynamics/NonMassBased/PulseDynamics.h"


using namespace std;
using json = nlohmann::json;




NonMassBasedDynamics* NonMassBasedDynamics::createInstance(const json &config, const PreciseDouble& timeStepsPerDay)
{
	switch(EnumClass<Type>::stringToEnumValue(config["dynamicsType"].get<string>())) {
		case Type::Cycle: {
			return new CycleDynamics(config["cycleParams"]);
			break;
		}
        case Type::Pulse: {
			return new PulseDynamics(config["pulseParams"], timeStepsPerDay);
			break;
		}
		case Type::GrowthRate:
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}



NonMassBasedDynamics::NonMassBasedDynamics()
	: Dynamics()
{

}

NonMassBasedDynamics::~NonMassBasedDynamics()
{
	
}


PreciseDouble NonMassBasedDynamics::getValue(const PreciseDouble& baseValue, const PreciseDouble&, const Temperature&, const PreciseDouble&, const PreciseDouble&, const PreciseDouble&) const
{
	return getValue(baseValue);
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(NonMassBasedDynamics)

template <class Archive>
void NonMassBasedDynamics::serialize(Archive &ar, const unsigned int) 
{
	ar & boost::serialization::base_object<Dynamics>(*this);
}

// Specialisation
template void NonMassBasedDynamics::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void NonMassBasedDynamics::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void NonMassBasedDynamics::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void NonMassBasedDynamics::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
