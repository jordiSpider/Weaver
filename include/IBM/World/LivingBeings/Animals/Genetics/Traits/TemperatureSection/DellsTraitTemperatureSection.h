#ifndef DELLS_TRAIT_TEMPERATURE_SECTION_H_
#define DELLS_TRAIT_TEMPERATURE_SECTION_H_

#include <string>
#include <unordered_map>
#include <cfloat>
#include <magic_enum.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/TemperatureSection/TraitTemperatureSection.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"
#include "IBM/Physics/Temperature.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"
#include "Misc/EnumClass.h"
#include "IBM/Maths/MathFunctions.h"


class AnimalSpecies;


class DellsTraitTemperatureSection : public TraitTemperatureSection {
public:
    DellsTraitTemperatureSection(const std::string& traitType, const nlohmann::json& info, const std::vector<std::string>& individualLevelTraitsOrder, const bool traitInverse, const std::vector<Locus*> &loci, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const std::vector<double>& rhoPerModule, const std::vector<unsigned int>& rhoRangePerModule);
    
    virtual ~DellsTraitTemperatureSection();

    TraitDefinitionSection* getActivationEnergy();
    TraitDefinitionSection* getEnergyDecay();

    TraitDefinitionSection* getLowerThreshold();
    TraitDefinitionSection* getUpperThreshold();

    TraitDefinitionSection* getTemperatureOptimal();

    const bool isInverse() const;

    std::unique_ptr<std::tuple<std::map<Temperature, std::pair<double, double>>, double, std::map<Temperature, std::pair<double, double>>, double>> calculateTempSizeRuleConstantAccumulationVector(const double& lengthAtTempFromLab, const AnimalSpecies* const animalSpecies) const;

    double applyTemperatureDependency(const Temperature& temperature, const CustomIndexedVector<TraitDefinitionSection::Elements, double>& traitElements,
        const std::map<Temperature, std::pair<double, double>>& lowerTempSizeRuleConstantAccumulationVector, const double& lastLowerTempSizeRuleConstant, 
        const std::map<Temperature, std::pair<double, double>>& upperTempSizeRuleConstantAccumulationVector, const double& lastUpperTempSizeRuleConstant,
        const AnimalSpecies* const animalSpecies
    ) const;

protected:
    TraitDefinitionSection* activationEnergy;
    TraitDefinitionSection* energyDecay;

    TraitDefinitionSection* lowerThreshold;
    TraitDefinitionSection* upperThreshold;

    TraitDefinitionSection* temperatureOptimal;

    bool traitInverse;
};


#endif // DELLS_TRAIT_TEMPERATURE_SECTION_H_
