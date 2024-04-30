
#include "IBM/World/Map/TerrainCells/ResourceSearchParams.h"

using namespace std;



ResourceSearchParams::ResourceSearchParams()
{
    
}

ResourceSearchParams::~ResourceSearchParams()
{
    
}

void ResourceSearchParams::addSearchParams(const WorldInterface* const worldInterface, const vector<ResourceSpecies::ResourceID> &searchableResourceSpecies)
{
    // -------------------------------------------------

    const vector<ResourceSpecies::ResourceID>* finalSearchableResourceSpecies;
    vector<ResourceSpecies::ResourceID> allSearchableResourceSpecies;

    if(searchableResourceSpecies.empty())
    {
        for(unsigned int resourceSpeciesId = 0; resourceSpeciesId < worldInterface->getExistingResourceSpecies().size(); resourceSpeciesId++)
        {
            allSearchableResourceSpecies.push_back(resourceSpeciesId);
        }

        finalSearchableResourceSpecies = &allSearchableResourceSpecies;
    }
    else
    {
        finalSearchableResourceSpecies = &searchableResourceSpecies;
    }

    // -------------------------------------------------

    for(const auto &resourceSpeciesId : *finalSearchableResourceSpecies)
    {
        searchParams.insert(resourceSpeciesId);
    }
}

const SearchableResourceSpecies& ResourceSearchParams::getSearchParams() const
{
    return searchParams;
}

template <class Archive>
void ResourceSearchParams::serialize(Archive &ar, const unsigned int version) {
    ar & searchParams;
}

// Specialisation
template void ResourceSearchParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ResourceSearchParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ResourceSearchParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ResourceSearchParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
