#include "App/Model/IBM/Landscape/Map/Navigation/RoutePlanner.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

#include "App/Model/IBM/Landscape/Map/Map.h"
#include "App/Model/IBM/Landscape/Map/Navigation/RouteCandidateGenerator.h"
#include "App/Model/IBM/Landscape/Map/Navigation/RouteCollisionValidator.h"
#include "App/Model/IBM/Landscape/Map/Navigation/RouteCostPolicy.h"
#include "App/Model/IBM/Landscape/Map/Navigation/DestinationSelectionPolicy.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"

using namespace std;

namespace
{
	struct SearchNode
	{
		size_t nodeIndex;
		double estimatedTotalCost;
		double cost;
		uint64_t order;
	};

	struct SearchNodeOrder
	{
		bool operator()(const SearchNode& left, const SearchNode& right) const
		{
			if (left.estimatedTotalCost == right.estimatedTotalCost)
			{
				return left.order > right.order;
			}
			return left.estimatedTotalCost > right.estimatedTotalCost;
		}
	};

}

NavigationRoute AStarRoutePlanner::planRoute(const Map& map, const TerrainCell& start, const TerrainCell& goal, const PointContinuous& startPosition, const PointContinuous& goalPosition, const AnimalNonStatistical& animal, const PreciseDouble& animalBodySection, const RouteCostPolicy& costPolicy) const
{
	NavigationRoute route;
	const RouteCollisionValidator collisionValidator(map);
	if (!collisionValidator.isPositionTraversable(startPosition, animalBodySection) || !collisionValidator.isPositionTraversable(goalPosition, animalBodySection))
	{
		return route;
	}

	if (startPosition.get<0>() == goalPosition.get<0>() && startPosition.get<1>() == goalPosition.get<1>())
	{
		route.waypoints.push_back({startPosition, PointMap(start.getPosition().getAxisValues())});
		return route;
	}

	const vector<PointContinuous> candidatePositions = RouteCandidateGenerator::generate(map, animalBodySection);

	struct Node
	{
		PointContinuous position;
		const TerrainCell* cell;
	};
	vector<Node> nodes;
	const auto addNode = [&](const PointContinuous& position)
	{
		if (!collisionValidator.isPositionTraversable(position, animalBodySection))
		{
			return;
		}
		const TerrainCell* const cell = map.getCell(position);
		if (cell != nullptr)
		{
			nodes.push_back({position, cell});
		}
	};

	addNode(startPosition);
	addNode(goalPosition);
	if (nodes.size() < 2u)
	{
		return NavigationRoute{};
	}
	const size_t startIndex = 0u;
	const size_t goalIndex = 1u;
	for (const PointContinuous& point : candidatePositions)
	{
		addNode(point);
	}

	priority_queue<SearchNode, vector<SearchNode>, SearchNodeOrder> frontier;
	vector<double> costSoFar(nodes.size(), numeric_limits<double>::infinity());
	vector<size_t> cameFrom(nodes.size(), numeric_limits<size_t>::max());
	uint64_t insertionOrder = 0u;
	costSoFar[startIndex] = 0.0;
	frontier.push({startIndex, costPolicy.estimateRemainingCost(startPosition, start, goalPosition, goal, animal).getValue(), 0.0, insertionOrder++});

	while (!frontier.empty())
	{
		const SearchNode current = frontier.top();
		frontier.pop();
		if (current.cost != costSoFar[current.nodeIndex])
		{
			continue;
		}
		if (current.nodeIndex == goalIndex)
		{
			vector<size_t> reversedPath;
			for (size_t nodeIndex = goalIndex; nodeIndex != numeric_limits<size_t>::max(); nodeIndex = cameFrom[nodeIndex])
			{
				reversedPath.push_back(nodeIndex);
				if (nodeIndex == startIndex)
				{
					break;
				}
			}
			if (reversedPath.back() != startIndex)
			{
				return NavigationRoute{};
			}
			reverse(reversedPath.begin(), reversedPath.end());
			for (const size_t nodeIndex : reversedPath)
			{
				const PointMap cellPosition(nodes[nodeIndex].cell->getPosition().getAxisValues());
				route.waypoints.push_back({nodes[nodeIndex].position, cellPosition});
			}
			return route;
		}

		const Node& currentNode = nodes[current.nodeIndex];
		for (size_t neighborIndex = 0u; neighborIndex < nodes.size(); ++neighborIndex)
		{
			if (neighborIndex == current.nodeIndex || !collisionValidator.isSegmentTraversable(currentNode.position, nodes[neighborIndex].position, animalBodySection))
			{
				continue;
			}

			const Node& neighbor = nodes[neighborIndex];
			const PreciseDouble transitionCost = costPolicy.calculateTransitionCost(currentNode.position, *currentNode.cell, neighbor.position, *neighbor.cell, animal);
			if (!isfinite(transitionCost.getValue()) || transitionCost < 0.0)
			{
				continue;
			}

			const double newCost = current.cost + transitionCost.getValue();
			if (newCost >= costSoFar[neighborIndex])
			{
				continue;
			}

			costSoFar[neighborIndex] = newCost;
			cameFrom[neighborIndex] = current.nodeIndex;
			const PreciseDouble remainingCost = costPolicy.estimateRemainingCost(neighbor.position, *neighbor.cell, goalPosition, goal, animal);
			frontier.push({neighborIndex, newCost + remainingCost.getValue(), newCost, insertionOrder++});
		}
	}

	return route;
}

