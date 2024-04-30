#include "IBM/World/Map/Patches/Obstacle/ObstaclePatch.h"

using namespace std;
using json = nlohmann::json;



ObstaclePatch::ObstaclePatch(const json &obstacleInfo, const FormPatch* const &form)
    : Patch(obstacleInfo["priority"], form)
{
    
}


ObstaclePatch::~ObstaclePatch()
{
	
}

const string ObstaclePatch::getContentType() const
{
	return string("obstacle");
}

const string ObstaclePatch::getContentInfo() const
{
	return fmt::format(
		" - Obstacle parameters:"
	);
}

template <class Archive>
void ObstaclePatch::serialize(Archive &ar, const unsigned int version) 
{
	
}
