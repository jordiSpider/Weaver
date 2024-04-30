#include "IBM/World/Map/Patches/Moisture/GaussianMoisturePatch.h"

using namespace std;
using json = nlohmann::json;



GaussianMoisturePatch::GaussianMoisturePatch(const json &moisturePatchInfo, const string &filename)
    : MoisturePatch(moisturePatchInfo, new GaussianPatch(moisturePatchInfo["shape"], filename))
{
    
}


GaussianMoisturePatch::~GaussianMoisturePatch()
{

}

template <class Archive>
void GaussianMoisturePatch::serialize(Archive &ar, const unsigned int version) 
{

}
