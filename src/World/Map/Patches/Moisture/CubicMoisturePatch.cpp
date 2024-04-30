#include "World/Map/Patches/Moisture/CubicMoisturePatch.h"

using namespace std;
using json = nlohmann::json;



CubicMoisturePatch::CubicMoisturePatch(const json &moisturePatchInfo, const string &filename)
    : MoisturePatch(moisturePatchInfo, new CubicPatch(moisturePatchInfo["shape"], filename))
{
    
}


CubicMoisturePatch::~CubicMoisturePatch()
{

}