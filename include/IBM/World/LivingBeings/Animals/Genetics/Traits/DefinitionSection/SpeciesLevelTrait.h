#ifndef SPECIES_LEVEL_TRAIT_H_
#define SPECIES_LEVEL_TRAIT_H_


#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"


/**
 * @class SpeciesLevelTrait
 * @brief Represents a species level trait with a specific value.
 */
class SpeciesLevelTrait: public TraitDefinitionSection {
private:
    double value;

public:
    /**
     * @brief Constructor for SpeciesLevelTrait.
     * @param typeStr The string representation of the trait type.
     * @param traitInfo Trait info.
     */
    SpeciesLevelTrait(const nlohmann::json& info);
    
    /**
     * @brief Virtual destructor for SpeciesLevelTrait.
     */
    virtual ~SpeciesLevelTrait();

    /**
     * @brief Gets the value of the species level trait.
     * @return The value of the species level trait.
     */
    const double getValue(const Genome& genome, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const std::vector<double>& rhoPerModule, const std::vector<unsigned int>& rhoRangePerModule) const;

    /**
     * @brief Sets the value of the species level trait.
     * @param value The new value of the species level trait.
     */
    void setValue(const double& value);

    const Type getType() const;

    const double& getMinTraitRestrictedRange() const;
    const double& getMaxTraitRestrictedRange() const;

    /**
     * @brief Serialize the SpeciesLevelTrait object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // SPECIES_LEVEL_TRAIT_H_
