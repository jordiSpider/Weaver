/**
 * @file PathSource.h
 * @brief Defines the PathSource abstract base class representing sources of landscape information.
 *
 * This file contains the declaration of the PathSource class, which serves as a
 * base for different types of sources used in landscapes, such as moisture,
 * obstacles, resources, or habitat domains.
 */

#ifndef PATH_SOURCE_H
#define PATH_SOURCE_H


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


class Landscape;

/**
 * @class PathSource
 * @brief Abstract base class for landscape data sources.
 *
 * PathSource represents any source of information used to define landscape features,
 * including moisture sources, obstacles, resource distributions, and habitat domains.
 * Derived classes must implement methods for displaying information and identifying
 * the source type.
 */
class PathSource {
public:
    /**
     * @enum Type
     * @brief Enumerates the possible types of landscape sources.
     */
    enum class Type {
        Moisture,        /**< Moisture source */
        Obstacle,        /**< Obstacle source */
        Resource,        /**< Resource source */
        HabitatDomain    /**< Habitat domain source */
    };

    /**
     * @brief Constructs a PathSource object.
     */
    PathSource();

    /**
     * @brief Virtual destructor.
     */
    virtual ~PathSource();

    /**
     * @brief Displays information about the path source.
     * @return A string describing the path source.
     */
    virtual std::string showInfo() const=0;

    /**
     * @brief Returns the type of this path source.
     * @return Type Enum indicating the type of the path source.
     */
    virtual Type getType() const=0;

    /**
     * @brief Serializes the object for persistence.
     * @tparam Archive Type of serialization archive.
     * @param ar Archive instance.
     * @param version Serialization version.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


#endif // PATH_SOURCE_H

