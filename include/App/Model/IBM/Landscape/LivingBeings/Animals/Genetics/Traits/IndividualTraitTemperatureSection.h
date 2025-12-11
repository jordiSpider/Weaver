/**
 * @file IndividualTraitTemperatureSection.h
 * @brief Defines the IndividualTraitTemperatureSection class representing the temperature-dependent section of an individual trait.
 *
 * This class handles the temperature-dependent modifications for an individual trait,
 * allowing tuning of phenotypic values according to environmental temperature
 * and providing checks against restricted ranges.
 */

#ifndef INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_H_
#define INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_H_


#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "App/Model/IBM/Physics/Temperature.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TraitTemperatureSection.h"


/**
 * @class IndividualTraitTemperatureSection
 * @brief Base class for handling temperature-dependent effects on an individual trait.
 *
 * IndividualTraitTemperatureSection encapsulates the logic to modify a trait based on
 * environmental temperature, to compute temperature-dependent phenotypic values,
 * and to check whether the trait remains within restricted ranges.
 */
class IndividualTraitTemperatureSection {
public:
    /**
     * @brief Returns a string representing "Not Available" (NA).
     * @return String "NA".
     */
    static std::string to_string_NA();

    /**
     * @brief Default constructor.
     */
    IndividualTraitTemperatureSection();

    /**
     * @brief Constructor initializing the section with a trait temperature reference.
     * @param traitTemperatureSection Pointer to the corresponding TraitTemperatureSection.
     */
    IndividualTraitTemperatureSection(const TraitTemperatureSection* traitTemperatureSection);
    
    /**
     * @brief Destructor.
     */
    virtual ~IndividualTraitTemperatureSection();

    /**
     * @brief Set a new TraitTemperatureSection.
     * @param newTraitTemperatureSection Pointer to the new TraitTemperatureSection.
     */
    void setTraitTemperatureSection(const TraitTemperatureSection* newTraitTemperatureSection);

    /**
     * @brief Convert the temperature section to a string representation.
     * @return String representation of the temperature section.
     */
    virtual std::string to_string() const=0;

    /**
     * @brief Apply the temperature dependency to a trait value.
     * 
     * Computes the phenotypic value of a trait under the current environmental temperature.
     *
     * @param temperature Current environmental temperature.
     * @param traitValue Current trait value.
     * @param coefficientForMassAforMature Coefficient A for mature mass calculation.
     * @param scaleForMassBforMature Scale B for mature mass calculation.
     * @param tempFromLab Temperature from lab experiments.
     * @return Modified trait value considering temperature effects.
     */
    virtual PreciseDouble applyTemperatureDependency(const Temperature& temperature, const PreciseDouble& traitValue,
        const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab
    ) const=0;

    /**
     * @brief Check if the trait value is inside restricted ranges.
     * @return True if inside the allowed range, false otherwise.
     */
    virtual bool isInsideRestrictedRanges() const=0;

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
    const TraitTemperatureSection* traitTemperatureSection; /**< Reference to the trait temperature section. */

    /**
     * @brief Get the trait temperature section.
     * @return Pointer to the TraitTemperatureSection.
     */
    virtual const TraitTemperatureSection* getTraitTemperatureSection() const;
};

#endif // INDIVIDUAL_TRAIT_TEMPERATURE_SECTION_H_
