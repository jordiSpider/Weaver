/**
 * @file Decay.h
 * @brief Defines the abstract Decay class for modeling decay processes.
 *
 * The Decay class represents decay dynamics for quantities such as mass or energy.
 * It provides a common interface for different types of decay, such as linear decay or no decay.
 */

#ifndef DECAY_H
#define DECAY_H


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

#include "Misc/EnumClass.h"
#include "App/Model/IBM/Maths/PreciseDouble.h"


/**
 * @class Decay
 * @brief Abstract base class for different decay models.
 *
 * Decay models define how a quantity decreases over time. Subclasses implement specific
 * decay laws, such as linear decay or no decay at all.
 */
class Decay {
public:
    /**
     * @brief Enumeration of decay types.
     */
    enum class Type : unsigned char
    {
        NonDecay, /**< No decay occurs. */
        Lineal    /**< Linear decay over time. */
    };

    /**
     * @brief Factory method to create a Decay instance from a JSON configuration.
     * @param config JSON object containing decay parameters.
     * @return Pointer to a Decay instance.
     */
    static Decay* createInstance(const nlohmann::json &config);
    
    /**
     * @brief Factory method to create a Decay instance with hyper-volume scaling.
     * @param config JSON object containing decay parameters.
     * @param hyperVolume Hyper-volume factor used for scaling.
     * @return Pointer to a Decay instance.
     */
    static Decay* createInstance(const nlohmann::json &config, const PreciseDouble& hyperVolume);

    /**
     * @brief Default constructor for Decay.
     */
    Decay();

    /**
     * @brief Virtual destructor for Decay.
     */
    virtual ~Decay();

    /**
     * @brief Applies the decay to a given value.
     * @param currentValue The current value to which decay will be applied.
     * @return The decayed value as PreciseDouble.
     */
    virtual PreciseDouble apply(const PreciseDouble& currentValue) const=0;

    /**
     * @brief Returns a string describing the decay instance.
     * @param indentation Indentation string for formatting output.
     * @return Formatted string with decay information.
     */
    virtual std::string showInfo(const std::string& indentation) const=0;
    
    /**
     * @brief Serializes the object using Boost serialization.
     * @tparam Archive Type of archive (binary/text, input/output).
     * @param ar Archive object.
     * @param version Version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


#endif // DECAY_H

