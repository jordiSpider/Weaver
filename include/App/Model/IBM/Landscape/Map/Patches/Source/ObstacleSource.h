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



#include <cstddef>
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
    struct PorosityParameters {
        double fraction = 0.0;
        double minimumPoreWidth = 0.0;
        double featureSize = 0.0;
        std::size_t octaves = 0;
    };

    /**
     * @brief Default constructor.
     */
    ObstacleSource();

    explicit ObstacleSource(const nlohmann::json& porosityConfig);

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

    const PorosityParameters& getPorosityParameters() const;

    bool hasPorosity() const;

    /**
     * @brief Returns the type of this path source.
     *
     * @return PathSource::Type::Obstacle
     */
    Type getType() const;

private:
    PorosityParameters porosityParameters;
};


#endif // OBSTACLE_SOURCE_H

