#include "World/Map/Patches/Moisture/SphericalMoisturePatch.h"

using namespace std;
using json = nlohmann::json;



SphericalMoisturePatch::SphericalMoisturePatch(const json &moisturePatchInfo, const string &filename)
    : MoisturePatch(moisturePatchInfo, new SphericalPatch(moisturePatchInfo["shape"], filename))
{
    
}


SphericalMoisturePatch::~SphericalMoisturePatch()
{

}