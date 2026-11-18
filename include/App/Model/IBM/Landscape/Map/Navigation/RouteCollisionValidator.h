#ifndef ROUTE_COLLISION_VALIDATOR_H_
#define ROUTE_COLLISION_VALIDATOR_H_

#include "App/Model/IBM/Landscape/Map/Points/PointContinuous.h"

class Map;

class RouteCollisionValidator
{
public:
	explicit RouteCollisionValidator(const Map& map);

	bool isPositionTraversable(const PointContinuous& position, const PreciseDouble& animalBodySection) const;

	bool isSegmentTraversable(const PointContinuous& start, const PointContinuous& end, const PreciseDouble& animalBodySection) const;

private:
	const Map& map;
};

#endif
