#ifndef GAUSSIAN_MOISTURE_PATCH_H_
#define GAUSSIAN_MOISTURE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/GaussianPatch.h"
#include "World/Map/Patches/Moisture/MoisturePatch.h"



class GaussianMoisturePatch : public MoisturePatch
{
public:
    GaussianMoisturePatch(const nlohmann::json &moisturePatchInfo, const std::string &filename);
    virtual ~GaussianMoisturePatch();
};

#endif /* GAUSSIAN_MOISTURE_PATCH_H_ */