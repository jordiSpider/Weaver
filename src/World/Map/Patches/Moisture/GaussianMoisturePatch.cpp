#include "World/Map/Patches/Moisture/GaussianMoisturePatch.h"

using namespace std;
using json = nlohmann::json;



GaussianMoisturePatch::GaussianMoisturePatch(const json &moisturePatchInfo, const string &filename)
    : MoisturePatch(moisturePatchInfo, new GaussianPatch(moisturePatchInfo["shape"], filename))
{
    
}


GaussianMoisturePatch::~GaussianMoisturePatch()
{

}