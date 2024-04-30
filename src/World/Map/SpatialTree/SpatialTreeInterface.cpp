
#include "World/Map/SpatialTree/SpatialTreeInterface.h"

using namespace std;
using json = nlohmann::json;



SpatialTreeInterface::SpatialTreeInterface(const json &mapConfig, WorldInterface* const worldInterface)
    : Map(mapConfig, worldInterface)
{
    
}


SpatialTreeInterface::~SpatialTreeInterface()
{
    
}
