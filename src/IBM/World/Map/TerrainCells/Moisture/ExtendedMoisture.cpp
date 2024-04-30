
#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"

#include "IBM/World/Map/TerrainCells/Moisture/CycleMoisture.h"
#include "IBM/World/Map/TerrainCells/Moisture/RainEventAndDecayOverTimeMoisture.h"

using namespace std;
using json = nlohmann::json;



unique_ptr<ExtendedMoisture> ExtendedMoisture::createInstance(const json &moistureInfo)
{
    switch(EnumClass<ExtendedMoisture::Type>::stringToEnumValue(moistureInfo["relativeHumidityUpdateMethod"]["updateMethodType"])) {
		case ExtendedMoisture::Type::Cycle: {
			return make_unique<CycleMoisture>(moistureInfo);
			break;
		}
        case ExtendedMoisture::Type::RainEventAndDecayOverTime: {
			return make_unique<RainEventAndDecayOverTimeMoisture>(moistureInfo);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}

unique_ptr<ExtendedMoisture> ExtendedMoisture::createInstance(const ExtendedMoisture::Type &extendedMoistureType)
{
    switch(extendedMoistureType) {
		case ExtendedMoisture::Type::Cycle: {
			return make_unique<CycleMoisture>();
			break;
		}
        case ExtendedMoisture::Type::RainEventAndDecayOverTime: {
			return make_unique<RainEventAndDecayOverTimeMoisture>();
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}


ExtendedMoisture::ExtendedMoisture()
    : MoistureInterface()
{

}

ExtendedMoisture::ExtendedMoisture(const json &moistureInfo)
    : MoistureInterface(),
	  inEnemyFreeSpace(moistureInfo["inEnemyFreeSpace"]), 
      inCompetitorFreeSpace(moistureInfo["inCompetitorFreeSpace"]),
      temperature(0.0), 
      moisture(0.0),
      maximumResourceCapacityDensity(moistureInfo["maximumResourceCapacityDensity"]),
      temperatureCycle(obtainTemperatureCycle(moistureInfo["temperatureCycle"]))
{

}

ExtendedMoisture::ExtendedMoisture(const ExtendedMoisture& other)
    : MoistureInterface(),
	  inEnemyFreeSpace(other.isInEnemyFreeSpace()), 
      inCompetitorFreeSpace(other.isInCompetitorFreeSpace()),
      temperature(other.getTemperature()), 
      moisture(other.getMoisture()),
      maximumResourceCapacityDensity(other.getMaximumResourceCapacityDensity()),
      temperatureCycle(other.getTemperatureCycle())
{

}

ExtendedMoisture::~ExtendedMoisture()
{

}

vector<Temperature> ExtendedMoisture::obtainTemperatureCycle(const json &temperatureCycleValues)
{
    vector<Temperature> temperatureCycle;
    transform(temperatureCycleValues.begin(), temperatureCycleValues.end(), back_inserter(temperatureCycle), [](double temperatureValue) {
        return Temperature(temperatureValue);
    });

    return temperatureCycle;
}

void ExtendedMoisture::setTemperature(const Temperature &newTemperature)
{
    temperature = newTemperature;
}

void ExtendedMoisture::setMoisture(const double &newMoisture)
{
    moisture = newMoisture;
}

void ExtendedMoisture::setInEnemyFreeSpace(const bool newInEnemyFreeSpace)
{
    inEnemyFreeSpace = newInEnemyFreeSpace;
}

void ExtendedMoisture::setInCompetitorFreeSpace(const bool newInCompetitorFreeSpace)
{
    inCompetitorFreeSpace = newInCompetitorFreeSpace;
}

const vector<Temperature>& ExtendedMoisture::getTemperatureCycle() const
{
    return temperatureCycle;
}

const double& ExtendedMoisture::getMaximumResourceCapacityDensity() const
{
    return maximumResourceCapacityDensity;
}

const Temperature& ExtendedMoisture::getTemperature() const
{
    return temperature;
}

const double& ExtendedMoisture::getMoisture() const
{
    return moisture;
}

const bool ExtendedMoisture::isInEnemyFreeSpace() const
{
    return inEnemyFreeSpace;
}

const bool ExtendedMoisture::isInCompetitorFreeSpace() const
{
    return inCompetitorFreeSpace;
}

void ExtendedMoisture::updateTemperature()
{
    // Not to implement
}

void ExtendedMoisture::updateRelativeHumidity()
{
    // Not to implement
}

void ExtendedMoisture::updateInfo()
{
    throwLineInfoException("Extended moisture is not updated");
}

const Temperature& ExtendedMoisture::getTemperatureOnTimeStep(const unsigned int timeStep) const
{
    return getTemperatureCycle().at(timeStep%getTemperatureCycle().size());
}

const string ExtendedMoisture::showMoistureInfo() const
{
	string temperatureCycleString = fmt::format(
        "[{}", getTemperatureCycle()[0].getTemperatureCelsius()
    );

    for(unsigned int i = 1; i < getTemperatureCycle().size(); i++)
    {
        temperatureCycleString += fmt::format(", {}", getTemperatureCycle()[i].getTemperatureCelsius());
    }

    temperatureCycleString += "]";

	return fmt::format(
		" - Moisture parameters:\n   - Temperature cycle = {}\n   - Maximum resource capacity density = {}", 
		temperatureCycleString, getMaximumResourceCapacityDensity()
	);
}

void ExtendedMoisture::refreshValue(const unsigned int timeStep)
{
    refreshTemperature(timeStep);
    refreshRelativeHumidity(timeStep);
}

void ExtendedMoisture::refreshTemperature(const unsigned int timeStep)
{
    setTemperature(getTemperatureOnTimeStep(timeStep));
}

const Temperature& ExtendedMoisture::getRandomTemperature() const
{
    return temperatureCycle.at(Random::randomIndex(getTemperatureCycle().size()));
}

template <class Archive>
void ExtendedMoisture::serialize(Archive &ar, const unsigned int version) {
    ar & boost::serialization::base_object<MoistureInterface>(*this);

    ar & inEnemyFreeSpace;
    ar & inCompetitorFreeSpace;
    ar & temperature;
    ar & moisture;
    ar & maximumResourceCapacityDensity;
    ar & temperatureCycle;
}

// Specialisation
template void ExtendedMoisture::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ExtendedMoisture::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ExtendedMoisture::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ExtendedMoisture::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);



namespace boost {
    namespace serialization {
        template <class Archive>
        void serialize(Archive &ar, ExtendedMoisture* &extendedMoisturePtr, const unsigned int version, vector<ExtendedMoisture*>& appliedMoisture) {
            ExtendedMoisture::Type extendedMoistureType;
            
            // For loading
            if(Archive::is_loading::value) 
            {
                ar & extendedMoistureType;
                extendedMoisturePtr = ExtendedMoisture::createInstance(extendedMoistureType).release();

                appliedMoisture.push_back(extendedMoisturePtr);
            }
            else
            {
                extendedMoistureType = extendedMoisturePtr->getExtendedMoistureType();
                ar & extendedMoistureType;
            }

            switch(extendedMoistureType) {
                case ExtendedMoisture::Type::Cycle: {
                    static_cast<CycleMoisture*>(extendedMoisturePtr)->serialize(ar, version);
                    break;
                }
                case ExtendedMoisture::Type::RainEventAndDecayOverTime: {
                    static_cast<RainEventAndDecayOverTimeMoisture*>(extendedMoisturePtr)->serialize(ar, version);
                    break;
                }
                default: {
                    throwLineInfoException("Default case");
                    break;
                }
            }
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, ExtendedMoisture*&, const unsigned int, vector<ExtendedMoisture*>&);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, ExtendedMoisture*&, const unsigned int, vector<ExtendedMoisture*>&);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, ExtendedMoisture*&, const unsigned int, vector<ExtendedMoisture*>&);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, ExtendedMoisture*&, const unsigned int, vector<ExtendedMoisture*>&);
    }
}
