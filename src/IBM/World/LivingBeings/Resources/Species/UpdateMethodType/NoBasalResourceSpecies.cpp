#include "IBM/World/LivingBeings/Resources/Species/UpdateMethodType/NoBasalResourceSpecies.h"


using namespace std;
using json = nlohmann::json;

NoBasalResourceSpecies::NoBasalResourceSpecies(const json &noBasalResourceSpeciesInfo, World* const world)
    : ResourceSpecies(noBasalResourceSpeciesInfo, world)
{

}

NoBasalResourceSpecies::~NoBasalResourceSpecies()
{

}
