/**
 * @file GrowthRateDynamics.h
 * @brief Defines the GrowthRateDynamics class, a mass-based dynamics modeling growth rate.
 *
 * This file contains the definition of GrowthRateDynamics, which represents
 * dynamics based on the growth rate of organisms. It inherits from MassBasedDynamics
 * and implements temperature, moisture, and relative humidity dependent growth.
 */

#ifndef GROWTH_RATE_DYNAMICS_H
#define GROWTH_RATE_DYNAMICS_H


#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>


#include <vector>
#include <nlohmann/json.hpp>


#include "App/Model/IBM/Maths/Dynamics/MassBasedDynamics.h"
#include "App/Model/IBM/Maths/Parabola.h"
#include "App/Model/IBM/Maths/Constants.h"
#include "App/Model/IBM/Maths/MathFunctions.h"


/**
 * @class GrowthRateDynamics
 * @brief Implements mass-based dynamics for growth rate of organisms.
 *
 * GrowthRateDynamics is a subclass of MassBasedDynamics and calculates the growth
 * rate based on environmental conditions, including temperature, moisture, and relative humidity.
 * The implementation follows the model described in Savage et al. 2004, Fig. 4B.
 */
class GrowthRateDynamics : public MassBasedDynamics {
public:
    /**
     * @brief Default constructor for GrowthRateDynamics.
     */
    GrowthRateDynamics();

    /**
     * @brief Constructs a GrowthRateDynamics instance from JSON configuration.
     * @param config JSON object containing configuration parameters.
     * @param scaleMass Scaling factor for mass-based dynamics.
     * @param cellMass Wet mass of the cell or organism.
     * @param hyperVolume Hyper-volume parameter used for scaling.
     */
    GrowthRateDynamics(const nlohmann::json &config, const PreciseDouble& scaleMass, const WetMass& cellMass, const PreciseDouble& hyperVolume);

    /**
     * @brief Destructor for GrowthRateDynamics.
     */
    virtual ~GrowthRateDynamics();

    /**
     * @brief Returns the initial value of the growth rate dynamics.
     * @return Initial value as PreciseDouble.
     */
    PreciseDouble getInitialValue() const;

    /**
     * @brief Computes the value of the growth rate at a given state.
     * @param baseValue Base value for the dynamics.
     * @param timeStepsPerDay Number of simulation time steps per day.
     * @param temperature Current temperature.
     * @param moisture Current moisture value.
     * @param minRH Minimum relative humidity.
     * @param maxRH Maximum relative humidity.
     * @return Computed growth rate as PreciseDouble.
     */
    PreciseDouble getValue(const PreciseDouble& baseValue, const PreciseDouble& timeStepsPerDay, const Temperature& temperature, const PreciseDouble& moisture, const PreciseDouble& minRH, const PreciseDouble& maxRH) const;

    /**
     * @brief Updates the internal state of the dynamics.
     */
    void update();

    /**
     * @brief Returns a string describing the current state of the dynamics.
     * @param indentation Indentation string for formatting output.
     * @return Formatted string with information about the dynamics.
     */
    std::string showInfo(const std::string& indentation) const;
    
    /**
     * @brief Serializes the object for Boost serialization.
     * @tparam Archive Type of archive (binary/text, input/output).
     * @param ar Archive object.
     * @param version Version of serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

private:
    /** @brief Activation energy constant from Savage et al. 2004, Fig. 4B. */
	static constexpr double E = 0.63;

    /** @brief Initial value of the growth rate. */
    PreciseDouble initialValue;

    /** @brief Mass of the organism or cell. */
    PreciseDouble M;

    /** @brief Flag indicating if the intrinsic rate of increase is variable. */
    bool variableIntrinsicRateOfIncrease;

    // Thermal performance curve (TPC) parameters
    Temperature temperatureOptimal; /**< Optimal temperature for growth */
    PreciseDouble energyDecay;      /**< Energy decay coefficient */
    PreciseDouble activationEnergy; /**< Activation energy for temperature effect */
    Temperature temperatureRef;     /**< Reference temperature */

    /** @brief Scaling factor for rMax. */
    PreciseDouble rMaxScale;

    /**
     * @brief Computes the intrinsic rate of increase under given environmental conditions.
     * @param temperature Current temperature.
     * @param moisture Current moisture value.
     * @param minRH Minimum relative humidity.
     * @param maxRH Maximum relative humidity.
     * @return Rate of increase as PreciseDouble.
     */
    PreciseDouble getRateOfIncrease(const Temperature& temperature, const PreciseDouble& moisture, const PreciseDouble& minRH, const PreciseDouble& maxRH) const;
};


#endif // GROWTH_RATE_DYNAMICS_H

