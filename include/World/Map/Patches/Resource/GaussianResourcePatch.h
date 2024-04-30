#ifndef GAUSSIAN_RESOURCE_PATCH_H_
#define GAUSSIAN_RESOURCE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/GaussianPatch.h"
#include "World/Map/Patches/Resource/ResourcePatch.h"



class GaussianResourcePatch : public ResourcePatch
{
public:
    GaussianResourcePatch(const nlohmann::json &resourcePatchInfo, ResourceSpecies* const species, const std::string &filename);
    virtual ~GaussianResourcePatch();
};

#endif /* GAUSSIAN_RESOURCE_PATCH_H_ */