#include "IBM/World/Map/TerrainCells/Moisture/Factory/MoistureFactory.h"


using namespace std;
using json = nlohmann::json;



unique_ptr<ExtendedMoisture> MoistureFactory::createInstance(const json &moistureInfo)
{
    switch(ExtendedMoisture::Type::stringToEnumValue(moistureInfo["relativeHumidityUpdateMethod"]["updateMethodType"])) {
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

template <class Archive>
void MoistureFactory::serialize(Archive &ar, const unsigned int version) {

}
