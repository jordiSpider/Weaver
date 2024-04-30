#ifndef MOISTURE_FACTORY_H_
#define MOISTURE_FACTORY_H_


#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "Exceptions/LineInfoException.h"
#include "World/Map/TerrainCells/Moisture/ExtendedMoisture.h"
#include "World/Map/TerrainCells/Moisture/UpdateMethodType/CycleMoisture.h"
#include "World/Map/TerrainCells/Moisture/UpdateMethodType/RainEventAndDecayOverTimeMoisture.h"



class MoistureFactory
{
public:
	static std::unique_ptr<ExtendedMoisture> createInstance(const nlohmann::json &moistureInfo);
};

#endif // MOISTURE_FACTORY_H_
