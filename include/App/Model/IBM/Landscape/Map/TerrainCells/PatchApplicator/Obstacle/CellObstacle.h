/**
 * @file CellObstacle.h
 * @brief Concrete implementation of the obstacle interface for terrain cells.
 *
 * This class implements the CellObstacleInterface and represents a specific
 * obstacle within a terrain cell. It inherits all functionality from the interface
 * and can be serialized for persistence.
 */

#ifndef CELL_OBSTACLE_H_
#define CELL_OBSTACLE_H_



#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Obstacle/CellObstacleInterface.h"


/**
 * @class CellObstacle
 * @brief Concrete obstacle element for a terrain cell.
 *
 * Implements the CellObstacleInterface to provide obstacle behavior in a terrain cell.
 * Supports patch application and serialization.
 *
 * @see CellObstacleInterface
 */
class CellObstacle : public CellObstacleInterface
{
public:
    /**
     * @brief Default constructor.
     *
     * Initializes the cell without obstacles and default patch priority.
     */
    CellObstacle();

    /**
     * @brief Constructor with obstacle patch priority.
     *
     * @param newObstaclePatchPriority Patch priority of the obstacle for this cell.
     */
    CellObstacle(const size_t newObstaclePatchPriority);

    /// Virtual destructor.
    virtual ~CellObstacle();

    /// Copy constructor deleted to prevent copying.
    CellObstacle(const CellObstacle&) = delete;

    /// Copy assignment deleted to prevent copying.
    CellObstacle& operator=(const CellObstacle&) = delete;

    /**
     * @brief Serialization method for Boost or other archives.
     *
     * @tparam Archive Archive type.
     * @param ar Archive object.
     * @param version Version number of the class.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* CELL_OBSTACLE_H_ */
