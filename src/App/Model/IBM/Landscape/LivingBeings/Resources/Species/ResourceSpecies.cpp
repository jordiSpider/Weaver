#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/ResourceSpecies.h"


using namespace std;
using json = nlohmann::json;


ResourceSpecies::ResourceID::ResourceID()
{

}

ResourceSpecies::ResourceID::ResourceID(const id_type& value)
	: value(value)
{

}

const id_type& ResourceSpecies::ResourceID::getValue() const
{
	return value;
}

ResourceSpecies::ResourceID::operator size_t() const {
	return static_cast<size_t>(value);
}

size_t hash<ResourceSpecies::ResourceID>::operator()(const ResourceSpecies::ResourceID& resourceID) const
{
    return hash<size_t>{}(resourceID.getValue());
}

template <class Archive>
void ResourceSpecies::ResourceID::serialize(Archive &ar, const unsigned int) {
	ar & value;
}

// Specialisation
template void ResourceSpecies::ResourceID::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ResourceSpecies::ResourceID::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ResourceSpecies::ResourceID::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ResourceSpecies::ResourceID::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);




ResourceSpecies::ResourceSpecies()
	: Species()
{

}

ResourceSpecies::ResourceSpecies(const ResourceSpecies::ID& speciesId, const ResourceSpecies::ResourceID& resourceSpeciesId, const json &resourceSpeciesInfo) 
	: Species(speciesId, resourceSpeciesInfo["name"].get<string>()), 
	  resourceSpeciesId(resourceSpeciesId), resourceSpeciesGrowth(resourceSpeciesInfo["growthModule"])
{
	
}

ResourceSpecies::~ResourceSpecies()
{
	
}

const ResourceSpecies::ResourceID& ResourceSpecies::getResourceSpeciesId() const
{
	return resourceSpeciesId;
}

SpeciesType ResourceSpecies::getType() const
{
	return SpeciesType::Resource;
}

bool ResourceSpecies::isMobile() const 
{ 
	return false; 
}

const ResourceSpeciesGrowth& ResourceSpecies::getGrowthBuildingBlock() const
{
	return resourceSpeciesGrowth;
}

ResourceSpeciesGrowth& ResourceSpecies::getMutableGrowthBuildingBlock()
{
	return resourceSpeciesGrowth;
}



BOOST_CLASS_EXPORT(ResourceSpecies)

template <class Archive>
void ResourceSpecies::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<Species>(*this);

	ar & resourceSpeciesId;

	ar & resourceSpeciesGrowth;
}

// // Specialisation
template void ResourceSpecies::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ResourceSpecies::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ResourceSpecies::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ResourceSpecies::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
