#ifndef ROUTE_COST_POLICY_H_
#define ROUTE_COST_POLICY_H_

#include "Misc/Maths/PreciseDouble.h"
#include "App/Model/IBM/Landscape/Map/Points/PointContinuous.h"

class AnimalNonStatistical;
class TerrainCell;

class RouteCostPolicy
{
public:
	virtual ~RouteCostPolicy() = default;
	virtual PreciseDouble calculateTransitionCost(const TerrainCell& from, const TerrainCell& to, const AnimalNonStatistical& animal) const = 0;
	virtual PreciseDouble estimateRemainingCost(const TerrainCell& from, const TerrainCell& goal, const AnimalNonStatistical& animal) const = 0;
	virtual PreciseDouble calculateTransitionCost(const PointContinuous& fromPosition, const TerrainCell& from, const PointContinuous& toPosition, const TerrainCell& to, const AnimalNonStatistical& animal) const;
	virtual PreciseDouble estimateRemainingCost(const PointContinuous& fromPosition, const TerrainCell& from, const PointContinuous& goalPosition, const TerrainCell& goal, const AnimalNonStatistical& animal) const;
};

class EuclideanRouteCostPolicy final : public RouteCostPolicy
{
public:
	PreciseDouble calculateTransitionCost(const TerrainCell& from, const TerrainCell& to, const AnimalNonStatistical& animal) const override;
	PreciseDouble estimateRemainingCost(const TerrainCell& from, const TerrainCell& goal, const AnimalNonStatistical& animal) const override;
	PreciseDouble calculateTransitionCost(const PointContinuous& fromPosition, const TerrainCell& from, const PointContinuous& toPosition, const TerrainCell& to, const AnimalNonStatistical& animal) const override;
	PreciseDouble estimateRemainingCost(const PointContinuous& fromPosition, const TerrainCell& from, const PointContinuous& goalPosition, const TerrainCell& goal, const AnimalNonStatistical& animal) const override;
};

#endif
