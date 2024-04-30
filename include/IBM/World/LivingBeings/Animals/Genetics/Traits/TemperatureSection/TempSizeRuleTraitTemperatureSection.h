#ifndef TEMP_SIZE_RULE_TRAIT_TEMPERATURE_SECTION_H_
#define TEMP_SIZE_RULE_TRAIT_TEMPERATURE_SECTION_H_

#include <string>
#include <unordered_map>
#include <cfloat>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>
#include <memory>

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/TemperatureSection/TraitTemperatureSection.h"
#include "IBM/Physics/Temperature.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"
#include "Misc/EnumClass.h"
#include "IBM/Maths/MathFunctions.h"


class AnimalSpecies;


class TempSizeRuleTraitTemperatureSection : public TraitTemperatureSection {
public:
    TempSizeRuleTraitTemperatureSection(const nlohmann::json& info, const Temperature& tempFromLab);
    
    virtual ~TempSizeRuleTraitTemperatureSection();

    const Temperature& getTempFromLab() const;

    std::unique_ptr<std::tuple<std::map<Temperature, std::pair<double, double>>, double, std::map<Temperature, std::pair<double, double>>, double>> calculateTempSizeRuleConstantAccumulationVector(const double& lengthAtTempFromLab, const AnimalSpecies* const animalSpecies) const;

    double applyTemperatureDependency(const Temperature& temperature, const CustomIndexedVector<TraitDefinitionSection::Elements, double>& traitElements,
        const std::map<Temperature, std::pair<double, double>>& lowerTempSizeRuleConstantAccumulationVector, const double& lastLowerTempSizeRuleConstant, 
        const std::map<Temperature, std::pair<double, double>>& upperTempSizeRuleConstantAccumulationVector, const double& lastUpperTempSizeRuleConstant,
        const AnimalSpecies* const animalSpecies
    ) const;

protected:
    std::map<Temperature, double> lowerTempSizeRuleVector;
    std::map<Temperature, double> upperTempSizeRuleVector;

    const Temperature* const tempFromLab;
};


#endif // TEMP_SIZE_RULE_TRAIT_TEMPERATURE_SECTION_H_
