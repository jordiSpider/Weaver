#include "App/Model/IBM/Landscape/Map/Navigation/RouteCostPolicy.h"

#include "App/Model/IBM/Landscape/Map/Geometry/Geometry.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"

using namespace std;

PreciseDouble EuclideanRouteCostPolicy::calculateTransitionCost(const TerrainCell& from, const TerrainCell& to, const AnimalNonStatistical&) const
{
	return Geometry::calculateDistanceBetweenPoints(from.getCenter(), to.getCenter());
}

PreciseDouble EuclideanRouteCostPolicy::estimateRemainingCost(const TerrainCell& from, const TerrainCell& goal, const AnimalNonStatistical&) const
{
	return Geometry::calculateDistanceBetweenPoints(from.getCenter(), goal.getCenter());
}

PreciseDouble RouteCostPolicy::calculateTransitionCost(const PointContinuous&, const TerrainCell& from, const PointContinuous&, const TerrainCell& to, const AnimalNonStatistical& animal) const
{
	return calculateTransitionCost(from, to, animal);
}

PreciseDouble RouteCostPolicy::estimateRemainingCost(const PointContinuous&, const TerrainCell& from, const PointContinuous&, const TerrainCell& goal, const AnimalNonStatistical& animal) const
{
	return estimateRemainingCost(from, goal, animal);
}

PreciseDouble EuclideanRouteCostPolicy::calculateTransitionCost(const PointContinuous& fromPosition, const TerrainCell&, const PointContinuous& toPosition, const TerrainCell&, const AnimalNonStatistical&) const
{
	return Geometry::calculateDistanceBetweenPoints(fromPosition, toPosition);
}

PreciseDouble EuclideanRouteCostPolicy::estimateRemainingCost(const PointContinuous& fromPosition, const TerrainCell&, const PointContinuous& goalPosition, const TerrainCell&, const AnimalNonStatistical&) const
{
	return Geometry::calculateDistanceBetweenPoints(fromPosition, goalPosition);
}
