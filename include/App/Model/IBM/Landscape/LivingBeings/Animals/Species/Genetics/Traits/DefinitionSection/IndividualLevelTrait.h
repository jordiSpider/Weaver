/**
 * @file IndividualLevelTrait.h
 * @brief Defines the IndividualLevelTrait class, representing traits that vary between individuals.
 */

#ifndef INDIVIDUAL_LEVEL_TRAIT_H_
#define INDIVIDUAL_LEVEL_TRAIT_H_


#include <string>


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"
#include "App/Model/IBM/Maths/MathFunctions.h"
#include "Misc/Utilities.h"


/**
 * @class IndividualLevelTrait
 * @brief Represents a trait defined at the individual level within a species.
 *
 * This class extends TraitDefinitionSection and handles traits that vary
 * among individuals, including genetic initialization, pseudo-value calculation,
 * and range checks.
 */
class IndividualLevelTrait: public TraitDefinitionSection {
public:
    /**
     * @brief Default constructor.
     */
    IndividualLevelTrait();

    /**
     * @brief Constructor from JSON configuration.
     *
     * @param config JSON object containing trait configuration.
     * @param newOrder Order of the trait in the module.
     * @param fileName File name associated with the trait.
     * @param traitStr Trait identifier string.
     * @param elementStr Element identifier string.
     */
    IndividualLevelTrait(const nlohmann::json& config, const size_t newOrder, const std::string& fileName, const std::string& traitStr, const std::string& elementStr);
    
    // Delete copy constructor
    IndividualLevelTrait(const IndividualLevelTrait&) = delete;

    // Delete assignment operator
    IndividualLevelTrait& operator=(const IndividualLevelTrait&) = delete;

    /**
     * @brief Virtual destructor.
     */
    virtual ~IndividualLevelTrait();

    /**
     * @brief Returns the order of this individual-level trait.
     * @return Order index of the trait.
     */
    const size_t& getOrder() const;

    /**
     * @brief Returns the minimum trait range.
     * @return Minimum trait range value.
     */
    const PreciseDouble& getMinTraitRange() const;

    /**
     * @brief Returns the maximum trait range.
     * @return Maximum trait range value.
     */
    const PreciseDouble& getMaxTraitRange() const;

    /**
     * @brief Returns the minimum restricted trait range.
     * @return Minimum restricted range value.
     */
    const PreciseDouble& getMinTraitRestrictedRange() const;

    /**
     * @brief Returns the maximum restricted trait range.
     * @return Maximum restricted range value.
     */
    const PreciseDouble& getMaxTraitRestrictedRange() const;

    /**
     * @brief Initializes a genetics file for storing individual trait values.
     *
     * @param geneticsFolder Path to the folder where genetics files are stored.
     * @param geneticHeader Header string for the genetics file.
     */
    void initializeGeneticFile(const std::filesystem::path& geneticsFolder, const std::string& geneticHeader);
    
    /**
     * @brief Calculates a pseudo-value for the trait based on genome information.
     *
     * @param genome Genome of the individual.
     * @param traitsPerModule Number of traits per module.
     * @param numberOfLociPerTrait Number of loci per trait.
     * @param rhoPerModule Vector of rho values per module.
     * @param rhoRangePerModule Vector of rho ranges per module.
     * @return Calculated pseudo-value as PreciseDouble.
     */
    PreciseDouble calculatePseudoValue(const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule) const;

    /**
     * @brief Prints genetics information for the individual.
     *
     * @param animalInfo String stream containing animal information.
     * @param genome Genome of the individual.
     * @param traitsPerModule Number of traits per module.
     * @param numberOfLociPerTrait Number of loci per trait.
     * @param rhoPerModule Vector of rho values per module.
     * @param rhoRangePerModule Vector of rho ranges per module.
     */
    void printGenetics(const std::ostringstream& animalInfo, const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule);

    /**
     * @brief Gets the trait value for a given genome.
     *
     * Overrides TraitDefinitionSection::getValue.
     *
     * @param genome Genome of the individual.
     * @param traitsPerModule Number of traits per module.
     * @param numberOfLociPerTrait Number of loci per trait.
     * @param rhoPerModule Vector of rho values per module.
     * @param rhoRangePerModule Vector of rho ranges per module.
     * @return Trait value as PreciseDouble.
     */
    PreciseDouble getValue(const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule) const;

    /**
     * @brief Checks if a trait value is within restricted ranges.
     *
     * @param traitValue Value to check.
     * @return True if within allowed range, false otherwise.
     */
    bool isInsideRestrictedRanges(const PreciseDouble& traitValue) const;

    /**
     * @brief Sets the pseudo-value limits for the trait.
     *
     * @param loci Vector of loci.
     * @param traitsPerModule Number of traits per module.
     * @param numberOfLociPerTrait Number of loci per trait.
     * @param rhoPerModule Vector of rho values per module.
     * @param rhoRangePerModule Vector of rho ranges per module.
     */
    void setPseudoValueLimits(const std::vector<Locus> &loci, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule);

    /**
     * @brief Returns the trait identifier string.
     * @return Trait string.
     */
    const std::string& getTraitStr() const;

    /**
     * @brief Returns the element identifier string.
     * @return Element string.
     */
    const std::string& getElementStr() const;

    /**
     * @brief Deserializes individual-level traits from serialized data.
     *
     * Overrides TraitDefinitionSection::deserializeIndividualLevelTraits.
     *
     * @param individualLevelTraits Vector of pointers to IndividualLevelTrait objects.
     */
    void deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits);

    /**
     * @brief Checks if the trait is null/uninitialized.
     *
     * @return True if uninitialized, false otherwise.
     */
    bool isNull() const;

    /**
     * @brief Returns the type of the trait (IndividualLevel).
     *
     * @return TraitDefinitionSection::Type::IndividualLevel
     */
    Type getType() const;

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
    /// Order of this trait in the module.
    size_t order;

    /// Minimum trait range.
    PreciseDouble minTraitRange;

    /// Maximum trait range.
    PreciseDouble maxTraitRange;

    /// Trait restriction value.
    PreciseDouble restrictValue;

    /// Minimum restricted trait range.
    PreciseDouble minTraitRestrictedRange;

    /// Maximum restricted trait range.
    PreciseDouble maxTraitRestrictedRange;

    /// Minimum trait limit.
    PreciseDouble minTraitLimit;

    /// Maximum trait limit.
    PreciseDouble maxTraitLimit;

    /// Minimum pseudo-value limit.
    PreciseDouble minPseudoValueLimit;

    /// Maximum pseudo-value limit.
    PreciseDouble maxPseudoValueLimit;

    /// Associated file name.
    std::string fileName;

    /// Output file for genetics data.
    std::ofstream geneticFile;

    /// Trait identifier string.
    std::string traitStr;

    /// Element identifier string.
    std::string elementStr;


    /// Returns minimum pseudo-value limit.
    const PreciseDouble& getMinPseudoValueLimit() const;

    /// Returns maximum pseudo-value limit.
    const PreciseDouble& getMaxPseudoValueLimit() const;

    /// Returns minimum trait limit.
    const PreciseDouble& getMinTraitLimit() const;

    /// Returns maximum trait limit.
    const PreciseDouble& getMaxTraitLimit() const;
};

#endif // INDIVIDUAL_LEVEL_TRAIT_H_
