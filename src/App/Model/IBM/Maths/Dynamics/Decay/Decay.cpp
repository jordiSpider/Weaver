#include "App/Model/IBM/Maths/Dynamics/Decay/Decay.h"

#include "App/Model/IBM/Maths/Dynamics/Decay/NonDecay.h"
#include "App/Model/IBM/Maths/Dynamics/Decay/LinealDecay.h"


using namespace std;
using json = nlohmann::json;





Decay* Decay::createInstance(const json &config)
{
	switch(EnumClass<Type>::stringToEnumValue(config["decayType"].get<string>())) {
		case Type::NonDecay: {
			return new NonDecay();
			break;
		}
        case Type::Lineal: {
			return new LinealDecay(config["linealDecayParams"]);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}

Decay* Decay::createInstance(const json &config, const PreciseDouble& hyperVolume)
{
	switch(EnumClass<Type>::stringToEnumValue(config["decayType"].get<string>())) {
		case Type::NonDecay: {
			return new NonDecay();
			break;
		}
        case Type::Lineal: {
			return new LinealDecay(config["linealDecayParams"], hyperVolume);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}



Decay::Decay()
{

}

Decay::~Decay()
{
	
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(Decay)

template <class Archive>
void Decay::serialize(Archive &, const unsigned int) 
{
	
}

// Specialisation
template void Decay::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Decay::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Decay::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Decay::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
