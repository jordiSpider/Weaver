#ifndef BASAL_RESOURCE_SPECIES_H_
#define BASAL_RESOURCE_SPECIES_H_

#include <string>
#include <nlohmann/json.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"


class BasalResourceSpecies: public ResourceSpecies
{
public:
	BasalResourceSpecies(WorldInterface* const worldInterface);
	BasalResourceSpecies(const ResourceSpecies::ID& speciesId, const ResourceSpecies::ResourceID& resourceSpeciesId, const nlohmann::json &basalResourceSpeciesInfo, WorldInterface* const worldInterface);
	~BasalResourceSpecies();

	const Type getResourceSpeciesType() const;

	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* BASAL_RESOURCE_SPECIES_H_ */
