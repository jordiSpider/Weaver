#include "World/Map/Patches/Resource/GaussianResourcePatch.h"

using namespace std;
using json = nlohmann::json;



GaussianResourcePatch::GaussianResourcePatch(const json &resourcePatchInfo, ResourceSpecies* const species, const string &filename)
    : ResourcePatch(resourcePatchInfo, new GaussianPatch(resourcePatchInfo["shape"], filename), species)
{
    
}


GaussianResourcePatch::~GaussianResourcePatch()
{

}