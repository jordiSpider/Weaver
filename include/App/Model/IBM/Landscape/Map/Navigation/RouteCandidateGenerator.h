#ifndef ROUTE_CANDIDATE_GENERATOR_H_
#define ROUTE_CANDIDATE_GENERATOR_H_

#include <vector>

#include "App/Model/IBM/Landscape/Map/Points/PointContinuous.h"
#include "Misc/Maths/PreciseDouble.h"

class Map;

class RouteCandidateGenerator
{
public:
	static std::vector<PointContinuous> generate(const Map& map, const PreciseDouble& animalBodySection);
};

#endif
