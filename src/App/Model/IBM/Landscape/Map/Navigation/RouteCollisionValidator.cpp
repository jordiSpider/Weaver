#include "App/Model/IBM/Landscape/Map/Navigation/RouteCollisionValidator.h"

#include "App/Model/IBM/Landscape/Map/Map.h"

using namespace std;

RouteCollisionValidator::RouteCollisionValidator(const Map& map)
	: map(map)
{
}

bool RouteCollisionValidator::isPositionTraversable(const PointContinuous& position, const PreciseDouble& animalBodySection) const
{
	return map.isTraversable(position, animalBodySection);
}

bool RouteCollisionValidator::isSegmentTraversable(const PointContinuous& start, const PointContinuous& end, const PreciseDouble& animalBodySection) const
{
	return map.isTraversable(start, end, animalBodySection);
}

