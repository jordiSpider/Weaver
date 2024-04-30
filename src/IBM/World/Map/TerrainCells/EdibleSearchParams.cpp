
#include "IBM/World/Map/TerrainCells/EdibleSearchParams.h"

using namespace std;



EdibleSearchParams::EdibleSearchParams()
    : animalSearchParams(), resourceSearchParams()
{
    
}

EdibleSearchParams::~EdibleSearchParams()
{
    
}

void EdibleSearchParams::addAnimalSearchParams(
        const World* const world,
        const vector<LifeStage::LifeStageValue> &searchableLifeStages,
        const vector<id_type> &searchableAnimalSpecies,
        const vector<Instar> &searchableInstars,
        const vector<AnimalSpecies::Gender::GenderValue> &searchableGenders)
{
    animalSearchParams.addSearchParams(
        world, searchableLifeStages, searchableAnimalSpecies, 
        searchableInstars, searchableGenders
    );
}

void EdibleSearchParams::addResourceSearchParams(const vector<id_type> &searchableResourceSpecies)
{
    resourceSearchParams.addSearchParams(searchableResourceSpecies);
}

const AnimalSearchParams& EdibleSearchParams::getAnimalSearchParams() const
{
    return animalSearchParams;
}

const ResourceSearchParams& EdibleSearchParams::getResourceSearchParams() const
{
    return resourceSearchParams;
}

template <class Archive>
void EdibleSearchParams::serialize(Archive &ar, const unsigned int version) {
    ar & animalSearchParams;
    ar & resourceSearchParams;
}
