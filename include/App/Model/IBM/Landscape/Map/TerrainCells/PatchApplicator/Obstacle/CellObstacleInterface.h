/**
 * @file CellObstacleInterface.h
 * @brief Interface for obstacle elements within a terrain cell.
 *
 * This class represents the obstacle component of a terrain cell.
 * It tracks whether a cell contains an obstacle and if it is fully blocked.
 * It is used by PatchApplicator to apply obstacle patches and check
 * traversability in the landscape simulation.
 */

#ifndef CELL_OBSTACLE_INTERFACE_H_
#define CELL_OBSTACLE_INTERFACE_H_


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include <fstream>
#include <ostream>
#include <unordered_set>
#include <memory>

#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/CellElement.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/ObstacleSource.h"



/**
 * @class CellObstacleInterface
 * @brief Interface for managing obstacles in a terrain cell.
 *
 * Provides functionality to apply obstacle patches, query obstacle state,
 * and control obstacle attributes. Derived classes can implement further
 * behavior for specific obstacle types.
 *
 * @see CellElement
 */
class CellObstacleInterface : public CellElement
{
public:
    /**
     * @brief Default constructor.
     *
     * Initializes the cell without obstacles.
     */
    CellObstacleInterface();

    /**
     * @brief Constructor with obstacle patch priority.
     *
     * @param newObstaclePatchPriority Patch priority of the obstacle for this cell.
     */
    CellObstacleInterface(const size_t newObstaclePatchPriority);

    /// Virtual destructor.
    virtual ~CellObstacleInterface();

    /// Copy constructor deleted to prevent copying.
    CellObstacleInterface(const CellObstacleInterface&) = delete;

    /// Copy assignment deleted to prevent copying.
    CellObstacleInterface& operator=(const CellObstacleInterface&) = delete;

    /**
     * @brief Applies an obstacle patch to this cell.
     *
     * Updates the obstacle status of the cell based on the patch priority.
     *
     * @param obstaclePatchPriority Patch priority of the applied obstacle.
     */
    void applySource(const size_t obstaclePatchPriority);

    /**
     * @brief Checks if the cell contains any obstacle.
     * @return true if the cell has an obstacle, false otherwise.
     */
    bool isObstacle() const;

    /**
     * @brief Checks if the cell is fully blocked by an obstacle.
     * @return true if the obstacle fully blocks the cell, false otherwise.
     */
    bool isFullObstacle() const;

    /**
     * @brief Serialization method for Boost or other archives.
     *
     * @tparam Archive Archive type.
     * @param ar Archive object.
     * @param version Version number of the class.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    /// Indicates whether the cell contains an obstacle.
    bool obstacle;

    /// Indicates whether the obstacle fully blocks the cell.
    bool fullObstacle;

    /**
     * @brief Sets the obstacle value.
     *
     * @param newValue New obstacle status.
     */
    void setObstacleValue(const bool newValue);

    /**
     * @brief Sets whether the obstacle fully blocks the cell.
     *
     * @param newValue New full-obstacle status.
     */
    void setFullObstacleValue(const bool newValue);
};

#endif /* CELL_OBSTACLE_INTERFACE_H_ */
