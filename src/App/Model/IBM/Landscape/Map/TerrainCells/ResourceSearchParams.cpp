
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
    validSignaturesBitmask = other.validSignaturesBitmask;
}

ResourceSearchParams& ResourceSearchParams::operator=(const ResourceSearchParams& other)
{
    if (this != &other) {
        validSignaturesBitmask = other.validSignaturesBitmask;
    }
    return *this;
}

void ResourceSearchParams::addSearchParams(size_t numberExistingResourceSpecies, const vector<ResourceSpecies::ResourceID> &searchableResourceSpecies)
{
    if(searchableResourceSpecies.empty())
    {
        validSignaturesBitmask.resize(numberExistingResourceSpecies, true);
    }
    else
    {
        for (const auto& resourceSpeciesId : searchableResourceSpecies)
        {
            validSignaturesBitmask[resourceSpeciesId] = true;
        }
    }
}

void ResourceSearchParams::init()
{
    if (ResourceSignature::BITMASK_DYNAMIC_SIZE == 0) {
        throwLineInfoException("ResourceSignature::BITMASK_DYNAMIC_SIZE is not configured. Please call ResourceSignature::configureBits() before initialize ResourceSearchParams.");
    }

    if (validSignaturesBitmask.size() < ResourceSignature::BITMASK_DYNAMIC_SIZE) {
        validSignaturesBitmask.resize(ResourceSignature::BITMASK_DYNAMIC_SIZE, false);
    }
}

bool ResourceSearchParams::matches(const CellResourceInterface& resource) const noexcept
{
    return validSignaturesBitmask[resource.getSignature().getValue()];
}
