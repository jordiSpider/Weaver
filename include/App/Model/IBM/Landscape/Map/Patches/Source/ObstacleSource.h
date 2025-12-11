/**
 * @file ObstacleSource.h
 * @brief Defines the ObstacleSource class representing obstacles in a landscape.
 *
 * This file contains the declaration of the ObstacleSource class, which is a specialized
 * PathSource providing information about obstacles that affect animal movement
 * or resource accessibility in a landscape.
 */

#ifndef OBSTACLE_SOURCE_H
#define OBSTACLE_SOURCE_H



#include <nlohmann/json.hpp>


#include "App/Model/IBM/Landscape/Map/Patches/Source/PathSource.h"


/**
 * @class ObstacleSource
 * @brief Represents an obstacle source in the landscape.
 *
 * ObstacleSource extends PathSource and provides information about obstacles
 * that animals encounter. This can be used to prevent movement or block resources
 * in specific areas.
 */
class ObstacleSource : public PathSource {
public:
    /**
     * @brief Default constructor.
     */
    ObstacleSource();

    /**
     * @brief Destructor.
     */
    ~ObstacleSource();

    /**
     * @brief Returns a string describing this obstacle source.
     *
     * @return Description of the obstacle source.
     */
    std::string showInfo() const;

    /**
     * @brief Returns the type of this path source.
     *
     * @return PathSource::Type::Obstacle
     */
    Type getType() const;
};


#endif // OBSTACLE_SOURCE_H

