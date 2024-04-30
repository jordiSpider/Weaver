#ifndef BASAL_RESOURCE_SPECIES_H_
#define BASAL_RESOURCE_SPECIES_H_

#include <string>
#include <nlohmann/json.hpp>

#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"


class BasalResourceSpecies: public ResourceSpecies
{
public:
	BasalResourceSpecies(const nlohmann::json &basalResourceSpeciesInfo, World* const world);
	~BasalResourceSpecies();

private:
	
};

#endif /* BASAL_RESOURCE_SPECIES_H_ */
