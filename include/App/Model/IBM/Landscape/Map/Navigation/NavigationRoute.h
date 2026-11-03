#ifndef NAVIGATION_ROUTE_H_
#define NAVIGATION_ROUTE_H_

#include <cstddef>
#include <vector>

#include "App/Model/IBM/Landscape/Map/Points/PointContinuous.h"
#include "App/Model/IBM/Landscape/Map/Points/PointMap.h"

struct NavigationWaypoint
{
	PointContinuous position;
	PointMap cell;
};

struct NavigationRoute
{
	std::vector<NavigationWaypoint> waypoints;
};

#endif
