#include "IBM/World/Map/Patches/Resource/ResourcePatch.h"

using namespace std;
using json = nlohmann::json;



ResourcePatch::ResourcePatch(const json &resourceInfo, const FormPatch* const &form, ResourceSpecies* const species)
    : Patch(resourceInfo["priority"], form),
	  species(species),
	  initialBiomassDensity(resourceInfo["initialBiomassDensity"]),
	  resourceMaximumCapacityDensity(resourceInfo["resourceMaximumCapacityDensity"]),
	  patchSpread(resourceInfo["patchSpread"])
{
    
}

ResourcePatch::ResourcePatch(const unsigned int priority, const json &resourceInfo, const FormPatch* const &form, ResourceSpecies* const species)
    : Patch(priority, form),
	  species(species),
	  initialBiomassDensity(resourceInfo["initialBiomassDensity"]),
	  resourceMaximumCapacityDensity(resourceInfo["resourceMaximumCapacityDensity"]),
	  patchSpread(resourceInfo["patchSpread"])
{
    
}


ResourcePatch::~ResourcePatch()
{
	
}


ResourceSpecies* const ResourcePatch::getSpecies() const
{
	return species;
}


const double& ResourcePatch::getInitialBiomassDensity() const
{
	return initialBiomassDensity;
}


const double& ResourcePatch::getResourceMaximumCapacityDensity() const
{
	return resourceMaximumCapacityDensity;
}


const bool ResourcePatch::getPatchSpread() const
{
	return patchSpread;
}

const string ResourcePatch::getContentType() const
{
	return string("resource");
}

const string ResourcePatch::getContentInfo() const
{
	return fmt::format(
		" - Resource parameters:\n   - Species = {}\n   - Initial biomass density = {}\n   - Resource maximum capacity density = {}\n   - Patch spread = {}", 
		getSpecies()->getScientificName(), getInitialBiomassDensity(), getResourceMaximumCapacityDensity(), getPatchSpread()
	);
}
