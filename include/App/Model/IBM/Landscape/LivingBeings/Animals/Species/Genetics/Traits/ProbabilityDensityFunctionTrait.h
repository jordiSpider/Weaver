/**
 * @file ProbabilityDensityFunctionTrait.h
 * @brief Defines the ProbabilityDensityFunctionTrait class, a specialized Trait representing
 *        probability density functions for animal traits.
 */

#ifndef PROBABILITY_DENSITY_FUNCTION_TRAIT_H_
#define PROBABILITY_DENSITY_FUNCTION_TRAIT_H_


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Trait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/ProbabilityDensityFunctionTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarTraitTemperatureSection.h"


/**
 * @class ProbabilityDensityFunctionTrait
 * @brief Represents a trait defined by a probability density function (PDF).
 *
 * This class extends Trait and provides specialized functions to generate traits, 
 * retrieve trait identifiers, and handle temperature-dependent sections.
 */
class ProbabilityDensityFunctionTrait : public Trait {
public:
    /**
     * @brief Generate a vector of ProbabilityDensityFunctionTrait objects.
     * @param individualLevelTraits Vector of individual-level traits used to construct these traits.
     * @param definitionConfig JSON configuration defining the traits.
     * @param individualLevelTraitsOrder Order of individual-level traits in JSON.
     * @return Vector of generated Trait pointers.
     */
    static std::vector<Trait*> generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& definitionConfig, const std::vector<nlohmann::json>& individualLevelTraitsOrder);

    /**
     * @brief Get a vector of string identifiers for these traits.
     * @return Vector of trait strings.
     */
    static std::vector<std::string> getTraitStrVector();

    /** @brief Default constructor. */
    ProbabilityDensityFunctionTrait();

    /**
     * @brief Constructor with type and configuration.
     * @param individualLevelTraits Vector of individual-level traits.
     * @param type Type of PDF trait.
     * @param config JSON configuration.
     * @param individualLevelTraitsOrder Order of individual-level traits.
     */
    ProbabilityDensityFunctionTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const ProbabilityDensityFunctionTraitType type, const nlohmann::json& config, const std::vector<nlohmann::json>& individualLevelTraitsOrder);

    /** @brief Destructor. */
    virtual ~ProbabilityDensityFunctionTrait();

    /**
     * @brief Determine if the trait can have negative values.
     * @return True if negative values are allowed.
     */
    bool canBeNegative() const;

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
     * @brief Get the JSON field name associated with this trait.
     * @return JSON field name as a string.
     */
    static std::string getJsonFieldName();

    /** @brief Get the file name associated with the trait. */
    std::string getFileName() const override;

    /** @brief Get the parent trait string identifier. */
    std::string getParentTraitStr() const override;

private:
    /**
     * @brief Create the temperature section for this trait.
     * @param individualLevelTraits Vector of individual-level traits.
     * @param temperatureConfig JSON configuration for temperature section.
     * @param individualLevelTraitsOrder Order of individual-level traits.
     * @param traitStr Trait string identifier.
     * @param fileName File name for this trait.
     * @return Pointer to the created TraitTemperatureSection.
     */
    static TraitTemperatureSection* createTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& temperatureConfig, const std::vector<nlohmann::json>& individualLevelTraitsOrder, const std::string& traitStr, const std::string& fileName);
};

#endif // PROBABILITY_DENSITY_FUNCTION_TRAIT_H_
