#ifndef SPHERICAL_MOISTURE_PATCH_H_
#define SPHERICAL_MOISTURE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/SphericalPatch.h"
#include "World/Map/Patches/Moisture/MoisturePatch.h"



class SphericalMoisturePatch : public MoisturePatch
{
public:
    SphericalMoisturePatch(const nlohmann::json &moisturePatchInfo, const std::string &filename);
    virtual ~SphericalMoisturePatch();
};

#endif /* SPHERICAL_MOISTURE_PATCH_H_ */