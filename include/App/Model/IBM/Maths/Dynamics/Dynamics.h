/**
 * @file Dynamics.h
 * @brief Defines the abstract Dynamics class for modeling temporal changes in traits or resources.
 *
 * This file contains the Dynamics base class, which represents
 * different types of temporal dynamics (Cycle, Pulse, GrowthRate)
 * in living beings or resources. It also provides static factory
 * methods to create specific instances based on JSON configuration.
 */

#ifndef DYNAMICS_H
#define DYNAMICS_H


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"
#include "Misc/EnumClass.h"
#include "App/Model/IBM/Physics/Temperature.h"


/**
 * @class Dynamics
 * @brief Abstract base class for modeling temporal dynamics.
 *
 * Dynamics represents a generic temporal change for traits or resources.
 * Derived classes must implement specific behavior such as cycles, pulses,
 * or growth rates.
 */
class Dynamics {
public:
    /**
     * @enum Type
     * @brief Types of dynamics.
     */
    enum class Type : unsigned char
    {
        Cycle,      /**< Represents a periodic cycle. */
        Pulse,      /**< Represents a sudden pulse or event. */
        GrowthRate  /**< Represents a continuous growth rate. */
    };

    /**
     * @brief Create a Dynamics instance based on a JSON configuration.
     * @param config JSON object containing the dynamics configuration.
     * @param timeStepsPerDay Number of time steps per day for scaling purposes.
     * @return Pointer to a newly created Dynamics object.
     */
    static Dynamics* createInstance(const nlohmann::json &config, const PreciseDouble& timeStepsPerDay);
    
    /**
     * @brief Create a Dynamics instance specifically for a resource.
     * @param config JSON object containing the dynamics configuration.
     * @param scaleMass Scaling factor for mass.
     * @param cellMass Wet mass of the cell.
     * @param timeStepsPerDay Number of time steps per day.
     * @param hyperVolume Hypervolume of the resource.
     * @return Pointer to a newly created Dynamics object.
     */
    static Dynamics* createInstanceForResource(const nlohmann::json &config, const PreciseDouble& scaleMass, const WetMass& cellMass, const PreciseDouble& timeStepsPerDay, const PreciseDouble& hyperVolume);

    /**
     * @brief Default constructor.
     */
    Dynamics();

    /**
     * @brief Virtual destructor.
     */
    virtual ~Dynamics();

    /**
     * @brief Get the initial value of the dynamics.
     * @return Initial value as PreciseDouble.
     */
    virtual PreciseDouble getInitialValue() const=0;

    /**
     * @brief Calculate the current value based on environment and parameters.
     * @param baseValue Base value for calculation.
     * @param timeStepsPerDay Number of time steps per day.
     * @param temperature Current temperature.
     * @param moisture Current moisture.
     * @param minRH Minimum relative humidity.
     * @param maxRH Maximum relative humidity.
     * @return Calculated value as PreciseDouble.
     */
    virtual PreciseDouble getValue(const PreciseDouble& baseValue, const PreciseDouble& timeStepsPerDay, const Temperature& temperature, const PreciseDouble& moisture, const PreciseDouble& minRH, const PreciseDouble& maxRH) const=0;

    /**
     * @brief Update the internal state of the dynamics.
     */
    virtual void update()=0;

    /**
     * @brief Return a string containing information about the dynamics.
     * @param indentation String used for indentation in formatted output.
     * @return Info string describing the dynamics.
     */
    virtual std::string showInfo(const std::string& indentation) const=0;
    
    /**
     * @brief Serialize the Dynamics object.
     * @tparam Archive Type of archive (binary/text, input/output).
     * @param ar Archive to serialize with.
     * @param version Version of serialization.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif // DYNAMICS_H

