#ifndef DESTINATION_SELECTION_POLICY_H_
#define DESTINATION_SELECTION_POLICY_H_

#include <cstddef>
#include <vector>

#include "App/Model/IBM/Landscape/Map/TerrainCells/CellValue.h"
#include "App/Model/IBM/Landscape/Map/Navigation/RoutePlanner.h"

class DestinationSelectionPolicy
{
public:
	virtual ~DestinationSelectionPolicy() = default;
	virtual std::vector<size_t> rankCandidates(const std::vector<CellValue>& candidates) const = 0;
};

class RandomizedCandidateSelectionPolicy final : public DestinationSelectionPolicy
{
public:
	std::vector<size_t> rankCandidates(const std::vector<CellValue>& candidates) const override;
};

class RouteDestinationSelectionPolicy
{
public:
	virtual ~RouteDestinationSelectionPolicy() = default;
	virtual size_t selectGoal(const std::vector<RouteGoal>& goals, const std::vector<size_t>& reachableGoalIndices, const std::vector<double>& routeCosts) const = 0;
};

class CandidatePriorityRouteSelectionPolicy final : public RouteDestinationSelectionPolicy
{
public:
	size_t selectGoal(const std::vector<RouteGoal>& goals, const std::vector<size_t>& reachableGoalIndices, const std::vector<double>& routeCosts) const override;
};

#endif
