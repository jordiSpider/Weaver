#ifndef HOMOGENEOUS_RESOURCE_PATCH_H_
#define HOMOGENEOUS_RESOURCE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/HomogeneousPatch.h"
#include "World/Map/Patches/Resource/ResourcePatch.h"



class HomogeneousResourcePatch : public ResourcePatch
{
protected:
    HomogeneousResourcePatch(const unsigned int priority, const nlohmann::json &resourcePatchInfo, ResourceSpecies* const species, const std::string &filename);

public:
    HomogeneousResourcePatch(const nlohmann::json &resourcePatchInfo, ResourceSpecies* const species, const std::string &filename);
    virtual ~HomogeneousResourcePatch();
};

#endif /* HOMOGENEOUS_RESOURCE_PATCH_H_ */