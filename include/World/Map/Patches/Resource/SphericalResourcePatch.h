#ifndef SPHERICAL_RESOURCE_PATCH_H_
#define SPHERICAL_RESOURCE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/SphericalPatch.h"
#include "World/Map/Patches/Resource/ResourcePatch.h"



class SphericalResourcePatch : public ResourcePatch
{
public:
    SphericalResourcePatch(const nlohmann::json &resourcePatchInfo, ResourceSpecies* const species, const std::string &filename);
    virtual ~SphericalResourcePatch();
};

#endif /* SPHERICAL_RESOURCE_PATCH_H_ */