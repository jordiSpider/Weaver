/**
 * @file NonDynamics.h
 * @brief Defines the NonDynamics class, representing static (non-changing)
 *        dynamics in the ecosystem model.
 *
 * This file contains the definition of the NonDynamics class, which implements
 * a type of dynamics where the value remains constant over time. It inherits
 * from NonMassBasedDynamics and overrides its methods to maintain the value
 * unchanged.
 *
 * NonDynamics is useful for traits or processes that do not evolve during the
 * simulation, such as constant metabolic costs or fixed behavioral modifiers.
 */

#ifndef NON_DYNAMICS_H
#define NON_DYNAMICS_H


#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>


#include <nlohmann/json.hpp>


#include "App/Model/IBM/Maths/Dynamics/NonMassBasedDynamics.h"


/**
 * @class NonDynamics
 * @brief Represents a non-changing dynamic component in the ecosystem model.
 *
 * This class implements a dynamics model where the associated value does not
 * change with time. All operations simply return the base value or perform
 * no action on update. It inherits from NonMassBasedDynamics and implements
 * its virtual interface with constant behavior.
 *
 * Typical use cases:
 * - Traits or parameters that are constant for an individual
 * - Fixed environmental or physiological factors
 * - Default or placeholder dynamics when no temporal evolution is required
 */
class NonDynamics : public NonMassBasedDynamics {
public:
    /**
     * @brief Default constructor.
     *
     * Initializes a NonDynamics object. No parameters are required since
     * the value remains constant.
     */
    NonDynamics();

    /**
     * @brief Virtual destructor.
     *
     * Ensures proper cleanup in derived classes.
     */
    virtual ~NonDynamics();

    /**
     * @brief Returns the initial value of the dynamics.
     *
     * @return A PreciseDouble equal to the base value.
     */
    PreciseDouble getInitialValue() const;

    /**
     * @brief Returns the current value of the dynamics.
     *
     * For NonDynamics, this method always returns the provided base value.
     *
     * @param baseValue Reference value for the process.
     * @return A PreciseDouble equal to baseValue.
     */
    PreciseDouble getValue(const PreciseDouble& baseValue) const;

    /**
     * @brief Updates the internal state of the dynamics.
     *
     * NonDynamics does not evolve over time, so this function performs no action.
     */
    void update();

    /**
     * @brief Returns a string with information about the dynamics.
     *
     * Useful for debugging or logging. Indicates that the dynamics is static.
     *
     * @param indentation String used for indentation in output.
     * @return A human-readable description of the dynamics.
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
};

#endif // NON_DYNAMICS_H

