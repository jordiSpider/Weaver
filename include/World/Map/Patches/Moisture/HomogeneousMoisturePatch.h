#ifndef HOMOGENEOUS_MOISTURE_PATCH_H_
#define HOMOGENEOUS_MOISTURE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/HomogeneousPatch.h"
#include "World/Map/Patches/Moisture/MoisturePatch.h"



class HomogeneousMoisturePatch : public MoisturePatch
{
public:
    HomogeneousMoisturePatch(const nlohmann::json &moisturePatchInfo, const std::string &filename);
    HomogeneousMoisturePatch(const unsigned int priority, const nlohmann::json &moisturePatchInfo, const std::string &filename);
    virtual ~HomogeneousMoisturePatch();
};

#endif /* HOMOGENEOUS_MOISTURE_PATCH_H_ */