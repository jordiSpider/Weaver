#ifndef ROUTE_PLANNER_H_
#define ROUTE_PLANNER_H_

#include <cstddef>
#include <vector>

#include "App/Model/IBM/Landscape/Map/Navigation/NavigationRoute.h"
#include "App/Model/IBM/Landscape/Map/Points/PointContinuous.h"
#include "Misc/Maths/PreciseDouble.h"

class AnimalNonStatistical;
class RouteCostPolicy;
class RouteDestinationSelectionPolicy;
class Map;
class TerrainCell;

struct RouteGoal
{
	const TerrainCell* cell;
	PointContinuous position;
	size_t priority;
};

struct RoutePlanResult
{
	NavigationRoute route;
	size_t goalIndex;
};

class RoutePlanner
{
public:
	virtual ~RoutePlanner() = default;
	virtual NavigationRoute planRoute(const Map& map, const TerrainCell& start, const TerrainCell& goal, const PointContinuous& startPosition, const PointContinuous& goalPosition, const AnimalNonStatistical& animal, const PreciseDouble& animalBodySection, const RouteCostPolicy& costPolicy) const = 0;
	virtual RoutePlanResult planRouteToBestGoal(const Map& map, const TerrainCell& start, const PointContinuous& startPosition, const std::vector<RouteGoal>& goals, const AnimalNonStatistical& animal, const PreciseDouble& animalBodySection, const RouteCostPolicy& costPolicy, const RouteDestinationSelectionPolicy& selectionPolicy) const = 0;
};

class AStarRoutePlanner final : public RoutePlanner
{
public:
	NavigationRoute planRoute(const Map& map, const TerrainCell& start, const TerrainCell& goal, const PointContinuous& startPosition, const PointContinuous& goalPosition, const AnimalNonStatistical& animal, const PreciseDouble& animalBodySection, const RouteCostPolicy& costPolicy) const override;
	RoutePlanResult planRouteToBestGoal(const Map& map, const TerrainCell& start, const PointContinuous& startPosition, const std::vector<RouteGoal>& goals, const AnimalNonStatistical& animal, const PreciseDouble& animalBodySection, const RouteCostPolicy& costPolicy, const RouteDestinationSelectionPolicy& selectionPolicy) const override;
};

#endif
