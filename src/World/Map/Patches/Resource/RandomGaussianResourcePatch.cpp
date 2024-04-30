#include "World/Map/Patches/Resource/RandomGaussianResourcePatch.h"

using namespace std;
using json = nlohmann::json;



RandomGaussianResourcePatch::RandomGaussianResourcePatch(const json &resourcePatchInfo, ResourceSpecies* const species, const string &filename)
    : ResourcePatch(resourcePatchInfo, new RandomGaussianPatch(resourcePatchInfo["shape"], filename), species)
{
    
}


RandomGaussianResourcePatch::~RandomGaussianResourcePatch()
{

}