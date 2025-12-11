/**
 * @file CellValueTrait.h
 * @brief Defines the CellValueTrait class, representing traits related to cell values in the simulation.
 *
 * CellValueTrait inherits from Trait and provides functionality to represent
 * traits that store values associated with cells. It supports serialization,
 * negative value checking, and configuration via JSON.
 */

#ifndef CELL_VALUE_TRAIT_H_
#define CELL_VALUE_TRAIT_H_


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Trait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/CellValueTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarTraitTemperatureSection.h"


/**
 * @class CellValueTrait
 * @brief Represents a trait storing values in a cell for individual-based simulations.
 *
 * This class extends the Trait base class and includes functionality to:
 * - Generate multiple traits from configuration.
 * - Retrieve trait identifiers.
 * - Handle JSON configuration and serialization.
 */
class CellValueTrait : public Trait {
public:
    /**
     * @brief Generates a list of CellValueTrait objects from JSON configuration.
     *
     * @param individualLevelTraits Vector of pointers to individual-level traits.
     * @param definitionConfig JSON object defining the traits.
     * @param individualLevelTraitsOrder Vector of JSON objects specifying the order of individual-level traits.
     * @return Vector of pointers to Trait objects created.
     */
    static std::vector<Trait*> generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& definitionConfig, const std::vector<nlohmann::json>& individualLevelTraitsOrder);

    /**
     * @brief Returns the string identifiers for all CellValueTrait types.
     *
     * @return Vector of strings representing the trait identifiers.
     */
    static std::vector<std::string> getTraitStrVector();

    /**
     * @brief Default constructor.
     *
     * Initializes an empty CellValueTrait object.
     */
    CellValueTrait();

    /**
     * @brief Constructor with initialization parameters.
     *
     * @param individualLevelTraits Vector of pointers to individual-level traits.
     * @param type Type of CellValueTrait.
     * @param config JSON object containing trait configuration.
     * @param individualLevelTraitsOrder Vector of JSON objects defining the order of traits.
     */
    CellValueTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const CellValueTraitType type, const nlohmann::json& config, const std::vector<nlohmann::json>& individualLevelTraitsOrder);

    /**
     * @brief Destructor.
     *
     * Cleans up resources used by the trait.
     */
    virtual ~CellValueTrait();

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
    /**
     * @brief Creates a temperature section for the trait from JSON configuration.
     *
     * This is used internally to initialize temperature-dependent aspects of the trait.
     *
     * @param individualLevelTraits Vector of pointers to individual-level traits.
     * @param temperatureConfig JSON object containing temperature configuration.
     * @param individualLevelTraitsOrder Vector defining order of traits.
     * @param traitStr Trait identifier string.
     * @param fileName File name associated with the trait.
     * @return Pointer to the created TraitTemperatureSection.
     */
    static TraitTemperatureSection* createTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& temperatureConfig, const std::vector<nlohmann::json>& individualLevelTraitsOrder, const std::string& traitStr, const std::string& fileName);
};

#endif // CELL_VALUE_TRAIT_H_
