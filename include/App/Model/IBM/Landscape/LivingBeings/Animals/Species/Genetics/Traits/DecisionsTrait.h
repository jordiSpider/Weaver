/**
 * @file DecisionsTrait.h
 * @brief Defines the DecisionsTrait class, a type of Trait that represents decision-making characteristics.
 *
 * The DecisionsTrait class extends the base Trait class to provide functionality
 * for traits that can influence decisions of individuals in the simulation.
 * It supports serialization for persistence and JSON configuration.
 */

#ifndef DECISIONS_TRAIT_H_
#define DECISIONS_TRAIT_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Trait.h"


/**
 * @class DecisionsTrait
 * @brief Represents a trait that affects decision-making of individuals.
 *
 * DecisionsTrait inherits from Trait and may contain individual-level traits.
 * It provides methods to check trait constraints, handle JSON configuration,
 * and serialize the trait for persistence.
 */
class DecisionsTrait : public Trait {
public:
    /**
     * @brief Virtual destructor.
     *
     * Ensures proper cleanup when deleting derived classes through a pointer to DecisionsTrait.
     */
    virtual ~DecisionsTrait();

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
     * @brief Retrieves the JSON field name for this trait.
     *
     * This function returns the key used in JSON configuration files
     * to identify this trait.
     *
     * @return JSON field name as a std::string.
     */
    static std::string getJsonFieldName();

    /**
     * @brief Default constructor.
     *
     * Initializes an empty DecisionsTrait object.
     */
    DecisionsTrait();

    /**
     * @brief Constructor with configuration parameters.
     *
     * Initializes the trait based on JSON configuration and individual-level traits.
     *
     * @param individualLevelTraits Vector of pointers to individual-level traits.
     * @param config JSON object containing configuration for this trait.
     * @param individualLevelTraitsOrder Vector of JSON objects defining the order of traits.
     * @param traitStr String identifier for this trait.
     * @param fileName File name associated with this trait.
     * @param temperatureSection Pointer to temperature-specific trait section (optional).
     */
    DecisionsTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& config, const std::vector<nlohmann::json>& individualLevelTraitsOrder, const std::string& traitStr, const std::string& fileName, TraitTemperatureSection* temperatureSection);

    /**
     * @brief Returns the file name associated with this trait.
     *
     * @return File name as a std::string.
     */
    virtual std::string getFileName() const;

    /**
     * @brief Returns the parent trait identifier as a string.
     *
     * @return Parent trait string.
     */
    virtual std::string getParentTraitStr() const;
};

#endif // DECISIONS_TRAIT_H_
