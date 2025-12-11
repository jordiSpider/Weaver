/**
 * @file LinealDecay.h
 * @brief Defines the LinealDecay class which implements linear decay dynamics.
 *
 * LinealDecay models a linear decrease of a quantity over time. It is derived from the abstract
 * Decay class and implements the apply method to reduce the value linearly based on a decay rate.
 */

#ifndef LINEAL_DECAY_H
#define LINEAL_DECAY_H


#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Maths/Dynamics/Decay/Decay.h"


/**
 * @class LinealDecay
 * @brief Implements linear decay dynamics.
 *
 * This class decreases a value linearly over time according to a specified decay rate.
 * It is derived from Decay and overrides the apply method.
 */
class LinealDecay : public Decay {
public:
    /**
     * @brief Default constructor.
     *
     * Initializes decay with default parameters.
     */
    LinealDecay();

    /**
     * @brief Constructor from JSON configuration.
     *
     * @param config JSON object containing decay parameters, such as decay rate.
     */
    LinealDecay(const nlohmann::json &config);
    
    /**
     * @brief Constructor from JSON configuration with hyper-volume scaling.
     *
     * @param config JSON object containing decay parameters.
     * @param hyperVolume Factor to scale decay rate according to hyper-volume.
     */
    LinealDecay(const nlohmann::json &config, const PreciseDouble& hyperVolume);

    /**
     * @brief Destructor for LinealDecay.
     */
    virtual ~LinealDecay();

    /**
     * @brief Apply the linear decay to a given value.
     *
     * The value is reduced linearly according to the decayOverTime parameter.
     *
     * @param currentValue The current value to decay.
     * @return The decayed value as PreciseDouble.
     */
    PreciseDouble apply(const PreciseDouble& currentValue) const;

    /**
     * @brief Returns a string describing the LinealDecay instance.
     *
     * @param indentation String used for indentation in the output.
     * @return Formatted string with decay information.
     */
    std::string showInfo(const std::string& indentation) const;
    
    /**
     * @brief Serializes the object using Boost serialization.
     *
     * @tparam Archive Type of archive (binary/text, input/output).
     * @param ar Archive object.
     * @param version Version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

private:
    PreciseDouble decayOverTime; /**< Linear decay rate applied per time unit. */
};

#endif // LINEAL_DECAY_H

