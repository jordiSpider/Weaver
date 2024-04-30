#ifndef CUBIC_OBSTACLE_PATCH_H_
#define CUBIC_OBSTACLE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/CubicPatch.h"
#include "World/Map/Patches/Obstacle/ObstaclePatch.h"



class CubicObstaclePatch : public ObstaclePatch
{
public:
    CubicObstaclePatch(const nlohmann::json &obstaclePatchInfo, const std::string &filename);
    virtual ~CubicObstaclePatch();
};

#endif /* CUBIC_OBSTACLE_PATCH_H_ */