#include "App/Model/IBM/Landscape/Map/Navigation/DestinationSelectionPolicy.h"

#include <algorithm>
#include <limits>
#include <numeric>

#include "App/Model/IBM/Maths/Random.h"

using namespace std;

vector<size_t> RandomizedCandidateSelectionPolicy::rankCandidates(const vector<CellValue>& candidates) const
{
	vector<size_t> candidateOrder(candidates.size());
	iota(candidateOrder.begin(), candidateOrder.end(), 0u);

	stable_sort(candidateOrder.begin(), candidateOrder.end(), [&candidates](const size_t left, const size_t right) {
		return candidates[left] > candidates[right];
	});

	size_t tieStart = 0u;
	while (tieStart < candidateOrder.size())
	{
		size_t tieEnd = tieStart + 1u;
		while (tieEnd < candidateOrder.size()
			&& !(candidates[candidateOrder[tieStart]] > candidates[candidateOrder[tieEnd]])
			&& !(candidates[candidateOrder[tieEnd]] > candidates[candidateOrder[tieStart]]))
		{
			++tieEnd;
		}

		for (size_t i = tieEnd; i > tieStart + 1u; --i)
		{
			swap(candidateOrder[i - 1u], candidateOrder[tieStart + Random::randomIndex(i - tieStart)]);
		}

		tieStart = tieEnd;
	}

	return candidateOrder;
}

size_t CandidatePriorityRouteSelectionPolicy::selectGoal(const vector<RouteGoal>& goals, const vector<size_t>& reachableGoalIndices, const vector<double>& routeCosts) const
{
	size_t selectedGoalIndex = numeric_limits<size_t>::max();
	for (const size_t goalIndex : reachableGoalIndices)
	{
		if (goalIndex >= goals.size() || goalIndex >= routeCosts.size())
		{
			continue;
		}
		if (selectedGoalIndex == numeric_limits<size_t>::max()
			|| goals[goalIndex].priority < goals[selectedGoalIndex].priority
			|| (goals[goalIndex].priority == goals[selectedGoalIndex].priority && routeCosts[goalIndex] < routeCosts[selectedGoalIndex]))
		{
			selectedGoalIndex = goalIndex;
		}
	}
	return selectedGoalIndex;
}
