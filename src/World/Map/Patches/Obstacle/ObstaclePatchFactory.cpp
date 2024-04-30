#include "World/Map/Patches/Obstacle/ObstaclePatchFactory.h"


using namespace std;
using json = nlohmann::json;



unique_ptr<const ObstaclePatch> ObstaclePatchFactory::createInstance(const json &obstaclePatchInfo, const string &filename)
{
    switch(PatchFactory::Type::stringToEnumValue(obstaclePatchInfo["shape"]["type"])) {
		case PatchFactory::Type::Cubic: {
			return make_unique<const CubicObstaclePatch>(obstaclePatchInfo, filename);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}
