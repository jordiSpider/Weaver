#include "App/Model/IBM/Maths/Dynamics/Decay/LinealDecay.h"


using namespace std;
using json = nlohmann::json;





LinealDecay::LinealDecay()
    : Decay()
{

}

LinealDecay::LinealDecay(const json &config)
    : Decay(), decayOverTime(config["decayOverTime"].get<double>())
{

}

LinealDecay::LinealDecay(const json &config, const PreciseDouble& hyperVolume)
    : Decay(), decayOverTime(config["decayOverTime"].get<double>() * hyperVolume)
{

}

LinealDecay::~LinealDecay()
{
	
}


PreciseDouble LinealDecay::apply(const PreciseDouble& currentValue) const
{
    return currentValue - decayOverTime;;
}

std::string LinealDecay::showInfo(const std::string& indentation) const
{
    ostringstream info;

    info << indentation << "- Decay over time = " << decayOverTime;

    return info.str();
}


BOOST_CLASS_EXPORT(LinealDecay)

template <class Archive>
void LinealDecay::serialize(Archive &ar, const unsigned int) 
{
	ar & boost::serialization::base_object<Decay>(*this);

    ar & decayOverTime;
}

// Specialisation
template void LinealDecay::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void LinealDecay::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void LinealDecay::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void LinealDecay::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
