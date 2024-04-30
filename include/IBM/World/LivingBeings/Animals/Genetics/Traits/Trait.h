#ifndef TRAIT_H_
#define TRAIT_H_

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/TemperatureSection/TempSizeRuleTraitTemperatureSection.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/TemperatureSection/DellsTraitTemperatureSection.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/EnumClass.h"
#include "Misc/CustomIndexedVector.h"
#include "IBM/Physics/Temperature.h"


class AnimalSpecies;


/**
 * @class Trait
 * @brief Represents a trait, which can be species level or individual level.
 */
class Trait {
public:
    /**
     * @enum Type
     * @brief Enumerates different trait types.
     */
	enum class Type : unsigned int {
		energy_tank,
		growth,
		eggDevTime, 
		factorEggMass,
		assim,
		voracity,
		speed,
		search_area,
		met_rate,
		shock_resistance,
        actE_met,
		memoryDepth,
        perception_area,
        interaction_area,
        devTime,
        lengthAtMaturation,
        pupaPeriodLength,
        timeAddedToMeetLastRepro
	};


    Trait(
        const std::string& traitType, const nlohmann::json& info, const std::vector<std::string>& individualLevelTraitsOrder, 
        const Temperature& tempFromLab, const std::vector<Locus*> &loci, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const std::vector<double>& rhoPerModule, const std::vector<unsigned int>& rhoRangePerModule
    );
    
    virtual ~Trait();

    const CustomIndexedVector<TraitDefinitionSection::Elements, TraitDefinitionSection*>& getElements() const;
    CustomIndexedVector<TraitDefinitionSection::Elements, TraitDefinitionSection*>& getMutableElements();

    const Type getType() const;

    const bool isInverse() const;

    const bool isThermallyDependent() const;

    const TraitTemperatureSection& getTemperatureSection() const;
    TraitTemperatureSection& getMutableTemperatureSection();

    double applyTemperatureDependency(const Temperature& temperature, const CustomIndexedVector<TraitDefinitionSection::Elements, double>& traitElements,
        const std::map<Temperature, std::pair<double, double>>& lowerTempSizeRuleConstantAccumulationVector, const double& lastLowerTempSizeRuleConstant, 
        const std::map<Temperature, std::pair<double, double>>& upperTempSizeRuleConstantAccumulationVector, const double& lastUpperTempSizeRuleConstant,
        const AnimalSpecies* const animalSpecies
    ) const;


protected:
    static CustomIndexedVector<Type, bool> inverseTraitVector;


    Type type;

    CustomIndexedVector<TraitDefinitionSection::Elements, TraitDefinitionSection*> traitElements;

    bool thermallyDependent;
    TraitTemperatureSection* temperatureSection;
};


#endif // TRAIT_H_
