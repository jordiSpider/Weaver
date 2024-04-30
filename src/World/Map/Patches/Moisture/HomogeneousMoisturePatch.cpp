#include "World/Map/Patches/Moisture/HomogeneousMoisturePatch.h"

using namespace std;
using json = nlohmann::json;



HomogeneousMoisturePatch::HomogeneousMoisturePatch(const json &moisturePatchInfo, const string &filename)
    : MoisturePatch(moisturePatchInfo, new HomogeneousPatch(filename))
{
    
}


HomogeneousMoisturePatch::HomogeneousMoisturePatch(const unsigned int priority, const nlohmann::json &moisturePatchInfo, const string &filename)
    : MoisturePatch(priority, moisturePatchInfo, new HomogeneousPatch(filename))
{

}


HomogeneousMoisturePatch::~HomogeneousMoisturePatch()
{

}