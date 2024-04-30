#ifndef TRAIT_TEMPERATURE_SECTION_H_
#define TRAIT_TEMPERATURE_SECTION_H_

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

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"
#include "IBM/Physics/Temperature.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"
#include "Misc/EnumClass.h"
#include "Misc/CustomIndexedVector.h"


class AnimalSpecies;


class TraitTemperatureSection {
public:
    TraitTemperatureSection();
    
    virtual ~TraitTemperatureSection();

    virtual std::unique_ptr<std::tuple<std::map<Temperature, std::pair<double, double>>, double, std::map<Temperature, std::pair<double, double>>, double>> calculateTempSizeRuleConstantAccumulationVector(const double& lengthAtTempFromLab, const AnimalSpecies* const animalSpecies) const=0;

    virtual double applyTemperatureDependency(const Temperature& temperature, const CustomIndexedVector<TraitDefinitionSection::Elements, double>& traitElements,
        const std::map<Temperature, std::pair<double, double>>& lowerTempSizeRuleConstantAccumulationVector, const double& lastLowerTempSizeRuleConstant, 
        const std::map<Temperature, std::pair<double, double>>& upperTempSizeRuleConstantAccumulationVector, const double& lastUpperTempSizeRuleConstant,
        const AnimalSpecies* const animalSpecies
    ) const=0;    
};


#endif // TRAIT_TEMPERATURE_SECTION_H_
