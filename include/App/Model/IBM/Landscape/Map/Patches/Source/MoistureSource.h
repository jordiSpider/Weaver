/**
 * @file MoistureSource.h
 * @brief Defines the MoistureSource class representing environmental moisture dynamics.
 *
 * This file contains the declaration of the MoistureSource class, which is a specialized
 * PathSource providing information about moisture availability, temperature, and
 * environmental conditions affecting animals in a landscape.
 */

#ifndef MOISTURE_SOURCE_H
#define MOISTURE_SOURCE_H


#include <boost/serialization/export.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <sstream>


#include "App/Model/IBM/Landscape/Map/Patches/Source/PathSource.h"

#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "App/Model/IBM/Physics/Temperature.h"

#include "App/Model/IBM/Maths/Dynamics/NonMassBasedDynamics.h"
#include "App/Model/IBM/Maths/Dynamics/NonMassBased/CycleDynamics.h"


/**
 * @class MoistureSource
 * @brief Represents a moisture source in the landscape.
 *
 * MoistureSource extends PathSource and provides data on environmental moisture,
 * temperature, and dynamics over time. It includes information on enemy-free and
 * competitor-free space, as well as methods to update these dynamics per timestep.
 */
class MoistureSource : public PathSource {
public:
    /**
     * @brief Default constructor.
     */
    MoistureSource();

    /**
     * @brief Constructs a MoistureSource object from JSON configuration.
     *
     * @param config JSON configuration describing the moisture source.
     * @param timeStepsPerDay Number of simulation timesteps per day for dynamics calculations.
     */
    MoistureSource(const nlohmann::json &config, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Destructor.
     */
    ~MoistureSource();

    /**
     * @brief Returns a string describing this moisture source.
     *
     * @return Description of the moisture source.
     */
    std::string showInfo() const;

    /**
     * @brief Returns the type of this path source.
     *
     * @return PathSource::Type::Moisture
     */
    Type getType() const;

    /**
     * @brief Returns the maximum resource capacity density of the moisture source.
     *
     * @return WetMass representing maximum moisture capacity.
     */
    const WetMass& getMaximumResourceCapacityDensity() const;
    
    /**
     * @brief Returns the current temperature at this moisture source.
     *
     * @return Temperature of the moisture source.
     */
    const Temperature& getTemperature() const;
    
    /**
     * @brief Returns the current relative humidity of the moisture source.
     *
     * @return PreciseDouble value of relative humidity.
     */
    const PreciseDouble& getMoisture() const;
    
    /**
     * @brief Checks if the area is free from enemy species.
     *
     * @return True if enemy-free, false otherwise.
     */
    bool isInEnemyFreeSpace() const;
    
    /**
     * @brief Checks if the area is free from competitor species.
     *
     * @return True if competitor-free, false otherwise.
     */
    bool isInCompetitorFreeSpace() const;

    /**
     * @brief Returns the dynamics controlling temperature over time.
     *
     * @return Reference to the CycleDynamics object representing temperature changes.
     */
    const CycleDynamics& getTemperatureDynamics() const;

    /**
     * @brief Updates the moisture source dynamics for the current timestep.
     */
    void update();

    /**
     * @brief Serializes the MoistureSource object.
     *
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

private:
    bool inEnemyFreeSpace;  /**< Indicates if the area is free of enemy species */
    bool inCompetitorFreeSpace; /**< Indicates if the area is free of competitor species */

    WetMass maximumResourceCapacityDensity; /**< Maximum resource (moisture) capacity */

    CycleDynamics temperatureDynamics; /**< Dynamics of temperature over time */
    Temperature currentTemperature; /**< Current temperature at this moisture source */

    NonMassBasedDynamics* relativeHumidityDynamics; /**< Dynamics controlling relative humidity */
    PreciseDouble currentRelativeHumidity; /**< Current relative humidity */
};


#endif // MOISTURE_SOURCE_H

