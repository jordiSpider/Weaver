#include "World/Map/Patches/Resource/CubicResourcePatch.h"

using namespace std;
using json = nlohmann::json;



CubicResourcePatch::CubicResourcePatch(const json &resourcePatchInfo, ResourceSpecies* const species, const string &filename)
    : ResourcePatch(resourcePatchInfo, new CubicPatch(resourcePatchInfo["shape"], filename), species)
{
    
}


CubicResourcePatch::~CubicResourcePatch()
{

}