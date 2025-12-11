/**
 * @file PawarIndividualTraitTemperatureSection.h
 * @brief Defines the PawarIndividualTraitTemperatureSection class for temperature-dependent trait modification using the Pawar model.
 *
 * This class implements a temperature-dependent phenotypic modification
 * based on the Pawar model, extending the IndividualTraitTemperatureSection.
 * It supports strict positivity constraints and optional inversion of the response.
 */

#ifndef PAWAR_INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_H_
#define PAWAR_INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_H_


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTraitTemperatureSection.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarTraitTemperatureSection.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarElement.h"


/**
 * @class PawarIndividualTraitTemperatureSection
 * @brief Temperature-dependent section of an individual trait using the Pawar model.
 *
 * This class modifies an individual trait according to environmental temperature,
 * using a Pawar-based temperature performance curve. It allows optional inversion
 * of the curve and enforces strictly positive trait values if required.
 */
class PawarIndividualTraitTemperatureSection : public IndividualTraitTemperatureSection {
public:
    /**
     * @brief Returns a string representing "Not Available" (NA).
     * @return String "NA".
     */
    static std::string to_string_NA();

    /**
     * @brief Default constructor.
     */
    PawarIndividualTraitTemperatureSection();

    /**
     * @brief Constructor with initialization.
     * @param traitTemperatureSection Pointer to the corresponding TraitTemperatureSection.
     * @param elements Vector of Pawar elements defining the temperature response curve.
     * @param geneticValue Constitutive genetic value of the trait.
     * @param inverse Whether the temperature response curve should be inverted.
     * @param strictlyPositive Whether the resulting phenotypic values must remain strictly positive.
     */
    PawarIndividualTraitTemperatureSection(const TraitTemperatureSection* traitTemperatureSection, const CustomIndexedVector<PawarElement, PreciseDouble>& elements, const PreciseDouble& geneticValue, bool inverse, bool strictlyPositive);
    
    /**
     * @brief Destructor.
     */
    virtual ~PawarIndividualTraitTemperatureSection();

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
     * @brief Check if the trait value is inside restricted ranges.
     * @return True if inside allowed range, false otherwise.
     */
    bool isInsideRestrictedRanges() const;

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

protected:
    CustomIndexedVector<PawarElement, PreciseDouble> elements; /**< Vector of Pawar elements defining the TPC. */
    bool inverse; /**< Whether the temperature response curve should be inverted. */
    bool strictlyPositive; /**< Whether phenotypic values must remain strictly positive. */
    std::pair<Temperature,Temperature> temperatureRangeTPC; /**< Temperature range of the performance curve. */

    /**
     * @brief Get the Pawar trait temperature section.
     * @return Pointer to the PawarTraitTemperatureSection.
     */
    const PawarTraitTemperatureSection* getTraitTemperatureSection() const override;
};

#endif // PAWAR_INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_H_
