
#include "IBM/World/Map/TerrainCells/AnimalSearchParams.h"
#include "IBM/World/World.h"

using namespace std;



AnimalSearchParams::AnimalSearchParams()
{
    
}

AnimalSearchParams::AnimalSearchParams(
        const World* const world,
        const vector<LifeStage::LifeStageValue> &newSearchableLifeStages,
        const vector<id_type> &newSearchableAnimalSpecies,
        const vector<Instar> &newSearchableInstars,
        const vector<AnimalSpecies::Gender::GenderValue> &newSearchableGenders)
{
    addSearchParams(world, newSearchableLifeStages, newSearchableAnimalSpecies, newSearchableInstars, newSearchableGenders);
}

AnimalSearchParams::~AnimalSearchParams()
{
    
}

void AnimalSearchParams::initializedParams(const World* const world)
{
    searchableAnimalSpecies.resize(LifeStage::size());
    searchableInstars.resize(LifeStage::size());
    searchableGenders.resize(LifeStage::size());

    // -------------------------------------------------

    for(unsigned int lifeStage = 0; lifeStage < LifeStage::size(); lifeStage++)
    {
        searchableInstars[lifeStage].resize(AnimalSpecies::getAnimalSpeciesCounter());
        searchableGenders[lifeStage].resize(AnimalSpecies::getAnimalSpeciesCounter());
    
        // -------------------------------------------------

        for(unsigned int animalSpeciesId = 0; animalSpeciesId < AnimalSpecies::getAnimalSpeciesCounter(); animalSpeciesId++)
        {
            searchableGenders[lifeStage][animalSpeciesId].resize(world->getExistingAnimalSpecies()[animalSpeciesId]->getNumberOfInstars());
        }
    }
}

void AnimalSearchParams::addSearchParams(
        const World* const world,
        const vector<LifeStage::LifeStageValue> &newSearchableLifeStages,
        const vector<id_type> &newSearchableAnimalSpecies,
        const vector<Instar> &newSearchableInstars,
        const vector<AnimalSpecies::Gender::GenderValue> &newSearchableGenders)
{
    #ifdef DEBUG
    if(newSearchableLifeStages.empty())
    {
        throwLineInfoException("The search life stages vector must not be empty");
    }

    if(newSearchableGenders.empty())
    {
        throwLineInfoException("The search genders vector must not be empty");
    }
	#endif

    if(searchableAnimalSpecies.empty())
    {
        initializedParams(world);
    }

    // -------------------------------------------------

    const vector<id_type>* finalSearchableAnimalSpecies;
    vector<id_type> allSearchableAnimalSpecies;

    if(newSearchableAnimalSpecies.empty())
    {
        for(unsigned int animalSpeciesId = 0; animalSpeciesId < AnimalSpecies::getAnimalSpeciesCounter(); animalSpeciesId++)
        {
            allSearchableAnimalSpecies.push_back(animalSpeciesId);
        }

        finalSearchableAnimalSpecies = &allSearchableAnimalSpecies;
    }
    else
    {
        finalSearchableAnimalSpecies = &newSearchableAnimalSpecies;
    }

    // -------------------------------------------------

    for(const LifeStage::LifeStageValue &lifeStage : newSearchableLifeStages)
    {
        searchableLifeStages.insert(lifeStage);

        for(const id_type &animalSpeciesId : *finalSearchableAnimalSpecies)
        {
            searchableAnimalSpecies[lifeStage].insert(animalSpeciesId);

            // -------------------------------------------------

            const vector<Instar>* finalSearchableInstars;

            if(newSearchableInstars.empty())
            {
                finalSearchableInstars = &world->getExistingAnimalSpecies()[animalSpeciesId]->getInstarsRange();
            }
            else
            {
                finalSearchableInstars = &newSearchableInstars;
            }

            // -------------------------------------------------

            for(const Instar &instar : *finalSearchableInstars)
            {
                searchableInstars[lifeStage][animalSpeciesId].insert(instar);

                for(const auto &gender : newSearchableGenders)
                {
                    searchableGenders[lifeStage][animalSpeciesId][instar].insert(gender);
                }
            }
        }
    }
}

const unordered_set<LifeStage::LifeStageValue>& AnimalSearchParams::getSearchableLifeStages() const
{
    return searchableLifeStages;
}

const unordered_set<id_type>& AnimalSearchParams::getSearchableAnimalSpecies(
        const LifeStage::LifeStageValue &lifeStage
    ) const
{
    return searchableAnimalSpecies[lifeStage];
}

const unordered_set<Instar>& AnimalSearchParams::getSearchableInstars(
        const LifeStage::LifeStageValue &lifeStage,
        const id_type &animalSpeciesId
    ) const
{
    return searchableInstars[lifeStage][animalSpeciesId];
}

const unordered_set<AnimalSpecies::Gender::GenderValue>& AnimalSearchParams::getSearchableGenders(
        const LifeStage::LifeStageValue &lifeStage,
        const id_type &animalSpeciesId, const Instar &instar
    ) const
{
    return searchableGenders[lifeStage][animalSpeciesId][instar];
}

template <class Archive>
void AnimalSearchParams::serialize(Archive &ar, const unsigned int version) {
    ar & searchableLifeStages;
    ar & searchableAnimalSpecies;
    ar & searchableInstars;
    ar & searchableGenders;
}
