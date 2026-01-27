#include "App/Model/IBM/Maths/Dynamics/Dynamics.h"

#include "App/Model/IBM/Maths/Dynamics/NonMassBased/CycleDynamics.h"
#include "App/Model/IBM/Maths/Dynamics/NonMassBased/PulseDynamics.h"
#include "App/Model/IBM/Maths/Dynamics/MassBased/GrowthRateDynamics.h"


using namespace std;
using json = nlohmann::json;



Dynamics* Dynamics::createInstance(const json &config, const PreciseDouble& timeStepsPerDay)
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
		case Type::GrowthRate: {
			throwLineInfoException("Error: For the 'GrowthRate' type, use the 'createInstanceForResource' method.");
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}

Dynamics* Dynamics::createInstanceForResource(const json &config, const PreciseDouble& scaleMass, const WetMass& cellMass, const PreciseDouble& timeStepsPerDay, const PreciseDouble& hyperVolume, const WetMass& resourceMaximumCapacity)
{
    switch(EnumClass<Type>::stringToEnumValue(config["dynamicsType"].get<string>())) {
		case Type::Cycle: {
			return new CycleDynamics(config["cycleParams"], hyperVolume);
			break;
		}
        case Type::Pulse: {
			return new PulseDynamics(config["pulseParams"], timeStepsPerDay, hyperVolume);
			break;
		}
		case Type::GrowthRate: {
			return new GrowthRateDynamics(config["growthRateParams"], scaleMass, cellMass, hyperVolume, resourceMaximumCapacity);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}



Dynamics::Dynamics()
{

}

Dynamics::~Dynamics()
{
	
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(Dynamics)

template <class Archive>
void Dynamics::serialize(Archive &, const unsigned int) 
{
	
}

// Specialisation
template void Dynamics::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Dynamics::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Dynamics::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Dynamics::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
