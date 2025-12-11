/**
 * @file Trait.h
 * @brief Defines the Trait class for modeling traits of individual-level and species-level genetics.
 */

#ifndef TRAIT_H_
#define TRAIT_H_

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <functional>


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TraitTemperatureSection.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/EnumClass.h"
#include "Misc/CustomIndexedVector.h"


/**
 * @class Trait
 * @brief Represents a trait of an animal species or individual.
 *
 * Traits can be base traits, weights for escape probability, predation probability,
 * edibility value, or other derived traits. Supports thermal dependence and serialization.
 */
class Trait {
public:
    /**
     * @brief Execution order of traits in processing.
     */
    enum class ExecutionOrder : unsigned char {
        Base,                       /**< Base trait */
        EscapeProbabilityWeight,    /**< Trait affecting escape probability */
        PredationProbabilityWeight, /**< Trait affecting predation probability */
        EdibilityValueWeight,       /**< Trait affecting edibility value */
        ProbabilityDensityFunction, /**< Trait representing PDF */
        CellValue,                  /**< Trait representing cell value */
        Preferences                 /**< Trait representing preferences */
    };

    /**
     * @brief Function type to generate traits.
     */
    using GenerateTraitsFunc = std::function<
        std::vector<Trait*>(
        std::vector<IndividualLevelTrait*>&,
        const nlohmann::json&,
        const std::vector<nlohmann::json>&)>;

    /**
     * @brief Default function to generate traits.
     */
    static std::vector<Trait*> defaultGenerateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& definitionConfig, const std::vector<nlohmann::json>& individualLevelTraitsOrder);

    /**
     * @brief Default function to return trait string vector.
     */
    static std::vector<std::string> defaultGetTraitStrVector();

    /**
     * @brief Registry for trait generation functions per execution order.
     */
    inline static CustomIndexedVector<ExecutionOrder, GenerateTraitsFunc> registryGenerateTraits{
        EnumClass<Trait::ExecutionOrder>::size(), &Trait::defaultGenerateTraits
    };

    /**
     * @brief Registry for trait string vector functions per execution order.
     */
    inline static CustomIndexedVector<ExecutionOrder, std::function<std::vector<std::string>()>> registryGetTraitStrVector{
        EnumClass<Trait::ExecutionOrder>::size(), &Trait::defaultGetTraitStrVector
    };

    /**
     * @brief Generate traits based on individual-level traits, config, and order.
     */
    static CustomIndexedVector<ExecutionOrder, std::vector<Trait*>> generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& definitionConfig, const std::vector<nlohmann::json>& individualLevelTraitsOrder);

    /**
     * @brief Get the header string for traits.
     * @return Header string.
     */
    static std::string getHeader();

    /** @brief Default constructor. */
    Trait();

    /**
     * @brief Constructor for a trait.
     * @param individualLevelTraits Reference to individual-level traits.
     * @param config JSON configuration for the trait.
     * @param individualLevelTraitsOrder Order of individual-level traits in config.
     * @param traitStr String identifier for the trait.
     * @param fileName File associated with this trait.
     * @param temperatureSection Temperature section for thermal dependence.
     */
    Trait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& config, const std::vector<nlohmann::json>& individualLevelTraitsOrder, const std::string& traitStr, const std::string& fileName, TraitTemperatureSection* temperatureSection);
    
    /** @brief Destructor. */
    virtual ~Trait();

    /** @brief Get the value section of the trait. */
    TraitDefinitionSection* getValue();

    /** @brief Get the temperature section of the trait. */
    TraitTemperatureSection* getTemperatureSection();

    /** @brief Check if the trait is null (uninitialized). */
    bool isNull() const;

    /** @brief Check if the trait is thermally dependent. */
    bool isThermallyDependent() const;

    /** @brief Deserialize individual-level traits from JSON or saved state. */
    void deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits);

    /**
     * @brief Determine if the trait can have negative values.
     * @return True if negative values are allowed.
     */
    virtual bool canBeNegative() const=0;

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
    TraitDefinitionSection* value; /**< Definition values of the trait */
    bool thermallyDependent; /**< Flag indicating if trait is temperature-dependent */
    TraitTemperatureSection* temperatureSection; /**< Temperature section of the trait, if any */

    /** @brief Get the file name associated with the trait. */
    virtual std::string getFileName() const=0;

    /** @brief Get the parent trait string identifier. */
    virtual std::string getParentTraitStr() const=0;
};

#endif // TRAIT_H_
