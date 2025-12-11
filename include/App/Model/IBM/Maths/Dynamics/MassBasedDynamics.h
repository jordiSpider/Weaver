/**
 * @file MassBasedDynamics.h
 * @brief Defines the MassBasedDynamics class, a Dynamics subclass based on mass.
 *
 * This file defines a class that represents dynamics whose calculations
 * depend on the mass of the entity. It inherits from the abstract base
 * class Dynamics and can be serialized using Boost.
 */

#ifndef MASS_BASED_DYNAMICS_H
#define MASS_BASED_DYNAMICS_H


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
 * @class MassBasedDynamics
 * @brief Represents dynamics based on mass for living beings or resources.
 *
 * MassBasedDynamics is a specialized subclass of Dynamics. Its behavior
 * depends on the mass of the organism or resource it is applied to.
 * This class can be extended to implement specific mass-dependent dynamics.
 */
class MassBasedDynamics : public Dynamics {
public:
    /**
     * @brief Default constructor for MassBasedDynamics.
     */
    MassBasedDynamics();

    /**
     * @brief Virtual destructor for MassBasedDynamics.
     */
    virtual ~MassBasedDynamics();
    
    /**
     * @brief Serialize the MassBasedDynamics object.
     * @tparam Archive Type of archive (binary/text, input/output).
     * @param ar Archive to serialize with.
     * @param version Version of serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif // MASS_BASED_DYNAMICS_H

