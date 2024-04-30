
#include "IBM/World/Map/TerrainCells/AnimalSearchParams.h"
#include "IBM/World/WorldInterface.h"

using namespace std;



AnimalSearchParams::AnimalSearchParams()
{
    
}

AnimalSearchParams::AnimalSearchParams(
        const WorldInterface* const worldInterface,
        const vector<LifeStage> &newSearchableLifeStages,
        const vector<AnimalSpecies::AnimalID> &newSearchableAnimalSpecies,
        const vector<Instar> &newSearchableInstars,
        const vector<AnimalSpecies::Gender> &newSearchableGenders)
{
    addSearchParams(worldInterface, newSearchableLifeStages, newSearchableAnimalSpecies, newSearchableInstars, newSearchableGenders);
}

AnimalSearchParams::~AnimalSearchParams()
{
    
}

void AnimalSearchParams::initializedParams(const WorldInterface* const worldInterface)
{
    searchableAnimalSpecies.resize(EnumClass<LifeStage>::size());
    searchableInstars.resize(EnumClass<LifeStage>::size());
    searchableGenders.resize(EnumClass<LifeStage>::size());

    // -------------------------------------------------

    for(const LifeStage& lifeStage : EnumClass<LifeStage>::getEnumValues())
    {
        searchableInstars[lifeStage].resize(worldInterface->getExistingAnimalSpecies().size());
        searchableGenders[lifeStage].resize(worldInterface->getExistingAnimalSpecies().size());
    
        // -------------------------------------------------

        for(unsigned int animalSpeciesId = 0; animalSpeciesId < worldInterface->getExistingAnimalSpecies().size(); animalSpeciesId++)
        {
            searchableGenders[lifeStage][animalSpeciesId].resize(worldInterface->getExistingAnimalSpecies()[animalSpeciesId]->getNumberOfInstars());
        }
    }
}

void AnimalSearchParams::addSearchParams(
        const WorldInterface* const worldInterface,
        const vector<LifeStage> &newSearchableLifeStages,
        const vector<AnimalSpecies::AnimalID> &newSearchableAnimalSpecies,
        const vector<Instar> &newSearchableInstars,
        const vector<AnimalSpecies::Gender> &newSearchableGenders)
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
        initializedParams(worldInterface);
    }

    // -------------------------------------------------

    const vector<AnimalSpecies::AnimalID>* finalSearchableAnimalSpecies;
    vector<AnimalSpecies::AnimalID> allSearchableAnimalSpecies;

    if(newSearchableAnimalSpecies.empty())
    {
        for(unsigned int animalSpeciesId = 0; animalSpeciesId < worldInterface->getExistingAnimalSpecies().size(); animalSpeciesId++)
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

    for(const LifeStage &lifeStage : newSearchableLifeStages)
    {
        searchableLifeStages.insert(lifeStage);

        for(const AnimalSpecies::AnimalID &animalSpeciesId : *finalSearchableAnimalSpecies)
        {
            searchableAnimalSpecies[lifeStage].insert(animalSpeciesId);

            // -------------------------------------------------

            const vector<Instar>* finalSearchableInstars;

            if(newSearchableInstars.empty())
            {
                finalSearchableInstars = &worldInterface->getExistingAnimalSpecies()[animalSpeciesId]->getInstarsRange();
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

const unordered_set<LifeStage>& AnimalSearchParams::getSearchableLifeStages() const
{
    return searchableLifeStages;
}

const unordered_set<AnimalSpecies::AnimalID>& AnimalSearchParams::getSearchableAnimalSpecies(
        const LifeStage &lifeStage
    ) const
{
    return searchableAnimalSpecies[lifeStage];
}

const unordered_set<Instar>& AnimalSearchParams::getSearchableInstars(
        const LifeStage &lifeStage,
        const AnimalSpecies::AnimalID &animalSpeciesId
    ) const
{
    return searchableInstars[lifeStage][animalSpeciesId];
}

const unordered_set<AnimalSpecies::Gender>& AnimalSearchParams::getSearchableGenders(
        const LifeStage &lifeStage,
        const AnimalSpecies::AnimalID &animalSpeciesId, const Instar &instar
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


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, AnimalSearchParams* &animalSearchParamsPtr, const unsigned int version) {
            // Deserialization
            if (Archive::is_loading::value) {
				ar & *animalSearchParamsPtr;
            } // Serialization
            else {
                ar & *animalSearchParamsPtr;
            }
        }

		// Specialisation
		template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, AnimalSearchParams*&, const unsigned int);
		template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, AnimalSearchParams*&, const unsigned int);

		template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, AnimalSearchParams*&, const unsigned int);
		template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, AnimalSearchParams*&, const unsigned int);
    }
}
