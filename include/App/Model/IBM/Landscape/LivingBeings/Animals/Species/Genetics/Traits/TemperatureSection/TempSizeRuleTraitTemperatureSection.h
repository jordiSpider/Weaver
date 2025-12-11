/**
 * @file TempSizeRuleTraitTemperatureSection.h
 * @brief Defines the TempSizeRuleTraitTemperatureSection class, handling temperature-dependent size rules.
 */

#ifndef TEMP_SIZE_RULE_TRAIT_TEMPERATURE_SECTION_H_
#define TEMP_SIZE_RULE_TRAIT_TEMPERATURE_SECTION_H_


#include <nlohmann/json.hpp>
#include <map>


#include <boost/serialization/export.hpp>
#include <boost/serialization/map.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TraitTemperatureSection.h"
#include "App/Model/IBM/Physics/Temperature.h"


/**
 * @class TempSizeRuleTraitTemperatureSection
 * @brief Represents a temperature-dependent trait section for size rules.
 *
 * This class extends TraitTemperatureSection to provide functionality for
 * traits where the size of an individual depends on temperature.
 * It stores a mapping from temperature to expected dry mass and
 * provides methods to generate individual trait sections and deserialize traits.
 */
class TempSizeRuleTraitTemperatureSection : public TraitTemperatureSection {
public:
    /**
     * @brief Default constructor.
     */
    TempSizeRuleTraitTemperatureSection();

    /**
     * @brief Constructor from JSON configuration.
     *
     * @param config JSON object containing temperature-size rule configuration.
     */
    TempSizeRuleTraitTemperatureSection(const nlohmann::json& config);
    
    /**
     * @brief Destructor.
     */
    virtual ~TempSizeRuleTraitTemperatureSection();

    /**
     * @brief Returns the temperature-to-size mapping.
     *
     * @return Reference to a map from Temperature to DryMass.
     */
    const std::map<Temperature, DryMass>& getTempSizeRuleVector() const;

    /**
     * @brief Generates an individual-level temperature section for this trait.
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
    /// Map from Temperature to DryMass defining the size rule at each temperature.
    std::map<Temperature, DryMass> tempSizeRuleVector;
};


#endif // TEMP_SIZE_RULE_TRAIT_TEMPERATURE_SECTION_H_
