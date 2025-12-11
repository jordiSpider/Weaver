/**
 * @file SpeciesLevelTrait.h
 * @brief Defines the SpeciesLevelTrait class, representing traits defined at the species level.
 */

#ifndef SPECIES_LEVEL_TRAIT_H_
#define SPECIES_LEVEL_TRAIT_H_


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"


/**
 * @class SpeciesLevelTrait
 * @brief Represents a trait that is constant for all individuals of a species.
 *
 * This class extends TraitDefinitionSection and provides functionality
 * for traits that have the same value across all individuals in the species.
 */
class SpeciesLevelTrait: public TraitDefinitionSection {
public:
    /**
     * @brief Default constructor.
     */
    SpeciesLevelTrait();

    /**
     * @brief Constructor from JSON configuration.
     * 
     * @param info JSON object containing the trait information.
     */
    SpeciesLevelTrait(const nlohmann::json& info);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~SpeciesLevelTrait();

    /**
     * @brief Gets the value of the species-level trait.
     *
     * Overrides TraitDefinitionSection::getValue.
     *
     * @param genome Genome of the individual (ignored for species-level traits).
     * @param traitsPerModule Number of traits per module (ignored).
     * @param numberOfLociPerTrait Number of loci per trait (ignored).
     * @param rhoPerModule Vector of rho values per module (ignored).
     * @param rhoRangePerModule Vector of rho ranges per module (ignored).
     * @return The value of the species-level trait.
     */
    PreciseDouble getValue(const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule) const;

    /**
     * @brief Sets the value of the species-level trait.
     *
     * @param newValue The new value to assign to the trait.
     */
    void setValue(const PreciseDouble& newValue);

    /**
     * @brief Returns the type of the trait (SpeciesLevel).
     *
     * @return TraitDefinitionSection::Type::SpeciesLevel
     */
    Type getType() const;

    /**
     * @brief Returns the minimum allowed value for the trait.
     *
     * @return Minimum restricted range as PreciseDouble.
     */
    const PreciseDouble& getMinTraitRestrictedRange() const;

    /**
     * @brief Returns the maximum allowed value for the trait.
     *
     * @return Maximum restricted range as PreciseDouble.
     */
    const PreciseDouble& getMaxTraitRestrictedRange() const;

    /**
     * @brief Checks if a given value is within restricted ranges.
     *
     * @param traitValue Value to check.
     * @return True if within allowed range, false otherwise.
     */
    bool isInsideRestrictedRanges(const PreciseDouble& traitValue) const;

    /**
     * @brief Deserializes individual-level traits (no-op for species-level traits).
     *
     * @param individualLevelTraits Vector of pointers to IndividualLevelTrait objects.
     */
    void deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits);

    /**
     * @brief Checks if the trait is null/uninitialized.
     *
     * @return True if the trait has not been initialized, false otherwise.
     */
    bool isNull() const;

    /**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    /// Value of the species-level trait.
    PreciseDouble value;

    /// Indicates whether the trait is uninitialized or null.
    bool null;
};

#endif // SPECIES_LEVEL_TRAIT_H_
