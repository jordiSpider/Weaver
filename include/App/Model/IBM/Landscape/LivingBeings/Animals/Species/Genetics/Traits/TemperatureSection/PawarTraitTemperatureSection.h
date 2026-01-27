/**
 * @file PawarTraitTemperatureSection.h
 * @brief Defines the PawarTraitTemperatureSection class, handling Pawar metabolic model traits with temperature dependence.
 */

#ifndef PAWAR_TRAIT_TEMPERATURE_SECTION_H_
#define PAWAR_TRAIT_TEMPERATURE_SECTION_H_


#include <string>
#include <nlohmann/json.hpp>


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarElement.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TraitTemperatureSection.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"
#include "Misc/EnumClass.h"
#include "Misc/CustomIndexedVector.h"


/**
 * @class PawarTraitTemperatureSection
 * @brief Represents a temperature-dependent section of a Pawar metabolic model trait.
 *
 * This class extends TraitTemperatureSection to manage traits based on the Pawar
 * model, which includes parameters such as activation energy, optimal temperature,
 * and energy decay. It supports individual trait generation, range checking,
 * and serialization.
 */
class PawarTraitTemperatureSection : public TraitTemperatureSection {
public:
    /**
     * @brief Returns the CSV header for a given trait.
     *
     * @param trait Name of the trait.
     * @return Header string for CSV or data output.
     */
    static std::string getHeader(const std::string& trait);


    /**
     * @brief Default constructor.
     */
    PawarTraitTemperatureSection();

    /**
     * @brief Constructor from configuration.
     *
     * @param individualLevelTraits Vector of pointers to individual-level traits.
     * @param config JSON object containing trait configuration.
     * @param individualLevelTraitsOrder Vector defining the order of traits.
     * @param inverse Whether the trait is inverse.
     * @param strictlyPositive Whether the trait must always be strictly positive.
     * @param trait Trait identifier string.
     * @param fileName File name associated with the trait.
     */
    PawarTraitTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& config, const std::vector<nlohmann::json>& individualLevelTraitsOrder, const bool inverse, const bool strictlyPositive, const std::string& trait, const std::string& fileName);
    
    /**
     * @brief Destructor.
     */
    virtual ~PawarTraitTemperatureSection();

    /**
     * @brief Checks if the trait is inverse.
     *
     * @return True if the trait is inverse, false otherwise.
     */
    bool isInverse() const;

    /**
     * @brief Checks if the trait must always be strictly positive.
     *
     * @return True if strictly positive, false otherwise.
     */
    bool isStrictlyPositive() const;

    /**
     * @brief Generates an individual-level temperature section for this Pawar trait.
     *
     * Overrides the pure virtual method from TraitTemperatureSection.
     *
     * @param geneticValue Genetic value of the trait.
     * @param genome Genome containing trait information.
     * @param traitsPerModule Number of traits per module.
     * @param numberOfLociPerTrait Number of loci per trait.
     * @param rhoPerModule Vector of rho values per module.
     * @param rhoRangePerModule Vector of rho ranges per module.
     * @return Pointer to an IndividualTraitTemperatureSection representing the individual's temperature-dependent trait.
     */
    IndividualTraitTemperatureSection* generateIndividualTraitTemperatureSection(const PreciseDouble& geneticValue, const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule) const;

    /**
     * @brief Deserializes individual-level traits from serialized data.
     *
     * Overrides the pure virtual method from TraitTemperatureSection.
     *
     * @param individualLevelTraits Vector of pointers to IndividualLevelTrait objects to populate.
     */
    void deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits);

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
    /// Pawar elements associated with this trait (e.g., activation energy, optimal temperature).
    CustomIndexedVector<PawarElement, TraitDefinitionSection*> elements;

    /// Indicates whether the trait is inverse.
    bool inverse;

    /// Indicates whether the trait must always be strictly positive.
    bool strictlyPositive;
};


#endif // PAWAR_TRAIT_TEMPERATURE_SECTION_H_
