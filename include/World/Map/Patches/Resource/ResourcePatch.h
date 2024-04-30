#ifndef RESOURCE_PATCH_H_
#define RESOURCE_PATCH_H_

#include <nlohmann/json.hpp>
#include <memory>

#include "World/Map/Patches/Patch.h"
#include "World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "Misc/Macros.h"



class ResourcePatch : public Patch {
protected:
    ResourceSpecies* const species;
    const double initialBiomassDensity;
    const double resourceMaximumCapacityDensity;
    const bool patchSpread;

    ResourcePatch(const nlohmann::json &resourceInfo, const FormPatch* const &form, ResourceSpecies* const species);
    ResourcePatch(const unsigned int priority, const nlohmann::json &resourceInfo, const FormPatch* const &form, ResourceSpecies* const species);

    // Getter
    const std::string getContentType() const;
    const std::string getContentInfo() const;

public:
    virtual ~ResourcePatch();

    ResourceSpecies* const getSpecies() const;
    const double& getInitialBiomassDensity() const;
    const double& getResourceMaximumCapacityDensity() const;
    const bool getPatchSpread() const;
};

#endif /* RESOURCE_PATCH_H_ */