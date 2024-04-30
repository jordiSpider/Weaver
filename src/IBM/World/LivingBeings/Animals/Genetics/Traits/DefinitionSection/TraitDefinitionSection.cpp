#include "IBM/World/LivingBeings/Animals/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/DefinitionSection/IndividualLevelTrait.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/DefinitionSection/SpeciesLevelTrait.h"


using namespace std;
using json = nlohmann::json;



unique_ptr<TraitDefinitionSection> TraitDefinitionSection::createInstance(const string& traitType, const json& info, const vector<string>& individualLevelTraitsOrder, const Elements& traitElement, const std::vector<Locus*> &loci, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const std::vector<double>& rhoPerModule, const std::vector<unsigned int>& rhoRangePerModule)
{
    string traitElementStr;

    switch(traitElement) {
        case Elements::TraitValue: {
            traitElementStr = string("value");
            break;
        }
        case Elements::ActivationEnergy: {
            traitElementStr = string("activation energy");
            break;
        }
        case Elements::EnergyDecay: {
            traitElementStr = string("energy decay");
            break;
        }
        case Elements::LowerThreshold: {
            traitElementStr = string("lower threshold");
            break;
        }
        case Elements::UpperThreshold: {
            traitElementStr = string("upper threshold");
            break;
        }
        case Elements::TemperatureOptimal: {
            traitElementStr = string("temperature optimal");
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }


    auto traitOrderIt = find(individualLevelTraitsOrder.cbegin(), individualLevelTraitsOrder.cend(), traitType + "$" + EnumClass<Elements>::to_string(traitElement));

	switch(EnumClass<Type>::stringToEnumValue(info["definitionType"])) {
        case Type::SpeciesLevel: {
            if(traitOrderIt != individualLevelTraitsOrder.cend())
            {
                throwLineInfoException("Error: The " + traitElementStr + " of trait '" + traitType + "' is species level and must not have a defined order.");
            }

            return make_unique<SpeciesLevelTrait>(info["speciesLevelParams"]);
            break;
        }
        case Type::IndividualLevel: {
            if(traitOrderIt == individualLevelTraitsOrder.cend())
            {
                throwLineInfoException("Error: The " + traitElementStr + " of trait '" + traitType + "' has no defined order.");
            }

            return make_unique<IndividualLevelTrait>(info["individualLevelParams"], distance(individualLevelTraitsOrder.cbegin(), traitOrderIt), loci, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule);
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}



TraitDefinitionSection::TraitDefinitionSection()
{
	
}

TraitDefinitionSection::~TraitDefinitionSection() 
{
	
}
