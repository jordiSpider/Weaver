#ifndef MOISTURE_PATCH_FACTORY_H_
#define MOISTURE_PATCH_FACTORY_H_


#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "Exceptions/LineInfoException.h"
#include "World/Map/Patches/PatchFactory.h"
#include "World/Map/Patches/Moisture/MoisturePatch.h"
#include "World/Map/Patches/Moisture/CubicMoisturePatch.h"
#include "World/Map/Patches/Moisture/SphericalMoisturePatch.h"
#include "World/Map/Patches/Moisture/GaussianMoisturePatch.h"
#include "World/Map/Patches/Moisture/HomogeneousMoisturePatch.h"
#include "World/Map/Patches/Moisture/RandomGaussianMoisturePatch.h"
#include "World/Map/Patches/Moisture/MoistureBasePatch.h"



class MoisturePatchFactory : public PatchFactory
{
public:
	static std::unique_ptr<MoisturePatch> createInstance(const nlohmann::json &moisturePatchInfo, const std::string &filename);
	static std::unique_ptr<MoisturePatch> createInstance(const nlohmann::json &moisturePatchInfo);
};

#endif // MOISTURE_PATCH_FACTORY_H_
