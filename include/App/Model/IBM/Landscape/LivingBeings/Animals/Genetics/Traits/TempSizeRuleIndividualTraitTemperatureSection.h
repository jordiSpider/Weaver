/**
 * @file TempSizeRuleIndividualTraitTemperatureSection.h
 * @brief Defines the TempSizeRuleIndividualTraitTemperatureSection class for temperature-dependent trait modification following a temperature-size rule.
 *
 * This class implements a temperature-dependent modification of individual traits
 * based on the Temperature-Size Rule (TSR), extending IndividualTraitTemperatureSection.
 * It modifies traits according to environmental temperature to simulate TSR effects.
 */

#ifndef TEMP_SIZE_RULE_INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_H_
#define TEMP_SIZE_RULE_INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_H_


#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "App/Model/IBM/Physics/Temperature.h"

/// Forward declaration to avoid depending on the full TraitTemperatureSection hierarchy in this header.
class TempSizeRuleTraitTemperatureSection;


/**
 * @class TempSizeRuleIndividualTraitTemperatureSection
 * @brief Temperature-dependent section of an individual trait following the Temperature-Size Rule.
 *
 * This class modifies an individual trait according to the Temperature-Size Rule,
 * which generally describes how ectotherms grow smaller at higher temperatures.
 */
class TempSizeRuleIndividualTraitTemperatureSection {
public:
    /**
     * @brief Default constructor.
     */
    TempSizeRuleIndividualTraitTemperatureSection();

    /**
     * @brief Constructor with initialization.
     * @param traitTemperatureSection Pointer to the corresponding TempSizeRuleTraitTemperatureSection.
     */
    TempSizeRuleIndividualTraitTemperatureSection(const TempSizeRuleTraitTemperatureSection* traitTemperatureSection);
    
    /**
     * @brief Destructor.
     */
    virtual ~TempSizeRuleIndividualTraitTemperatureSection();

    /**
     * @brief Apply temperature dependency to a trait value following the Temperature-Size Rule.
     * 
     * Modifies the phenotypic trait value according to environmental temperature.
     *
     * @param temperature Current environmental temperature.
     * @param traitValue Current trait value.
     * @param coefficientForMassAforMature Coefficient A for mature mass calculation.
     * @param scaleForMassBforMature Scale B for mature mass calculation.
     * @param tempFromLab Temperature from laboratory experiments.
     * @return Modified trait value considering temperature effects.
     */
    PreciseDouble applyTemperatureDependency(const Temperature& temperature, const PreciseDouble& traitValue, 
        const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab
    ) const;

    /**
     * @brief Update the species-level temperature section this individual trait refers to.
     * @param newTraitTemperatureSection Pointer to the new TempSizeRuleTraitTemperatureSection.
     */
    void setTraitTemperatureSection(const TempSizeRuleTraitTemperatureSection* newTraitTemperatureSection);

    /**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    const TempSizeRuleTraitTemperatureSection* traitTemperatureSection = nullptr;
};

#endif // TEMP_SIZE_RULE_INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_H_
