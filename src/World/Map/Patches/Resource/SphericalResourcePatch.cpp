#include "World/Map/Patches/Resource/SphericalResourcePatch.h"

using namespace std;
using json = nlohmann::json;



SphericalResourcePatch::SphericalResourcePatch(const json &resourcePatchInfo, ResourceSpecies* const species, const string &filename)
    : ResourcePatch(resourcePatchInfo, new SphericalPatch(resourcePatchInfo["shape"], filename), species)
{
    
}


SphericalResourcePatch::~SphericalResourcePatch()
{

}