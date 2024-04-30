#include "IBM/World/LivingBeings/Resources/Species/BasalResourceSpecies.h"


using namespace std;
using json = nlohmann::json;

BasalResourceSpecies::BasalResourceSpecies(WorldInterface* const worldInterface)
    : ResourceSpecies(worldInterface)
{

}

BasalResourceSpecies::BasalResourceSpecies(const ResourceSpecies::ID& speciesId, const ResourceSpecies::ResourceID& resourceSpeciesId, const json &basalResourceSpeciesInfo, WorldInterface* const worldInterface)
    : ResourceSpecies(speciesId, resourceSpeciesId, basalResourceSpeciesInfo, worldInterface)
{

}

BasalResourceSpecies::~BasalResourceSpecies()
{

}

const ResourceSpecies::Type BasalResourceSpecies::getResourceSpeciesType() const
{
    return ResourceSpecies::Type::BasalResource;
}

template <class Archive>
void BasalResourceSpecies::serialize(Archive &ar, const unsigned int version) {
    ar & boost::serialization::base_object<ResourceSpecies>(*this);
}

// Specialisation
template void BasalResourceSpecies::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void BasalResourceSpecies::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void BasalResourceSpecies::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void BasalResourceSpecies::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
