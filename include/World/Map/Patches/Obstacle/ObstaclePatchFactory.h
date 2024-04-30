#ifndef OBSTACLE_PATCH_FACTORY_H_
#define OBSTACLE_PATCH_FACTORY_H_


#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "Exceptions/LineInfoException.h"
#include "World/Map/Patches/PatchFactory.h"
#include "World/Map/Patches/Obstacle/ObstaclePatch.h"
#include "World/Map/Patches/Obstacle/CubicObstaclePatch.h"



class ObstaclePatchFactory : public PatchFactory
{
public:
	static std::unique_ptr<const ObstaclePatch> createInstance(const nlohmann::json &obstaclePatchInfo, const std::string &filename);
};

#endif // OBSTACLE_PATCH_FACTORY_H_
