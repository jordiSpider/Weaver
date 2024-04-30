#ifndef TRAIT_DEFINITION_SECTION_H_
#define TRAIT_DEFINITION_SECTION_H_


#include <nlohmann/json.hpp>
#include <vector>
#include <iterator>
#include <string>

#include "IBM/World/LivingBeings/Animals/Genetics/Genome.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/EnumClass.h"


class TraitDefinitionSection {
public:
    enum class Type : unsigned int
    {
        SpeciesLevel,
        IndividualLevel
    };

    enum class Elements : unsigned int
    {
        TraitValue,
        ActivationEnergy,
        EnergyDecay,
        LowerThreshold,
        UpperThreshold,
        TemperatureOptimal
    };


    static std::unique_ptr<TraitDefinitionSection> createInstance(const std::string& traitType, const nlohmann::json& info, const std::vector<std::string>& individualLevelTraitsOrder, const Elements& traitElement, const std::vector<Locus*> &loci, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const std::vector<double>& rhoPerModule, const std::vector<unsigned int>& rhoRangePerModule);


    TraitDefinitionSection();
    
    virtual ~TraitDefinitionSection();

    virtual const double getValue(const Genome& genome, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const std::vector<double>& rhoPerModule, const std::vector<unsigned int>& rhoRangePerModule) const=0;

    virtual const Type getType() const=0;

    virtual const double& getMinTraitRestrictedRange() const=0;
    virtual const double& getMaxTraitRestrictedRange() const=0;
};


#endif // TRAIT_DEFINITION_SECTION_H_
