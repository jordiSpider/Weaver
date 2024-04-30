
#include "IBM/World/Map/TerrainCells/Moisture/CycleMoisture.h"

using namespace std;
using json = nlohmann::json;


CycleMoisture::CycleMoisture()
    : ExtendedMoisture()
{

}

CycleMoisture::CycleMoisture(const json &moistureInfo)
    : ExtendedMoisture(moistureInfo),
      relativeHumidityCycle(moistureInfo["relativeHumidityUpdateMethod"]["cycleParams"]["relativeHumidityCycle"])
{

}

CycleMoisture::CycleMoisture(const CycleMoisture &other)
    : ExtendedMoisture(other),
      relativeHumidityCycle(other.getRelativeHumidityCycle())
{

}

CycleMoisture::~CycleMoisture()
{

}

CycleMoisture* CycleMoisture::clone() const
{
    return new CycleMoisture(*this);
}

const vector<double>& CycleMoisture::getRelativeHumidityCycle() const
{
    return relativeHumidityCycle;
}

void CycleMoisture::refreshRelativeHumidity(const unsigned int timeStep)
{
    setMoisture(getHumidityOnTimeStep(timeStep));
}

const double& CycleMoisture::getHumidityOnTimeStep(const unsigned int timeStep) const
{
    return getRelativeHumidityCycle().at(timeStep%getRelativeHumidityCycle().size());
}

const ExtendedMoisture::Type CycleMoisture::getExtendedMoistureType() const
{
    return ExtendedMoisture::Type::Cycle;
}

const string CycleMoisture::showMoistureInfo() const
{
    string relativeHumidityCycleString = fmt::format(
        "[{}", getRelativeHumidityCycle()[0]
    );

    for(unsigned int i = 1; i < getRelativeHumidityCycle().size(); i++)
    {
        relativeHumidityCycleString += fmt::format(", {}", getRelativeHumidityCycle()[i]);
    }

    relativeHumidityCycleString += "]";

	return fmt::format(
		"{}\n   - Relative humidity cycle = {}", 
        ExtendedMoisture::showMoistureInfo(), relativeHumidityCycleString
	);
}

template <class Archive>
void CycleMoisture::serialize(Archive &ar, const unsigned int version) {
    ar & boost::serialization::base_object<ExtendedMoisture>(*this);
    
    ar & relativeHumidityCycle;
}

// Specialisation
template void CycleMoisture::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CycleMoisture::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CycleMoisture::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CycleMoisture::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
