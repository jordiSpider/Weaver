#include "IBM/World/LivingBeings/Resources/Species/UpdateMethodType/BasalResourceSpecies.h"


using namespace std;
using json = nlohmann::json;

BasalResourceSpecies::BasalResourceSpecies(const json &basalResourceSpeciesInfo, World* const world)
    : ResourceSpecies(basalResourceSpeciesInfo, world)
{

}

BasalResourceSpecies::~BasalResourceSpecies()
{

}
