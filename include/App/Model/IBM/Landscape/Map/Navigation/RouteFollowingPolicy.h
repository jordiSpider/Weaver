#ifndef ROUTE_FOLLOWING_POLICY_H_
#define ROUTE_FOLLOWING_POLICY_H_

#include <cstddef>

#include "App/Model/IBM/Landscape/Map/Navigation/NavigationRoute.h"
#include "App/Model/IBM/Landscape/Map/Points/PointContinuous.h"
#include "Misc/Maths/PreciseDouble.h"

class Map;
class TerrainCell;

struct RouteFollowingResult
{
	PointContinuous position;
	TerrainCell* terrainCell;
	size_t nextCellIndex;
	bool atDestination;
	bool routeBlocked;
};

class RouteFollowingPolicy
{
public:
	virtual ~RouteFollowingPolicy() = default;
	virtual RouteFollowingResult advance(const NavigationRoute& route, size_t nextCellIndex, const Map& map, const PointContinuous& currentPosition, const PointContinuous& destination, const PreciseDouble& remainingDistance, const PreciseDouble& animalBodySection) const = 0;
};

class WaypointRouteFollowingPolicy final : public RouteFollowingPolicy
{
public:
	RouteFollowingResult advance(const NavigationRoute& route, size_t nextCellIndex, const Map& map, const PointContinuous& currentPosition, const PointContinuous& destination, const PreciseDouble& remainingDistance, const PreciseDouble& animalBodySection) const override;
};

#endif
