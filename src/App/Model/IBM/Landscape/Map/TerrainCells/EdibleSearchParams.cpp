
#include "App/Model/IBM/Landscape/Map/TerrainCells/EdibleSearchParams.h"

using namespace std;



EdibleSearchParams::EdibleSearchParams()
    : animalSearchParams(), resourceSearchParams()
{
    
}

EdibleSearchParams::~EdibleSearchParams()
{
    
}

EdibleSearchParams::EdibleSearchParams(const EdibleSearchParams &other)
{
    animalSearchParams = other.animalSearchParams;
    resourceSearchParams = other.resourceSearchParams;
}

EdibleSearchParams& EdibleSearchParams::operator=(const EdibleSearchParams& other)
{
    if (this != &other) {
        animalSearchParams = other.animalSearchParams;
        resourceSearchParams = other.resourceSearchParams;
    }
    return *this;
}

void EdibleSearchParams::addAnimalSearchParams(
        const vector<LifeStage> &searchableLifeStages,
        const vector<AnimalSpeciesID> &searchableAnimalSpecies,
        const vector<Instar> &searchableInstars,
        const vector<Gender> &searchableGenders)
{
    animalSearchParams.addSearchParams(searchableLifeStages, searchableAnimalSpecies, searchableInstars, searchableGenders);
}

void EdibleSearchParams::addResourceSearchParams(const vector<ResourceSpecies*>& existingResourceSpecies, const vector<ResourceSpecies::ResourceID> &searchableResourceSpecies)
{
    resourceSearchParams.addSearchParams(existingResourceSpecies, searchableResourceSpecies);
}

const AnimalSearchParams& EdibleSearchParams::getAnimalSearchParams() const
{
    return animalSearchParams;
}

const ResourceSearchParams& EdibleSearchParams::getResourceSearchParams() const
{
    return resourceSearchParams;
}

void EdibleSearchParams::clear()
{
    animalSearchParams.clear();
    resourceSearchParams.clear();
}



BOOST_CLASS_EXPORT(EdibleSearchParams)

template <class Archive>
void EdibleSearchParams::serialize(Archive &ar, const unsigned int) {
    ar & animalSearchParams;
    ar & resourceSearchParams;
}

// Specialisation
template void EdibleSearchParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void EdibleSearchParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void EdibleSearchParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void EdibleSearchParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
