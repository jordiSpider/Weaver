#include "IBM/World/Map/Patches/Resource/GaussianResourcePatch.h"

using namespace std;
using json = nlohmann::json;



GaussianResourcePatch::GaussianResourcePatch(const json &resourcePatchInfo, ResourceSpecies* const species, const string &filename)
    : ResourcePatch(resourcePatchInfo, new GaussianPatch(resourcePatchInfo["shape"], filename), species)
{
    
}


GaussianResourcePatch::~GaussianResourcePatch()
{

}

template <class Archive>
void GaussianResourcePatch::serialize(Archive &ar, const unsigned int version) 
{
    
}
