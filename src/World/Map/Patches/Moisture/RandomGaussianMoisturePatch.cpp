#include "World/Map/Patches/Moisture/RandomGaussianMoisturePatch.h"

using namespace std;
using json = nlohmann::json;



RandomGaussianMoisturePatch::RandomGaussianMoisturePatch(const json &moisturePatchInfo, const string &filename)
    : MoisturePatch(moisturePatchInfo, new RandomGaussianPatch(moisturePatchInfo["shape"], filename))
{
    
}


RandomGaussianMoisturePatch::~RandomGaussianMoisturePatch()
{

}