/**
 * @file NonMassBasedDynamics.h
 * @brief Defines the NonMassBasedDynamics class, a Dynamics subclass not dependent on mass.
 *
 * This file defines a class that represents dynamics which do not depend
 * on the mass of the entity. It inherits from the abstract base class Dynamics
 * and can be serialized using Boost.
 */

#ifndef NON_MASS_BASED_DYNAMICS_H
#define NON_MASS_BASED_DYNAMICS_H


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <string>
#include <sstream>
#include <nlohmann/json.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "Misc/EnumClass.h"
#include "App/Model/IBM/Physics/Temperature.h"
#include "App/Model/IBM/Maths/Dynamics/Dynamics.h"


/**
 * @class NonMassBasedDynamics
 * @brief Represents dynamics that are independent of mass.
 *
 * NonMassBasedDynamics is a subclass of Dynamics for behaviors that
 * are not influenced by the mass of the organism or resource. This class
 * is abstract and should be subclassed to implement specific non-mass-dependent
 * dynamic behaviors.
 */
class NonMassBasedDynamics : public Dynamics {
public:
    /**
     * @brief Factory method to create a NonMassBasedDynamics instance from JSON configuration.
     * @param config JSON configuration defining the dynamics.
     * @param timeStepsPerDay The number of simulation time steps per day.
     * @return Pointer to a newly created NonMassBasedDynamics instance.
     */
    static NonMassBasedDynamics* createInstance(const nlohmann::json &config, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Default constructor for NonMassBasedDynamics.
     */
    NonMassBasedDynamics();

    /**
     * @brief Virtual destructor for NonMassBasedDynamics.
     */
    virtual ~NonMassBasedDynamics();

    /**
     * @brief Pure virtual method to get the value of the dynamics based on a base value.
     * @param baseValue The reference value for computing the dynamics.
     * @return The computed value as a PreciseDouble.
     */
    virtual PreciseDouble getValue(const PreciseDouble& baseValue) const=0;
    
    /**
     * @brief Serialize the NonMassBasedDynamics object.
     * @tparam Archive Type of archive (binary/text, input/output).
     * @param ar Archive to serialize with.
     * @param version Version of serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

protected:
    /**
     * @brief Internal helper to compute the dynamics value including environmental factors.
     * @param baseValue Base value of the trait.
     * @param timeStepsPerDay Simulation time steps per day.
     * @param temperature Current temperature.
     * @param moisture Current moisture value.
     * @param minRH Minimum relative humidity.
     * @param maxRH Maximum relative humidity.
     * @return Computed value as a PreciseDouble.
     */
    PreciseDouble getValue(const PreciseDouble& baseValue, const PreciseDouble& timeStepsPerDay, const Temperature& temperature, const PreciseDouble& moisture, const PreciseDouble& minRH, const PreciseDouble& maxRH) const;
};

#endif // NON_MASS_BASED_DYNAMICS_H

