
#include "IBM/World/Map/TerrainCells/Moisture/UpdateMethodType/CycleMoisture.h"

using namespace std;
using json = nlohmann::json;


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
    ar & relativeHumidityCycle;
}
