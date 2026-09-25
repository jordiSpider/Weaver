
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

void EdibleSearchParams::addResourceSearchParams(size_t numberExistingResourceSpecies, const vector<ResourceSpecies::ResourceID> &searchableResourceSpecies)
{
    resourceSearchParams.addSearchParams(numberExistingResourceSpecies, searchableResourceSpecies);
}

const AnimalSearchParams& EdibleSearchParams::getAnimalSearchParams() const
{
    return animalSearchParams;
}

const ResourceSearchParams& EdibleSearchParams::getResourceSearchParams() const
{
    return resourceSearchParams;
}

void EdibleSearchParams::init()
{
    animalSearchParams.init();
    resourceSearchParams.init();
}
