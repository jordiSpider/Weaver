#include "App/Model/IBM/Maths/Dynamics/NonMassBased/CycleDynamics.h"


using namespace std;
using json = nlohmann::json;



CycleDynamics::CycleDynamics()
    : NonMassBasedDynamics()
{

}

CycleDynamics::CycleDynamics(const json &config)
    : NonMassBasedDynamics(), currentIndex(0u)
{
    for(const double& value : config["cycle"])
    {
        cycle.emplace_back(value);
    }
}

CycleDynamics::CycleDynamics(const json &config, const PreciseDouble& hyperVolume)
    : NonMassBasedDynamics(), currentIndex(0u)
{
    for(const double& value : config["cycle"])
    {
        cycle.emplace_back(value * hyperVolume);
    }
}

CycleDynamics::~CycleDynamics()
{

}


const std::vector<PreciseDouble>& CycleDynamics::getCycle() const
{
    return cycle;
}

PreciseDouble CycleDynamics::getInitialValue() const
{
    return cycle[currentIndex];
}


PreciseDouble CycleDynamics::getValue(const PreciseDouble&) const
{
    return cycle[currentIndex];
}


void CycleDynamics::update()
{
	currentIndex++;
	if(currentIndex >= cycle.size())
	{
		currentIndex = 0u;
	}
}

std::string CycleDynamics::showInfo(const std::string& indentation) const
{
    std::ostringstream info;

    info << indentation << "- Type = Cycle\n";

    info << indentation << "- Cycle values = ";

    info << "[" << cycle[0];

    for(unsigned int i = 1; i < cycle.size(); i++)
    {
        info << ", " << cycle[i];
    }

    info << "]";

    return info.str();
}



BOOST_CLASS_EXPORT(CycleDynamics)

template<class Archive>
void CycleDynamics::serialize(Archive & ar, const unsigned int) {
    ar & boost::serialization::base_object<NonMassBasedDynamics>(*this);

	ar & cycle;

	ar & currentIndex;
}

// Specialisation
template void CycleDynamics::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CycleDynamics::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CycleDynamics::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CycleDynamics::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
