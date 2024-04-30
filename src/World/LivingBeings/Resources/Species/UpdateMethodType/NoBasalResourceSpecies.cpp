#include "World/LivingBeings/Resources/Species/UpdateMethodType/NoBasalResourceSpecies.h"


using namespace std;
using json = nlohmann::json;

NoBasalResourceSpecies::NoBasalResourceSpecies(const json &noBasalResourceSpeciesInfo)
    : ResourceSpecies(noBasalResourceSpeciesInfo)
{

}

NoBasalResourceSpecies::~NoBasalResourceSpecies()
{

}
