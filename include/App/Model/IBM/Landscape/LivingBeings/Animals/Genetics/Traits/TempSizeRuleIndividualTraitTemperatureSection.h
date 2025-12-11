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


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTraitTemperatureSection.h"
#include "App/Model/IBM/Maths/MathFunctions.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TempSizeRuleTraitTemperatureSection.h"


/**
 * @class TempSizeRuleIndividualTraitTemperatureSection
 * @brief Temperature-dependent section of an individual trait following the Temperature-Size Rule.
 *
 * This class modifies an individual trait according to the Temperature-Size Rule,
 * which generally describes how ectotherms grow smaller at higher temperatures.
 */
class TempSizeRuleIndividualTraitTemperatureSection : public IndividualTraitTemperatureSection {
public:
    /**
     * @brief Default constructor.
     */
    TempSizeRuleIndividualTraitTemperatureSection();

    /**
     * @brief Constructor with initialization.
     * @param traitTemperatureSection Pointer to the corresponding TraitTemperatureSection.
     */
    TempSizeRuleIndividualTraitTemperatureSection(const TraitTemperatureSection* traitTemperatureSection);
    
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
     * @brief Check if the trait value is inside restricted ranges.
     * @return True if inside allowed range, false otherwise.
     */
    bool isInsideRestrictedRanges() const;

    /**
     * @brief Convert the temperature section to a string representation.
     * @return String representation of the Temperature-Size Rule temperature section.
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

protected:
    /**
     * @brief Get the TempSizeRule trait temperature section.
     * @return Pointer to the TempSizeRuleTraitTemperatureSection.
     */
    const TempSizeRuleTraitTemperatureSection* getTraitTemperatureSection() const override;
};

#endif // TEMP_SIZE_RULE_INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_H_
