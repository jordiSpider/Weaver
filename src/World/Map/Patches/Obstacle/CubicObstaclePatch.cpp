#include "World/Map/Patches/Obstacle/CubicObstaclePatch.h"

using namespace std;
using json = nlohmann::json;



CubicObstaclePatch::CubicObstaclePatch(const json &obstaclePatchInfo, const string &filename)
    : ObstaclePatch(obstaclePatchInfo, new CubicPatch(obstaclePatchInfo["shape"], filename))
{
    
}


CubicObstaclePatch::~CubicObstaclePatch()
{

}