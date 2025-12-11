/**
 * @file PulseDynamics.h
 * @brief Defines the PulseDynamics class, which models dynamic events occurring in pulses.
 *
 * This file contains the definition of the PulseDynamics class, a type of NonMassBasedDynamics
 * that represents phenomena with discrete pulse events over time. Each pulse may have a peak
 * value and occurs after a certain interval, which can be fixed or variable.
 *
 * Typical uses include modeling episodic processes, bursts of activity, or sudden changes
 * in environmental or physiological variables within the ecosystem model.
 */

#ifndef PULSE_DYNAMICS_H
#define PULSE_DYNAMICS_H


#include <boost/serialization/export.hpp>

#include <boost/serialization/utility.hpp>

#include <string>
#include <sstream>

#include <nlohmann/json.hpp>


#include "App/Model/IBM/Maths/Dynamics/NonMassBasedDynamics.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "App/Model/IBM/Maths/Random.h"
#include "App/Model/IBM/Maths/Dynamics/Decay/Decay.h"


/**
 * @class PulseDynamics
 * @brief Models dynamics that occur as discrete pulses over time.
 *
 * PulseDynamics implements a non-mass-based dynamics where the value changes in
 * episodic pulses rather than continuously. The timing and amplitude of each pulse
 * can be fixed or variable, and the class supports optional decay of pulse effects.
 *
 * Key features:
 * - Configurable pulse interval and peak value ranges
 * - Option for fixed or variable pulses
 * - Integrates with Decay objects for pulse amplitude changes over time
 * - Provides detailed information for logging and debugging
 */
class PulseDynamics : public NonMassBasedDynamics {
public:
    /**
     * @brief Default constructor.
     *
     * Initializes a PulseDynamics object with default parameters.
     */
    PulseDynamics();

    /**
     * @brief Constructs a PulseDynamics object from a JSON configuration.
     *
     * @param config JSON object containing pulse dynamics parameters.
     * @param timeStepsPerDay Number of simulation time steps in a day.
     */
    PulseDynamics(const nlohmann::json &config, const PreciseDouble& timeStepsPerDay);
    
    /**
     * @brief Constructs a PulseDynamics object from a JSON configuration and hypervolume.
     *
     * @param config JSON object containing pulse dynamics parameters.
     * @param timeStepsPerDay Number of simulation time steps in a day.
     * @param hyperVolume Scaling factor for the amplitude of pulses.
     */
    PulseDynamics(const nlohmann::json &config, const PreciseDouble& timeStepsPerDay, const PreciseDouble& hyperVolume);

    /**
     * @brief Virtual destructor.
     */
    virtual ~PulseDynamics();

    /**
     * @brief Returns the initial value before any pulse occurs.
     *
     * @return A PreciseDouble representing the initial state of the dynamics.
     */
    PreciseDouble getInitialValue() const;

    /**
     * @brief Returns the current value of the dynamics.
     *
     * The value may include contributions from ongoing pulses.
     *
     * @param baseValue Base value to use in calculations.
     * @return A PreciseDouble representing the current state.
     */
    PreciseDouble getValue(const PreciseDouble& baseValue) const;

    /**
     * @brief Updates the internal state of the dynamics.
     *
     * Advances the countdown for the next pulse event and applies pulse changes
     * if a pulse occurs.
     */
    void update();

    /**
     * @brief Provides a human-readable description of the pulse dynamics.
     *
     * Useful for debugging or logging simulation details.
     *
     * @param indentation String used to indent the output.
     * @return A string describing the current state of the dynamics.
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

protected:
    /// Range of time steps between consecutive pulses.
    std::pair<TimeStep, TimeStep> distanceBetweenPulsesRange;

    /// Whether the distance between pulses is fixed.
    bool fixedDistanceBetweenPulses;

    /// Countdown until the next pulse event.
    TimeStep nextPulseEventCountdown;

    /// Range of peak values for each pulse.
    std::pair<PreciseDouble, PreciseDouble> pulsePeakValueRange;

    /// Whether the pulse peak value is fixed.
    bool fixedPulsePeakValue;

    /// Optional decay applied to pulse amplitude.
    Decay* decay;


    /**
     * @brief Checks if the current time step corresponds to a pulse event.
     *
     * @return True if a pulse is occurring, false otherwise.
     */
    bool isOnPulseEvent() const;

    /**
     * @brief Computes the peak value for the current pulse.
     *
     * @return The peak amplitude of the current pulse as PreciseDouble.
     */
    PreciseDouble getPulsePeakValue() const;

    /**
     * @brief Determines the distance (in time steps) until the next pulse.
     *
     * @return Number of time steps until the next pulse event.
     */
    TimeStep getDistanceBetweenPulses() const;

    /**
     * @brief Returns a string describing the current pulse peak value.
     *
     * @return A string with human-readable information.
     */
    const std::string showPulsePeakValueInfo() const;

    /**
     * @brief Returns a string describing the current distance between pulses.
     *
     * @return A string with human-readable information.
     */
    const std::string showDistanceBetweenPulsesInfo() const;
};

#endif // PULSE_DYNAMICS_H

