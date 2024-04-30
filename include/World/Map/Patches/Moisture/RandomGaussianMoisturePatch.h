#ifndef RANDOM_GAUSSIAN_MOISTURE_PATCH_H_
#define RANDOM_GAUSSIAN_MOISTURE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/RandomGaussianPatch.h"
#include "World/Map/Patches/Moisture/MoisturePatch.h"



class RandomGaussianMoisturePatch : public MoisturePatch
{
public:
    RandomGaussianMoisturePatch(const nlohmann::json &moisturePatchInfo, const std::string &filename);
    virtual ~RandomGaussianMoisturePatch();
};

#endif /* RANDOM_GAUSSIAN_MOISTURE_PATCH_H_ */