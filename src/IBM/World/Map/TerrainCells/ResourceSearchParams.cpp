
#include "IBM/World/Map/TerrainCells/ResourceSearchParams.h"

using namespace std;



ResourceSearchParams::ResourceSearchParams()
{
    
}

ResourceSearchParams::~ResourceSearchParams()
{
    
}

void ResourceSearchParams::addSearchParams(const vector<id_type> &searchableResourceSpecies)
{
    // -------------------------------------------------

    const vector<id_type>* finalSearchableResourceSpecies;
    vector<id_type> allSearchableResourceSpecies;

    if(searchableResourceSpecies.empty())
    {
        for(unsigned int resourceSpeciesId = 0; resourceSpeciesId < ResourceSpecies::getResourceSpeciesCounter(); resourceSpeciesId++)
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
