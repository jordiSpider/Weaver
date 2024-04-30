#include "IBM/World/Map/Patches/Resource/HomogeneousResourcePatch.h"

using namespace std;
using json = nlohmann::json;



HomogeneousResourcePatch::HomogeneousResourcePatch(const json &resourcePatchInfo, ResourceSpecies* const species, const string &filename)
    : ResourcePatch(resourcePatchInfo, new HomogeneousPatch(filename), species)
{
    
}


HomogeneousResourcePatch::HomogeneousResourcePatch(const unsigned int priority, const nlohmann::json &resourcePatchInfo, ResourceSpecies* const species, const string &filename)
    : ResourcePatch(priority, resourcePatchInfo, new HomogeneousPatch(filename), species)
{

}


HomogeneousResourcePatch::~HomogeneousResourcePatch()
{

}