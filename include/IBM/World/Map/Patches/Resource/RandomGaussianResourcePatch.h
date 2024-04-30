#ifndef RANDOM_GAUSSIAN_RESOURCE_PATCH_H_
#define RANDOM_GAUSSIAN_RESOURCE_PATCH_H_

#include <nlohmann/json.hpp>

#include "IBM/World/Map/Patches/Forms/RandomGaussianPatch.h"
#include "IBM/World/Map/Patches/Resource/ResourcePatch.h"



class RandomGaussianResourcePatch : public ResourcePatch
{
public:
    RandomGaussianResourcePatch(const nlohmann::json &resourcePatchInfo, ResourceSpecies* const species, const std::string &filename);
    virtual ~RandomGaussianResourcePatch();
};

#endif /* RANDOM_GAUSSIAN_RESOURCE_PATCH_H_ */