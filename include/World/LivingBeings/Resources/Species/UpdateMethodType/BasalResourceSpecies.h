#ifndef BASAL_RESOURCE_SPECIES_H_
#define BASAL_RESOURCE_SPECIES_H_

#include <string>
#include <nlohmann/json.hpp>

#include "World/LivingBeings/Resources/Species/ResourceSpecies.h"


class BasalResourceSpecies: public ResourceSpecies
{
public:
	BasalResourceSpecies(const nlohmann::json &basalResourceSpeciesInfo);
	~BasalResourceSpecies();

private:
	
};

#endif /* BASAL_RESOURCE_SPECIES_H_ */
