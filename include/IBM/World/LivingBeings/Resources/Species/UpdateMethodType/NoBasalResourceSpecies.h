#ifndef NO_BASAL_RESOURCE_SPECIES_H_
#define NO_BASAL_RESOURCE_SPECIES_H_

#include <nlohmann/json.hpp>

#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"


class NoBasalResourceSpecies: public ResourceSpecies
{
public:
	NoBasalResourceSpecies(const nlohmann::json &noBasalResourceSpeciesInfo, World* const world);
	~NoBasalResourceSpecies();

private:
	
};

#endif /* NO_BASAL_RESOURCE_SPECIES_H_ */
