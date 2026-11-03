#include "App/Model/IBM/Landscape/Map/Navigation/RouteCandidateGenerator.h"

#include "App/Model/IBM/Landscape/Map/Map.h"

using namespace std;

vector<PointContinuous> RouteCandidateGenerator::generate(const Map& map, const PreciseDouble& animalBodySection)
{
	return map.generateRouteCandidatePositions(animalBodySection);
}
