#ifndef NO_BASAL_RESOURCE_SPECIES_H_
#define NO_BASAL_RESOURCE_SPECIES_H_

#include <nlohmann/json.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"


class NoBasalResourceSpecies: public ResourceSpecies
{
public:
	NoBasalResourceSpecies(WorldInterface* const worldInterface);
	NoBasalResourceSpecies(const ResourceSpecies::ID& speciesId, const ResourceSpecies::ResourceID& resourceSpeciesId, const nlohmann::json &noBasalResourceSpeciesInfo, WorldInterface* const worldInterface);
	~NoBasalResourceSpecies();

	const Type getResourceSpeciesType() const;

	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* NO_BASAL_RESOURCE_SPECIES_H_ */
