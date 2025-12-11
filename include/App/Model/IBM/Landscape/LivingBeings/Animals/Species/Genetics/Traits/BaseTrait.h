/**
 * @file BaseTrait.h
 * @brief Defines the BaseTrait class, representing fundamental traits in the simulation.
 *
 * BaseTrait extends the Trait base class and provides functionality for
 * traits that represent fundamental characteristics of individuals.
 * It supports serialization, trait generation, and configuration via JSON.
 */

#ifndef BASE_TRAIT_H_
#define BASE_TRAIT_H_


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Trait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/BaseTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TempSizeRuleTraitTemperatureSection.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarTraitTemperatureSection.h"


/**
 * @class BaseTrait
 * @brief Represents a fundamental trait for individuals in the simulation.
 *
 * BaseTrait inherits from Trait and includes functionality to:
 * - Generate multiple traits from JSON configuration.
 * - Retrieve trait identifiers.
 * - Check trait properties such as inverse or strictly positive.
 * - Handle serialization and temperature-dependent trait sections.
 */
class BaseTrait : public Trait {
public:
    /**
     * @brief Generates a list of BaseTrait objects from JSON configuration.
     *
     * @param individualLevelTraits Vector of pointers to individual-level traits.
     * @param definitionConfig JSON object defining the traits.
     * @param individualLevelTraitsOrder Vector of JSON objects specifying the order of traits.
     * @return Vector of pointers to Trait objects created.
     */
    static std::vector<Trait*> generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& definitionConfig, const std::vector<nlohmann::json>& individualLevelTraitsOrder);

    /**
     * @brief Returns the string identifiers for all BaseTrait types.
     *
     * @return Vector of strings representing the trait identifiers.
     */
    static std::vector<std::string> getTraitStrVector();

    /**
     * @brief Default constructor.
     *
     * Initializes an empty BaseTrait object.
     */
    BaseTrait();

    /**
     * @brief Constructor with initialization parameters.
     *
     * @param individualLevelTraits Vector of pointers to individual-level traits.
     * @param type Type of BaseTrait.
     * @param config JSON object containing trait configuration.
     * @param individualLevelTraitsOrder Vector of JSON objects defining the order of traits.
     */
    BaseTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const BaseTraitType type, const nlohmann::json& config, const std::vector<nlohmann::json>& individualLevelTraitsOrder);
    
    /**
     * @brief Destructor.
     *
     * Cleans up resources used by the trait.
     */
    virtual ~BaseTrait();

    /**
     * @brief Checks whether the given trait type represents an inverse trait.
     *
     * @param type BaseTraitType to check.
     * @return True if the trait is inverse, false otherwise.
     */
    static bool isInverse(const BaseTraitType type);

    /**
     * @brief Checks whether the given trait type must always be strictly positive.
     *
     * @param type BaseTraitType to check.
     * @return True if the trait is strictly positive, false otherwise.
     */
    static bool isStrictlyPositive(const BaseTraitType type);

    /**
     * @brief Checks whether this trait can have negative values.
     *
     * @return True if the trait can be negative, false otherwise.
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
     * @brief Returns the JSON field name used for this trait.
     *
     * @return JSON field name as a std::string.
     */
    static std::string getJsonFieldName();

    /**
     * @brief Returns the file name associated with this trait.
     *
     * @return File name as a std::string.
     */
    std::string getFileName() const override;

    /**
     * @brief Returns the parent trait string identifier.
     *
     * @return Parent trait string.
     */
    std::string getParentTraitStr() const override;

private:
    /// The specific type of this BaseTrait.
    BaseTraitType type;

    /**
     * @brief Creates a temperature section for the trait from JSON configuration.
     *
     * This is used internally to initialize temperature-dependent aspects of the trait.
     *
     * @param individualLevelTraits Vector of pointers to individual-level traits.
     * @param type Type of BaseTrait.
     * @param temperatureConfig JSON object containing temperature configuration.
     * @param individualLevelTraitsOrder Vector defining order of traits.
     * @param traitStr Trait identifier string.
     * @param fileName File name associated with the trait.
     * @return Pointer to the created TraitTemperatureSection.
     */
    static TraitTemperatureSection* createTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const BaseTraitType type, const nlohmann::json& temperatureConfig, const std::vector<nlohmann::json>& individualLevelTraitsOrder, const std::string& traitStr, const std::string& fileName);
};

#endif // BASE_TRAIT_H_
