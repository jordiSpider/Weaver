/**
 * @file ResourceSource.h
 * @brief Defines the ResourceSource class representing resource availability in the landscape.
 *
 * This file contains the declaration of the ResourceSource class, which is a specialized
 * PathSource that provides information about resource species in a landscape,
 * including maximum capacity, growth dynamics, and edible fractions.
 */

#ifndef RESOURCE_SOURCE_H
#define RESOURCE_SOURCE_H



#include <boost/serialization/export.hpp>

#include <nlohmann/json.hpp>


#include "App/Model/IBM/Landscape/Map/Patches/Source/PathSource.h"

#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "App/Model/IBM/Maths/Dynamics/Dynamics.h"
#include "App/Model/IBM/Maths/Dynamics/NonMassBased/NonDynamics.h"


/**
 * @class ResourceSource
 * @brief Represents a resource source in the landscape.
 *
 * ResourceSource extends PathSource to manage information about a specific resource
 * species, including its maximum capacity, edible fraction, growth dynamics,
 * and whether it spreads in patches across the landscape.
 */
class ResourceSource : public PathSource {
public:
    /**
     * @brief Default constructor.
     */
    ResourceSource();

    /**
     * @brief Constructor with associated resource species.
     * @param species Pointer to the resource species.
     */
    ResourceSource(ResourceSpecies* const species);
    
    /**
     * @brief Constructor using JSON configuration.
     * @param config JSON configuration object.
     * @param species Pointer to the associated resource species.
     * @param hyperVolume Hypervolume of the landscape.
     * @param scaleMass Scaling mass used in calculations.
     * @param timeStepsPerDay Number of time steps per day.
     */
    ResourceSource(const nlohmann::json &config, ResourceSpecies* const species, const PreciseDouble& hyperVolume, const PreciseDouble& scaleMass, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Destructor.
     */
    ~ResourceSource();

    /**
     * @brief Returns the ID of the associated resource species.
     * @return ResourceSpecies::ResourceID
     */
    const ResourceSpecies::ResourceID& getResourceSpeciesId() const;
    
    /**
     * @brief Returns the maximum capacity of the resource.
     * @return WetMass Maximum biomass of the resource.
     */
    const WetMass& getResourceMaximumCapacity() const;
    
    /**
     * @brief Returns the fraction of the maximum capacity that is edible.
     * @return PreciseDouble Edible fraction of maximum carrying capacity.
     */
    const PreciseDouble& getEdibleFractionOfMaxCarryingCapacity() const;
    
    /**
     * @brief Returns the minimum biomass required to be considered edible.
     * @return WetMass Minimum edible biomass.
     */
    const WetMass& getMinimumEdibleBiomass() const;
    
    /**
     * @brief Indicates whether this resource spreads in patches.
     * @return true if patch spread is enabled, false otherwise.
     */
    bool getPatchSpread() const;

    /**
     * @brief Returns a string describing this resource source.
     * @return Description of the resource source.
     */
    std::string showInfo() const;

    /**
     * @brief Returns the type of this path source.
     * @return PathSource::Type::Resource
     */
    Type getType() const;

    /**
     * @brief Returns the growth dynamics associated with this resource.
     * @return Dynamics object managing the resource growth.
     */
    const Dynamics& getGrowthDynamics() const;

    /**
     * @brief Updates the resource state according to its growth dynamics.
     */
    void update();

    /**
     * @brief Serializes the object for persistence.
     * @tparam Archive Serialization archive type.
     * @param ar Archive instance.
     * @param version Serialization version.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

private:
    ResourceSpecies::ResourceID resourceSpeciesId; ///< ID of the resource species
    std::string resourceSpeciesName;               ///< Name of the resource species
    WetMass resourceMaximumCapacity;               ///< Maximum biomass of the resource
    PreciseDouble edibleFractionOfMaxCarryingCapacity; ///< Fraction of max capacity edible
    WetMass minimumEdibleBiomass;                  ///< Minimum biomass considered edible
    bool patchSpread;                              ///< Whether resource spreads in patches
    Dynamics* growthDynamics;                      ///< Pointer to growth dynamics object
};


#endif // RESOURCE_SOURCE_H

