#ifndef CUBIC_MOISTURE_PATCH_H_
#define CUBIC_MOISTURE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/CubicPatch.h"
#include "World/Map/Patches/Moisture/MoisturePatch.h"



class CubicMoisturePatch : public MoisturePatch
{
public:
    CubicMoisturePatch(const nlohmann::json &moisturePatchInfo, const std::string &filename);
    virtual ~CubicMoisturePatch();
};

#endif /* CUBIC_MOISTURE_PATCH_H_ */