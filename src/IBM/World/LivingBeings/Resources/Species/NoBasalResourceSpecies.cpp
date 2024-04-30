#include "IBM/World/LivingBeings/Resources/Species/NoBasalResourceSpecies.h"


using namespace std;
using json = nlohmann::json;


NoBasalResourceSpecies::NoBasalResourceSpecies(WorldInterface* const worldInterface)
    : ResourceSpecies(worldInterface)
{

}

NoBasalResourceSpecies::NoBasalResourceSpecies(const ResourceSpecies::ID& speciesId, const ResourceSpecies::ResourceID& resourceSpeciesId, const json &noBasalResourceSpeciesInfo, WorldInterface* const worldInterface)
    : ResourceSpecies(speciesId, resourceSpeciesId, noBasalResourceSpeciesInfo, worldInterface)
{

}

NoBasalResourceSpecies::~NoBasalResourceSpecies()
{

}

const ResourceSpecies::Type NoBasalResourceSpecies::getResourceSpeciesType() const
{
    return ResourceSpecies::Type::NoBasalResource;
}

template <class Archive>
void NoBasalResourceSpecies::serialize(Archive &ar, const unsigned int version) {
    ar & boost::serialization::base_object<ResourceSpecies>(*this);
}

// Specialisation
template void NoBasalResourceSpecies::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void NoBasalResourceSpecies::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void NoBasalResourceSpecies::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void NoBasalResourceSpecies::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
