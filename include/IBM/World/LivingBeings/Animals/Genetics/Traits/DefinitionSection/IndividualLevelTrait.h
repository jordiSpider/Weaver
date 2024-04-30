#ifndef INDIVIDUAL_LEVEL_TRAIT_H_
#define INDIVIDUAL_LEVEL_TRAIT_H_


#include <nlohmann/json.hpp>
#include <cfloat>

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"
#include "IBM/Maths/MathFunctions.h"
#include "Misc/Types.h"


/**
 * @class IndividualLevelTrait
 * @brief Represents a individual level trait with specified ranges and limits.
 */
class IndividualLevelTrait: public TraitDefinitionSection {
protected:
    unsigned int order;
    double minTraitRange;
    double maxTraitRange;
    double restrictValue;
    double minTraitRestrictedRange;
    double maxTraitRestrictedRange;
    double minTraitLimit;
    double maxTraitLimit;

    double minPseudoValueLimit;
    double maxPseudoValueLimit;

    const double& getMinPseudoValueLimit() const;
    const double& getMaxPseudoValueLimit() const;

    /**
     * @brief Gets the minimum trait limit.
     * @return The minimum trait limit.
     */
    const double& getMinTraitLimit() const;

    /**
     * @brief Gets the maximum trait limit.
     * @return The maximum trait limit.
     */
    const double& getMaxTraitLimit() const;

public:
    /**
     * @brief Constructor for IndividualLevelTrait.
     * @param typeStr The string representation of the trait type.
     * @param traitInfo Trait info.
     * @param order The order of the trait.
     */
    IndividualLevelTrait(const nlohmann::json& info, const unsigned int order, const std::vector<Locus*> &loci, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const std::vector<double>& rhoPerModule, const std::vector<unsigned int>& rhoRangePerModule);
    
    /**
     * @brief Virtual destructor for IndividualLevelTrait.
     */
    virtual ~IndividualLevelTrait();

    /**
     * @brief Gets the order of the individual level trait.
     * @return The order of the individual level trait.
     */
    const unsigned int& getOrder() const;

    /**
     * @brief Gets the minimum trait range.
     * @return The minimum trait range.
     */
    const double& getMinTraitRange() const;

    /**
     * @brief Gets the maximum trait range.
     * @return The maximum trait range.
     */
    const double& getMaxTraitRange() const;

    /**
     * @brief Gets the minimum restricted range.
     * @return The minimum restricted range.
     */
    const double& getMinTraitRestrictedRange() const;

    /**
     * @brief Gets the maximum restricted range.
     * @return The maximum restricted range.
     */
    const double& getMaxTraitRestrictedRange() const;
    
    const double calculatePseudoValue(const Genome& genome, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const std::vector<double>& rhoPerModule, const std::vector<unsigned int>& rhoRangePerModule) const;

    const double getValue(const Genome& genome, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const std::vector<double>& rhoPerModule, const std::vector<unsigned int>& rhoRangePerModule) const;

    bool isInsideRestrictedRanges(const double& traitValue) const;

    void setPseudoValueLimits(const std::vector<Locus*> &loci, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const std::vector<double>& rhoPerModule, const std::vector<unsigned int>& rhoRangePerModule);

    const Type getType() const;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // INDIVIDUAL_LEVEL_TRAIT_H_
