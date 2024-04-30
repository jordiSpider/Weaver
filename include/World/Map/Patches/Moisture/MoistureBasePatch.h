#ifndef MOISTURE_BASE_PATCH_H_
#define MOISTURE_BASE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Moisture/HomogeneousMoisturePatch.h"



class MoistureBasePatch : public HomogeneousMoisturePatch
{
public:
    MoistureBasePatch(const nlohmann::json &moisturePatchInfo);
    virtual ~MoistureBasePatch();
};

#endif /* MOISTURE_BASE_PATCH_H_ */