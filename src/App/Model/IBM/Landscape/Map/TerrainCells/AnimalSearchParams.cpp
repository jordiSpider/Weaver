
#include "App/Model/IBM/Landscape/Map/TerrainCells/AnimalSearchParams.h"


using namespace std;



AnimalSearchParams::AnimalSearchParams()
{
    searchableAnimalSpecies.resize(EnumClass<LifeStage>::size());
    searchableInstars.resize(EnumClass<LifeStage>::size());
    searchableGenders.resize(EnumClass<LifeStage>::size());
}

AnimalSearchParams::AnimalSearchParams(
    const vector<LifeStage> &newSearchableLifeStages,
    const vector<AnimalSpeciesID> &newSearchableAnimalSpecies,
    const vector<Instar> &newSearchableInstars,
    const vector<Gender> &newSearchableGenders)
    : AnimalSearchParams()
{
    addSearchParams(newSearchableLifeStages, newSearchableAnimalSpecies, newSearchableInstars, newSearchableGenders);
}

AnimalSearchParams::~AnimalSearchParams()
{
    
}

AnimalSearchParams::AnimalSearchParams(const AnimalSearchParams &other)
{
    searchableLifeStages = other.searchableLifeStages;
    searchableAnimalSpecies = other.searchableAnimalSpecies;
    searchableInstars = other.searchableInstars;
    searchableGenders = other.searchableGenders;
}

AnimalSearchParams& AnimalSearchParams::operator=(const AnimalSearchParams& other)
{
    if (this != &other) {
        searchableLifeStages = other.searchableLifeStages;
        searchableAnimalSpecies = other.searchableAnimalSpecies;
        searchableInstars = other.searchableInstars;
        searchableGenders = other.searchableGenders;
    }
    return *this;
}

template<typename T>
void AnimalSearchParams::insertElement(vector<T>& uniqueVector, const T& elementToInsert)
{
    if(!binary_search(uniqueVector.begin(), uniqueVector.end(), elementToInsert)) {
        uniqueVector.insert(std::upper_bound(uniqueVector.begin(), uniqueVector.end(), elementToInsert), elementToInsert);
    }
}

void AnimalSearchParams::addSearchParams(
    const vector<LifeStage> &newSearchableLifeStages,
    const vector<AnimalSpeciesID> &newSearchableAnimalSpecies,
    const vector<Instar> &newSearchableInstars,
    const vector<Gender> &newSearchableGenders)
{
    #ifdef DEBUG
    if(newSearchableLifeStages.empty())
    {
        throwLineInfoException("The search life stages vector must not be empty");
    }

    if(newSearchableAnimalSpecies.empty())
    {
        throwLineInfoException("The search animal species vector must not be empty");
    }

    if(newSearchableInstars.empty())
    {
        throwLineInfoException("The search instars vector must not be empty");
    }

    if(newSearchableGenders.empty())
    {
        throwLineInfoException("The search genders vector must not be empty");
    }
	#endif

    
    for(const LifeStage &lifeStage : newSearchableLifeStages)
    {
        insertElement(searchableLifeStages, lifeStage);

        for(const AnimalSpeciesID &animalSpeciesId : newSearchableAnimalSpecies)
        {
            insertElement(searchableAnimalSpecies[lifeStage], animalSpeciesId);

            for(const Instar &instar : newSearchableInstars)
            {
                try {
                    insertElement(searchableInstars[lifeStage].at(animalSpeciesId), instar);
                } catch (const std::out_of_range&) {
                    searchableInstars[lifeStage].resize(animalSpeciesId+1);
                    searchableGenders[lifeStage].resize(animalSpeciesId+1);

                    insertElement(searchableInstars[lifeStage][animalSpeciesId], instar);
                }


                for(const auto &gender : newSearchableGenders)
                {
                    try {
                        insertElement(searchableGenders[lifeStage][animalSpeciesId].at(instar), gender);
                    } catch (const std::out_of_range&) {
                        searchableGenders[lifeStage][animalSpeciesId].resize(static_cast<size_t>(instar)+1);

                        insertElement(searchableGenders[lifeStage][animalSpeciesId][instar], gender);
                    }
                }
            }
        }
    }
}

const vector<LifeStage>& AnimalSearchParams::getSearchableLifeStages() const
{
    return searchableLifeStages;
}

const vector<AnimalSpeciesID>& AnimalSearchParams::getSearchableAnimalSpecies(
        const LifeStage &lifeStage
    ) const
{
    return searchableAnimalSpecies[lifeStage];
}

const vector<Instar>& AnimalSearchParams::getSearchableInstars(
        const LifeStage &lifeStage,
        const AnimalSpeciesID &animalSpeciesId
    ) const
{
    return searchableInstars[lifeStage][animalSpeciesId];
}

const vector<Gender>& AnimalSearchParams::getSearchableGenders(
        const LifeStage &lifeStage,
        const AnimalSpeciesID &animalSpeciesId, const Instar &instar
    ) const
{
    return searchableGenders[lifeStage][animalSpeciesId][instar];
}

void AnimalSearchParams::clear()
{
    searchableLifeStages.clear();
    searchableAnimalSpecies.clear();
    searchableInstars.clear();
    searchableGenders.clear();

    searchableAnimalSpecies.resize(EnumClass<LifeStage>::size());
    searchableInstars.resize(EnumClass<LifeStage>::size());
    searchableGenders.resize(EnumClass<LifeStage>::size());
}



BOOST_CLASS_EXPORT(AnimalSearchParams)

template <class Archive>
void AnimalSearchParams::serialize(Archive &ar, const unsigned int) {
    ar & searchableLifeStages;
    ar & searchableAnimalSpecies;
    ar & searchableInstars;
    ar & searchableGenders;
}

// Specialisation
template void AnimalSearchParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalSearchParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalSearchParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalSearchParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
