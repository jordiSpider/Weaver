#ifndef CUBIC_RESOURCE_PATCH_H_
#define CUBIC_RESOURCE_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/CubicPatch.h"
#include "World/Map/Patches/Resource/ResourcePatch.h"



class CubicResourcePatch : public ResourcePatch
{
public:
    CubicResourcePatch(const nlohmann::json &resourcePatchInfo, ResourceSpecies* const species, const std::string &filename);
    virtual ~CubicResourcePatch();
};

#endif /* CUBIC_RESOURCE_PATCH_H_ */