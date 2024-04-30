#ifndef VARIABLE_TRAIT_H_
#define VARIABLE_TRAIT_H_


#include <string>
#include <nlohmann/json.hpp>

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/Trait.h"


/**
 * @class VariableTrait
 * @brief Represents a variable trait with specified ranges and limits.
 */
class VariableTrait: public Trait {
private:
    const unsigned int order;
    const double minTraitRange;
    const double maxTraitRange;
    const double restrictValue;
    const double minRestrictedRange;
    const double maxRestrictedRange;
    const double minTraitLimit;
    const double maxTraitLimit;

    double minObservedPseudoValue;
    double maxObservedPseudoValue;

    friend class boost::serialization::access;

public:
    /**
     * @brief Constructor for VariableTrait.
     * @param typeStr The string representation of the trait type.
     * @param traitInfo Trait info.
     * @param order The order of the trait.
     */
    VariableTrait(const std::string& typeStr, const nlohmann::json& traitInfo, const unsigned int& order);
    
    /**
     * @brief Virtual destructor for VariableTrait.
     */
    virtual ~VariableTrait();

    /**
     * @brief Gets the order of the variable trait.
     * @return The order of the variable trait.
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
    const double& getMinRestrictedRange() const;

    /**
     * @brief Gets the maximum restricted range.
     * @return The maximum restricted range.
     */
    const double& getMaxRestrictedRange() const;

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

    /**
     * @brief Gets the minimum observed pseudo value.
     * @return The minimum observed pseudo value.
     */
    const double& getMinObservedPseudoValue() const;

    /**
     * @brief Gets the maximum observed pseudo value.
     * @return The maximum observed pseudo value.
     */
    const double& getMaxObservedPseudoValue() const;

    ValueUpdateMethod::MethodType getValueUpdateMethodType() const;

    /**
     * @brief Sets the minimum observed pseudo value.
     * @param minObservedPseudoValue The new minimum observed pseudo value.
     */
    void setMinObservedPseudoValue(const double& newMinObservedPseudoValue);
    
    /**
     * @brief Sets the maximum observed pseudo value.
     * @param maxObservedPseudoValue The new maximum observed pseudo value.
     */
    void setMaxObservedPseudoValue(const double& newMaxObservedPseudoValue);

    /**
     * @brief Serialize the FixedTrait object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // VARIABLE_TRAIT_H_
