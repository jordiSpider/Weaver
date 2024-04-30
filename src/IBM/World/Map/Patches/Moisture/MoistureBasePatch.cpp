#include "IBM/World/Map/Patches/Moisture/MoistureBasePatch.h"

using namespace std;
using json = nlohmann::json;



MoistureBasePatch::MoistureBasePatch(const json &moisturePatchInfo)
    : HomogeneousMoisturePatch(0, moisturePatchInfo, "Moisture base patch")
{
    
}


MoistureBasePatch::~MoistureBasePatch()
{

}

template <class Archive>
void MoistureBasePatch::serialize(Archive &ar, const unsigned int version) 
{
    
}
