#ifndef RESOURCE_PATCH_FACTORY_H_
#define RESOURCE_PATCH_FACTORY_H_


#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "Exceptions/LineInfoException.h"
#include "World/Map/Patches/PatchFactory.h"
#include "World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "World/Map/Patches/Resource/ResourcePatch.h"
#include "World/Map/Patches/Resource/CubicResourcePatch.h"
#include "World/Map/Patches/Resource/SphericalResourcePatch.h"
#include "World/Map/Patches/Resource/GaussianResourcePatch.h"
#include "World/Map/Patches/Resource/HomogeneousResourcePatch.h"
#include "World/Map/Patches/Resource/RandomGaussianResourcePatch.h"



class ResourcePatchFactory : public PatchFactory
{
public:
	static std::unique_ptr<const ResourcePatch> createInstance(const nlohmann::json &resourcePatchInfo, ResourceSpecies* const species, const std::string &filename);
};

#endif // RESOURCE_PATCH_FACTORY_H_
