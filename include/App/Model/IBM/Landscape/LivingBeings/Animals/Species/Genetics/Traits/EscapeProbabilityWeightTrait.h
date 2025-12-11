/**
 * @file EscapeProbabilityWeightTrait.h
 * @brief Defines the EscapeProbabilityWeightTrait class, representing traits that affect escape probability decisions in animals.
 */

#ifndef ESCAPE_PROBABILITY_WEIGHT_TRAIT_H_
#define ESCAPE_PROBABILITY_WEIGHT_TRAIT_H_


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DecisionsTrait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/DecisionsTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarTraitTemperatureSection.h"


/**
 * @class EscapeProbabilityWeightTrait
 * @brief Represents a trait that influences escape probability for behavioral decisions in animal species.
 *
 * Inherits from DecisionsTrait and provides methods for trait generation, retrieval of trait identifiers,
 * temperature-dependent section creation, and serialization.
 */
class EscapeProbabilityWeightTrait : public DecisionsTrait {
public:
    /**
     * @brief Generate a vector of EscapeProbabilityWeightTrait objects.
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
    EscapeProbabilityWeightTrait();

    /**
     * @brief Constructor with type and configuration.
     * @param individualLevelTraits Vector of individual-level traits.
     * @param type Type of EscapeProbabilityWeightTrait.
     * @param config JSON configuration.
     * @param individualLevelTraitsOrder Order of individual-level traits.
     */
    EscapeProbabilityWeightTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const EscapeProbabilityWeightType type, const nlohmann::json& config, const std::vector<nlohmann::json>& individualLevelTraitsOrder);
    
    /** @brief Destructor. */
    virtual ~EscapeProbabilityWeightTrait();

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

    /** @brief Get the file name associated with this trait. */
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

#endif // ESCAPE_PROBABILITY_WEIGHT_TRAIT_H_
