/**
 * @file TraitDefinitionSection.h
 * @brief Defines the TraitDefinitionSection class, representing a section of a trait definition.
 */

#ifndef TRAIT_DEFINITION_SECTION_H_
#define TRAIT_DEFINITION_SECTION_H_


#include <nlohmann/json.hpp>
#include <vector>
#include <string>


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Genome.h"
#include "Misc/EnumClass.h"
#include "Exceptions/LineInfoException.h"



class IndividualLevelTrait;


/**
 * @class TraitDefinitionSection
 * @brief Represents the definition of a trait, either at the species or individual level.
 *
 * This class provides an interface for trait definitions, including:
 * - Retrieving trait values based on genome and module information.
 * - Checking trait ranges.
 * - Deserializing individual-level traits.
 * - Serializing the trait definition for persistence.
 */
class TraitDefinitionSection {
public:
    /**
     * @enum Type
     * @brief Represents whether the trait is defined at the species or individual level.
     */
    enum class Type : unsigned char
    {
        /// Trait defined at the species level.
        SpeciesLevel,

        /// Trait defined at the individual level.
        IndividualLevel
    };

    /**
     * @brief Finds the order of a trait element in a configuration.
     *
     * @param type Type of the trait (SpeciesLevel or IndividualLevel).
     * @param individualLevelTraitsOrder Vector of JSON objects defining trait order.
     * @param traitStr String identifier of the trait.
     * @param elementStr String identifier of the element.
     * @return Position index of the element in the order vector.
     */
    static size_t findOrder(Type type, const std::vector<nlohmann::json>& individualLevelTraitsOrder, const std::string& traitStr, const std::string& elementStr);

    /**
     * @brief Creates a new instance of a TraitDefinitionSection from configuration.
     *
     * @param individualLevelTraits Vector of pointers to individual-level traits.
     * @param config JSON object defining the trait.
     * @param individualLevelTraitsOrder Vector defining the order of traits.
     * @param traitStr Trait identifier string.
     * @param elementStr Element identifier string.
     * @param fileName File name associated with the trait.
     * @return Pointer to a new TraitDefinitionSection instance.
     */
    static TraitDefinitionSection* createInstance(std::vector<IndividualLevelTrait*>& individualLevelTraits, const nlohmann::json& config, const std::vector<nlohmann::json>& individualLevelTraitsOrder, const std::string& traitStr, const std::string& elementStr, const std::string& fileName);


    /**
     * @brief Default constructor.
     */
    TraitDefinitionSection();
    
    /**
     * @brief Destructor.
     */
    virtual ~TraitDefinitionSection();

    /**
     * @brief Retrieves the value of the trait for a given genome.
     *
     * Must be implemented by derived classes.
     *
     * @param genome Genome containing trait information.
     * @param traitsPerModule Number of traits per module.
     * @param numberOfLociPerTrait Number of loci per trait.
     * @param rhoPerModule Vector of rho values per module.
     * @param rhoRangePerModule Vector of rho ranges per module.
     * @return Trait value as a PreciseDouble.
     */
    virtual PreciseDouble getValue(const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule) const=0;

    /**
     * @brief Deserializes individual-level traits from a serialized format.
     *
     * Must be implemented by derived classes.
     *
     * @param individualLevelTraits Vector of pointers to IndividualLevelTrait objects to populate.
     */
    virtual void deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits)=0;

    /**
     * @brief Returns the type of the trait (SpeciesLevel or IndividualLevel).
     *
     * Must be implemented by derived classes.
     *
     * @return Trait type.
     */
    virtual Type getType() const=0;

    /**
     * @brief Returns the minimum allowed value for the trait.
     *
     * Must be implemented by derived classes.
     *
     * @return Minimum restricted range as PreciseDouble.
     */
    virtual const PreciseDouble& getMinTraitRestrictedRange() const=0;

    /**
     * @brief Returns the maximum allowed value for the trait.
     *
     * Must be implemented by derived classes.
     *
     * @return Maximum restricted range as PreciseDouble.
     */
    virtual const PreciseDouble& getMaxTraitRestrictedRange() const=0;

    /**
     * @brief Checks if a trait value is within restricted ranges.
     *
     * Must be implemented by derived classes.
     *
     * @param traitValue Trait value to check.
     * @return True if the value is inside restricted ranges, false otherwise.
     */
    virtual bool isInsideRestrictedRanges(const PreciseDouble& traitValue) const=0;

    /**
     * @brief Checks if the trait definition is null or uninitialized.
     *
     * Must be implemented by derived classes.
     *
     * @return True if the trait definition is null, false otherwise.
     */
    virtual bool isNull() const=0;

    /**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // TRAIT_DEFINITION_SECTION_H_
