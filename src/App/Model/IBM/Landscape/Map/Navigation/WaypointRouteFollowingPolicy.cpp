#include "App/Model/IBM/Landscape/Map/Navigation/RouteFollowingPolicy.h"

#include "App/Model/IBM/Landscape/Map/Geometry/Geometry.h"
#include "App/Model/IBM/Landscape/Map/Map.h"
#include "App/Model/IBM/Landscape/Map/Navigation/RouteCollisionValidator.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"

using namespace std;

RouteFollowingResult WaypointRouteFollowingPolicy::advance(const NavigationRoute& route, const size_t nextCellIndex, const Map& map, const PointContinuous& currentPosition, const PointContinuous& destination, const PreciseDouble& remainingDistance, const PreciseDouble& animalBodySection) const
{
	RouteFollowingResult result{currentPosition, map.getCell(currentPosition), nextCellIndex, false, false};
	const RouteCollisionValidator collisionValidator(map);
	if (route.waypoints.empty())
	{
		result.routeBlocked = true;
		return result;
	}

	PointContinuous position = currentPosition;
	size_t routeIndex = nextCellIndex;

	PointContinuous waypoint;
	TerrainCell* waypointCell = nullptr;
	const bool destinationWaypoint = routeIndex >= route.waypoints.size();

	if (destinationWaypoint)
	{
		waypoint = destination;
		waypointCell = map.getCell(destination);
	}
	else
	{
		waypointCell = map.getCell(route.waypoints[routeIndex].cell);
		if (waypointCell != nullptr)
		{
			waypoint = route.waypoints[routeIndex].position;
		}
	}

	if (waypointCell == nullptr || !collisionValidator.isPositionTraversable(waypoint, animalBodySection))
	{
		result.routeBlocked = true;
		return result;
	}

	const PreciseDouble distanceToWaypoint = Geometry::calculateDistanceBetweenPoints(position, waypoint);
	if (distanceToWaypoint <= remainingDistance)
	{
		if (!collisionValidator.isSegmentTraversable(position, waypoint, animalBodySection))
		{
			result.routeBlocked = true;
			return result;
		}
		position = waypoint;

		if (destinationWaypoint)
		{
			result.position = position;
			result.terrainCell = waypointCell;
			result.nextCellIndex = routeIndex;
			result.atDestination = true;
			return result;
		}

		++routeIndex;
			
		result.position = position;
		result.terrainCell = map.getCell(position);
		result.nextCellIndex = routeIndex;
		result.atDestination = routeIndex >= route.waypoints.size();
		return result;
	}

	PointContinuous partialPosition = position;
	const PreciseDouble fraction = remainingDistance / distanceToWaypoint;
	for (unsigned char axis = 0; axis < DIMENSIONS; ++axis)
	{
		const PreciseDouble currentAxisValue = getPositionAxisValue(position, axis);
		const PreciseDouble waypointAxisValue = getPositionAxisValue(waypoint, axis);
		setPositionAxisValue(partialPosition, axis, currentAxisValue + (waypointAxisValue - currentAxisValue) * fraction);
	}

	TerrainCell* partialCell = map.getCell(partialPosition);
	if (partialCell == nullptr || !collisionValidator.isSegmentTraversable(position, partialPosition, animalBodySection))
	{
		result.routeBlocked = true;
		return result;
	}

	result.position = partialPosition;
	result.terrainCell = partialCell;
	result.nextCellIndex = routeIndex;
	return result;
}
