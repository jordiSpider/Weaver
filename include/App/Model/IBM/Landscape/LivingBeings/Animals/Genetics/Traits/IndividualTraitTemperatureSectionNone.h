/**
 * @file PawarIndividualTraitTemperatureSection.h
 * @brief Defines the PawarIndividualTraitTemperatureSection class for temperature-dependent trait modification using the Pawar model.
 *
 * This class implements a temperature-dependent phenotypic modification
 * based on the Pawar model, extending the IndividualTraitTemperatureSection.
 * It supports strict positivity constraints and optional inversion of the response.
 */

#ifndef INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_NONE_H_
#define INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_NONE_H_


#include <string>
#include <utility>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/utility.hpp>

#include "Misc/Maths/PreciseDouble.h"
#include "App/Model/IBM/Physics/Temperature.h"


class IndividualTraitTemperatureSectionNone {
public:
    /**
     * @brief Default constructor.
     */
    IndividualTraitTemperatureSectionNone();

    /**
     * @brief Destructor.
     */
    virtual ~IndividualTraitTemperatureSectionNone();

    /**
     * @brief Apply the temperature dependency to a trait value.
     * 
     * Computes the phenotypic value of a trait under the current environmental temperature
     * using the Pawar temperature performance curve.
     *
     * @param temperature Current environmental temperature.
     * @param traitValue Current trait value.
     * @param coefficientForMassAforMature Coefficient A for mature mass calculation.
     * @param scaleForMassBforMature Scale B for mature mass calculation.
     * @param tempFromLab Temperature from lab experiments.
     * @return Modified trait value considering temperature effects.
     */
    PreciseDouble applyTemperatureDependency(const Temperature& temperature, const PreciseDouble& traitValue,
        const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab
    ) const;

    /**
     * @brief Convert the temperature section to a string representation.
     * @return String representation of the Pawar temperature section.
     */
    std::string to_string() const;

    /**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_NONE_H_
