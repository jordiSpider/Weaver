/**
 * @file NonDecay.h
 * @brief Defines the NonDecay class, representing a model with no decay effect.
 *
 * The NonDecay class is a concrete implementation of the abstract Decay class
 * where no decay is applied. It simply returns the input value unchanged.
 */

#ifndef NON_DECAY_H
#define NON_DECAY_H


#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Maths/Dynamics/Decay/Decay.h"


/**
 * @class NonDecay
 * @brief Represents a decay model with no decay effect.
 *
 * This class models a situation where the value remains constant over time.
 * It overrides the apply method to return the input value unchanged, effectively
 * disabling any decay process.
 */
class NonDecay : public Decay {
public:
    /**
     * @brief Default constructor for NonDecay.
     *
     * Initializes a non-decaying dynamic with default settings.
     */
    NonDecay();

    /**
     * @brief Destructor for NonDecay.
     */
    virtual ~NonDecay();

    /**
     * @brief Applies the non-decay effect to a given value.
     *
     * Since this is a non-decaying model, the output value is equal to the input value.
     *
     * @param currentValue The current value to process.
     * @return The same value as the input (no change applied).
     */
    PreciseDouble apply(const PreciseDouble& currentValue) const;

    /**
     * @brief Returns a textual description of the NonDecay instance.
     *
     * @param indentation A string used for indentation in formatted output.
     * @return A formatted string describing the decay model (indicating no decay).
     */
    std::string showInfo(const std::string& indentation) const;
    
    /**
     * @brief Serializes the NonDecay object for saving or loading.
     *
     * @tparam Archive Type of archive (binary/text, input/output).
     * @param ar Archive object.
     * @param version Version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif // NON_DECAY_H

