#include "App/Model/IBM/Landscape/Map/Patches/Source/ResourceSource.h"


using namespace std;
using json = nlohmann::json;


ResourceSource::ResourceSource()
    : PathSource()
{

}

ResourceSource::ResourceSource(ResourceSpecies* const species)
    : PathSource(),
	  resourceSpeciesId(species->getResourceSpeciesId()),
	  resourceSpeciesName(species->getScientificName()),
	  resourceMaximumCapacity(0.0),
	  edibleFractionOfMaxCarryingCapacity(1.0),
	  minimumEdibleBiomass(0.0),
	  patchSpread(false),
	  growthDynamics(new NonDynamics())
{

}

ResourceSource::ResourceSource(const json &config, ResourceSpecies* const species, const PreciseDouble& hyperVolume, const PreciseDouble& scaleMass, const PreciseDouble& timeStepsPerDay)
    : PathSource(),
	  resourceSpeciesId(species->getResourceSpeciesId()),
	  resourceSpeciesName(species->getScientificName()),
	  resourceMaximumCapacity(config["resourceMaximumCapacityDensity"].get<double>() * hyperVolume),
	  edibleFractionOfMaxCarryingCapacity(config["edibleFractionOfMaxCarryingCapacity"].get<double>()),
	  minimumEdibleBiomass(resourceMaximumCapacity.getValue()*(1.0 - edibleFractionOfMaxCarryingCapacity)),
	  patchSpread(config["patchSpread"].get<bool>()),
	  growthDynamics(Dynamics::createInstanceForResource(config["growthDynamics"], scaleMass, species->getGrowthBuildingBlock().getCellMass(), timeStepsPerDay, hyperVolume))
{

}

ResourceSource::~ResourceSource()
{
    delete growthDynamics;
}

const ResourceSpecies::ResourceID& ResourceSource::getResourceSpeciesId() const
{
	return resourceSpeciesId;
}


const WetMass& ResourceSource::getResourceMaximumCapacity() const
{
	return resourceMaximumCapacity;
}

const PreciseDouble& ResourceSource::getEdibleFractionOfMaxCarryingCapacity() const
{
	return edibleFractionOfMaxCarryingCapacity;
}

const WetMass& ResourceSource::getMinimumEdibleBiomass() const
{
	return minimumEdibleBiomass;
}

bool ResourceSource::getPatchSpread() const
{
	return patchSpread;
}

void ResourceSource::update()
{
	growthDynamics->update();
}

string ResourceSource::showInfo() const
{
	std::ostringstream info;

	info << " - Resource parameters:\n   - Species = " << resourceSpeciesName << "\n";

	info << "   - Resource maximum capacity = " << getResourceMaximumCapacity().getValue() << "\n";

	info << "   - Edible fraction of max carrying capacity = " << getEdibleFractionOfMaxCarryingCapacity() << "\n";

	info << "   - Patch spread = " << getPatchSpread();

	info << "   - Growth dynamics:\n";

	info << growthDynamics->showInfo("     ") << "\n";

	return info.str();
}

PathSource::Type ResourceSource::getType() const
{
	return Type::Resource;
}

const Dynamics& ResourceSource::getGrowthDynamics() const
{
	return *growthDynamics;
}



BOOST_CLASS_EXPORT(ResourceSource)

template<class Archive>
void ResourceSource::serialize(Archive & ar, const unsigned int) {
    ar & boost::serialization::base_object<PathSource>(*this);

	ar & resourceSpeciesId;
	ar & resourceSpeciesName;

    ar & resourceMaximumCapacity;
    ar & edibleFractionOfMaxCarryingCapacity;
	ar & minimumEdibleBiomass;

    ar & patchSpread;

    ar & growthDynamics;
}

// Specialisation
template void ResourceSource::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ResourceSource::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ResourceSource::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ResourceSource::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
