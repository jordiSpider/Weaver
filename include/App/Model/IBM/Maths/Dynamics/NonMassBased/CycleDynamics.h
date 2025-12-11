/**
 * @file CycleDynamics.h
 * @brief Defines the CycleDynamics class, which implements periodic non–mass-based dynamics.
 *
 * The CycleDynamics class models a repeating sequence of values (a cycle) that influences
 * dynamic traits or ecological processes within the Weaver simulation framework. These
 * dynamics do not depend on biomass; instead, they follow a discrete periodic pattern
 * that updates at each time step.
 *
 * Typical uses include seasonal activity cycles, periodic physiological adjustments,
 * or cyclical environmental effects.
 */

#ifndef CYCLE_DYNAMICS_H
#define CYCLE_DYNAMICS_H


#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>


#include <vector>
#include <nlohmann/json.hpp>


#include "App/Model/IBM/Maths/Dynamics/NonMassBasedDynamics.h"
#include "App/Model/IBM/Maths/Random.h"


/**
 * @class CycleDynamics
 * @brief Implements periodic, non–mass-based dynamic behavior.
 *
 * The CycleDynamics class models a cyclic sequence of values that repeat over time.
 * It is used for traits or ecological processes whose dynamics follow a discrete,
 * repeating pattern (e.g., seasonal activity cycles, periodic metabolic adjustments,
 * environmental oscillations).
 *
 * The class stores a vector of values representing the cycle and an internal index
 * that advances on each update. The dynamics do not depend on biomass or mass-based
 * parameters, consistent with the NonMassBasedDynamics interface.
 */
class CycleDynamics : public NonMassBasedDynamics {
public:
    /**
     * @brief Constructs an empty cycle with default initialization.
     *
     * The cycle is initially empty and must be configured before producing meaningful
     * values. The current index starts at zero.
     */
    CycleDynamics();

    /**
     * @brief Constructs a CycleDynamics object from JSON configuration.
     *
     * The JSON configuration typically contains the cycle values and optional parameters
     * defining how the periodic sequence should behave.
     *
     * @param config JSON configuration object.
     */
    CycleDynamics(const nlohmann::json &config);
    
    /**
     * @brief Constructs a CycleDynamics object using JSON configuration and a hypervolume.
     *
     * The hypervolume parameter may be used in contexts where the cycle depends on
     * spatial or ecological scaling factors.
     *
     * @param config JSON configuration object.
     * @param hyperVolume Additional scaling factor applied during initialization.
     */
    CycleDynamics(const nlohmann::json &config, const PreciseDouble& hyperVolume);

    /**
     * @brief Destructor.
     */
    virtual ~CycleDynamics();

    /**
     * @brief Returns the full vector of cycle values.
     *
     * @return A const reference to the internal vector representing the periodic sequence.
     */
    const std::vector<PreciseDouble>& getCycle() const;

    /**
     * @brief Returns the first value of the cycle.
     *
     * Useful for initializing state variables when the dynamics are first applied.
     *
     * @return Initial value of the cycle.
     */
    PreciseDouble getInitialValue() const;

    /**
     * @brief Computes the output value based on a base input and the current cycle state.
     *
     * The function may scale or modulate @p baseValue using the current cyclical factor.
     *
     * @param baseValue Input value to be transformed according to the cycle.
     * @return Resulting dynamic value after applying the cycle.
     */
    PreciseDouble getValue(const PreciseDouble& baseValue) const;

    /**
     * @brief Advances the cycle to the next position.
     *
     * Increments the internal index. When the end of the cycle is reached,
     * the index wraps around to the beginning, ensuring periodic behavior.
     */
    void update();

    /**
     * @brief Returns a human-readable description of the cycle configuration.
     *
     * @param indentation Prefix applied at the beginning of each line (useful for nesting).
     * @return Formatted string summarizing the cycle parameters and current state.
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
    /**
     * @brief Vector containing one full period of the dynamics.
     *
     * Each entry corresponds to a discrete step in the cycle. The cycle is traversed
     * sequentially, looping back to the beginning when finished.
     */
    std::vector<PreciseDouble> cycle;

    /**
     * @brief Index of the current position in the cycle.
     *
     * Ranges from 0 to cycle.size() - 1. Automatically wraps around after updates.
     */
    size_t currentIndex;
};

#endif // CYCLE_DYNAMICS_H

