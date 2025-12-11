
#include "App/Model/IBM/Landscape/Map/TerrainCells/ResourceSearchParams.h"


using namespace std;



ResourceSearchParams::ResourceSearchParams()
{
    
}

ResourceSearchParams::~ResourceSearchParams()
{
    
}

ResourceSearchParams::ResourceSearchParams(const ResourceSearchParams &other)
{
    searchParams = other.searchParams;
}

ResourceSearchParams& ResourceSearchParams::operator=(const ResourceSearchParams& other)
{
    if (this != &other) {
        searchParams = other.searchParams;
    }
    return *this;
}

void ResourceSearchParams::addSearchParams(const vector<ResourceSpecies*>& existingResourceSpecies, const vector<ResourceSpecies::ResourceID> &searchableResourceSpecies)
{
    // -------------------------------------------------

    const vector<ResourceSpecies::ResourceID>* finalSearchableResourceSpecies;
    vector<ResourceSpecies::ResourceID> allSearchableResourceSpecies;

    if(searchableResourceSpecies.empty())
    {
        for(ResourceSpecies *const& resourceSpecies : existingResourceSpecies)
        {
            allSearchableResourceSpecies.push_back(resourceSpecies->getResourceSpeciesId());
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

void ResourceSearchParams::clear()
{
    searchParams.clear();
}



BOOST_CLASS_EXPORT(ResourceSearchParams)

template <class Archive>
void ResourceSearchParams::serialize(Archive &ar, const unsigned int) {
    ar & searchParams;
}

// Specialisation
template void ResourceSearchParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ResourceSearchParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ResourceSearchParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ResourceSearchParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
