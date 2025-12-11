/**
 * @file BranchCellObstacle.h
 * @brief Defines the BranchCellObstacle class for managing obstacles in branch terrain cells.
 */

#ifndef BRANCH_CELL_OBSTACLE_H_
#define BRANCH_CELL_OBSTACLE_H_


#include <vector>
#include <numeric>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Obstacle/CellObstacleInterface.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/ObstacleSource.h"


class BranchTerrainCell;


/**
 * @class BranchCellObstacle
 * @brief Obstacle representation for branch nodes in a spatial tree.
 *
 * This class aggregates obstacle information from child terrain cells of a branch
 * terrain cell. It provides flags for full obstacles and partial obstacles and
 * calculates the patch priority for updates.
 */
class BranchCellObstacle : public CellObstacleInterface
{
public:
    /// Default constructor.
    BranchCellObstacle();

    /**
     * @brief Constructor with initial patch priority.
     * @param newObstaclePatchPriority Patch priority for obstacle updates.
     */
    BranchCellObstacle(const size_t newObstaclePatchPriority);

    /// Destructor.
    virtual ~BranchCellObstacle();

    /// Deleted copy constructor
    BranchCellObstacle(const BranchCellObstacle&) = delete;

    /// Deleted copy assignment
    BranchCellObstacle& operator=(const BranchCellObstacle&) = delete;

    /**
     * @brief Aggregate obstacle data from child terrain cells.
     * @param newSummaryTerrainCell Pointer to the branch terrain cell to summarize.
     */
    void applyBranch(BranchTerrainCell* const newSummaryTerrainCell);

    /**
     * @brief Serialization function.
     * @tparam Archive Type of the archive.
     * @param ar Archive object.
     * @param version Version of serialization.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    /**
     * @brief Calculate the full obstacle flag based on child cells.
     * @param summaryTerrainCell Pointer to the branch terrain cell.
     * @return True if all child cells are full obstacles, false otherwise.
     */
    static bool calculateFullObstacleValue(BranchTerrainCell* const summaryTerrainCell);

    /**
     * @brief Calculate the obstacle flag based on child cells.
     * @param summaryTerrainCell Pointer to the branch terrain cell.
     * @return True if at least one child cell is an obstacle, false otherwise.
     */
    static bool calculateObstacleValue(BranchTerrainCell* const summaryTerrainCell);

    /**
     * @brief Calculate the patch priority for obstacle updates based on child cells.
     * @param summaryTerrainCell Pointer to the branch terrain cell.
     * @return Calculated patch priority.
     */
    static size_t calculateObstaclePatchPriority(BranchTerrainCell* const summaryTerrainCell);
};

#endif /* BRANCH_CELL_OBSTACLE_H_ */