RoutePlanResult AStarRoutePlanner::planRouteToBestGoal(const Map& map, const TerrainCell& start, const PointContinuous& startPosition, const vector<RouteGoal>& goals, const AnimalNonStatistical& animal, const PreciseDouble& animalBodySection, const RouteCostPolicy& costPolicy, const RouteDestinationSelectionPolicy& selectionPolicy) const
{
	RoutePlanResult result{NavigationRoute{}, numeric_limits<size_t>::max()};
	const RouteCollisionValidator collisionValidator(map);
	if (goals.empty() || !collisionValidator.isPositionTraversable(startPosition, animalBodySection))
	{
		return result;
	}

	struct Node
	{
		PointContinuous position;
		const TerrainCell* cell;
	};
	vector<Node> nodes;
	vector<vector<size_t>> nodeGoalIndices;
	const TerrainCell* const startCell = map.getCell(startPosition);
	if (startCell == nullptr)
	{
		return result;
	}
	nodes.push_back({startPosition, startCell});
	nodeGoalIndices.emplace_back();
	vector<size_t> goalNodeIndices(goals.size(), numeric_limits<size_t>::max());
	vector<double> routeCosts(goals.size(), numeric_limits<double>::infinity());
	vector<bool> goalSettled(goals.size(), false);
	size_t validGoalCount = 0u;

	for (size_t goalIndex = 0u; goalIndex < goals.size(); ++goalIndex)
	{
		const RouteGoal& goal = goals[goalIndex];
		if (goal.cell == nullptr || !collisionValidator.isPositionTraversable(goal.position, animalBodySection))
		{
			continue;
		}

		size_t nodeIndex = numeric_limits<size_t>::max();
		for (size_t existingIndex = 0u; existingIndex < nodes.size(); ++existingIndex)
		{
			if (nodes[existingIndex].position.get<0>() == goal.position.get<0>()
				&& nodes[existingIndex].position.get<1>() == goal.position.get<1>())
			{
				nodeIndex = existingIndex;
				break;
			}
		}
		if (nodeIndex == numeric_limits<size_t>::max())
		{
			nodeIndex = nodes.size();
			nodes.push_back({goal.position, goal.cell});
			nodeGoalIndices.emplace_back();
		}
		goalNodeIndices[goalIndex] = nodeIndex;
		nodeGoalIndices[nodeIndex].push_back(goalIndex);
		++validGoalCount;
	}
	if (validGoalCount == 0u)
	{
		return result;
	}

	const vector<PointContinuous> candidatePositions = RouteCandidateGenerator::generate(map, animalBodySection);
	for (const PointContinuous& point : candidatePositions)
	{
		if (!collisionValidator.isPositionTraversable(point, animalBodySection))
		{
			continue;
		}
		const TerrainCell* const cell = map.getCell(point);
		if (cell != nullptr)
		{
			nodes.push_back({point, cell});
			nodeGoalIndices.emplace_back();
		}
	}

	priority_queue<SearchNode, vector<SearchNode>, SearchNodeOrder> frontier;
	vector<double> costSoFar(nodes.size(), numeric_limits<double>::infinity());
	vector<size_t> cameFrom(nodes.size(), numeric_limits<size_t>::max());
	vector<bool> settled(nodes.size(), false);
	uint64_t insertionOrder = 0u;
	costSoFar[0u] = 0.0;
	frontier.push({0u, 0.0, 0.0, insertionOrder++});
	size_t settledGoalCount = 0u;

	while (!frontier.empty())
	{
		const SearchNode current = frontier.top();
		frontier.pop();
		if (current.cost != costSoFar[current.nodeIndex] || settled[current.nodeIndex])
		{
			continue;
		}
		settled[current.nodeIndex] = true;
		for (const size_t goalIndex : nodeGoalIndices[current.nodeIndex])
		{
			goalSettled[goalIndex] = true;
			routeCosts[goalIndex] = current.cost;
			++settledGoalCount;
		}
		if (settledGoalCount == validGoalCount)
		{
			break;
		}

		const Node& currentNode = nodes[current.nodeIndex];
		for (size_t neighborIndex = 0u; neighborIndex < nodes.size(); ++neighborIndex)
		{
			if (neighborIndex == current.nodeIndex || settled[neighborIndex]
				|| !collisionValidator.isSegmentTraversable(currentNode.position, nodes[neighborIndex].position, animalBodySection))
			{
				continue;
			}

			const Node& neighbor = nodes[neighborIndex];
			const PreciseDouble transitionCost = costPolicy.calculateTransitionCost(currentNode.position, *currentNode.cell, neighbor.position, *neighbor.cell, animal);
			if (!isfinite(transitionCost.getValue()) || transitionCost < 0.0)
			{
				continue;
			}

			const double newCost = current.cost + transitionCost.getValue();
			if (newCost >= costSoFar[neighborIndex])
			{
				continue;
			}
			costSoFar[neighborIndex] = newCost;
			cameFrom[neighborIndex] = current.nodeIndex;
			frontier.push({neighborIndex, newCost, newCost, insertionOrder++});
		}
	}

	vector<size_t> reachableGoalIndices;
	for (size_t goalIndex = 0u; goalIndex < goalSettled.size(); ++goalIndex)
	{
		if (goalSettled[goalIndex])
		{
			reachableGoalIndices.push_back(goalIndex);
		}
	}
	const size_t selectedGoalIndex = selectionPolicy.selectGoal(goals, reachableGoalIndices, routeCosts);
	if (selectedGoalIndex >= goals.size() || !goalSettled[selectedGoalIndex])
	{
		return result;
	}

	vector<size_t> reversedPath;
	for (size_t nodeIndex = goalNodeIndices[selectedGoalIndex]; nodeIndex != numeric_limits<size_t>::max(); nodeIndex = cameFrom[nodeIndex])
	{
		reversedPath.push_back(nodeIndex);
		if (nodeIndex == 0u)
		{
			break;
		}
	}
	if (reversedPath.back() != 0u)
	{
		return result;
	}
	reverse(reversedPath.begin(), reversedPath.end());
	for (const size_t nodeIndex : reversedPath)
	{
		const PointMap cellPosition(nodes[nodeIndex].cell->getPosition().getAxisValues());
		result.route.waypoints.push_back({nodes[nodeIndex].position, cellPosition});
	}
	result.goalIndex = selectedGoalIndex;
	return result;
}