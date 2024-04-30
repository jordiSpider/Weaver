#include "IBM/World/LivingBeings/Animals/Genetics/Traits/Trait.h"


using namespace std;
using json = nlohmann::json;



CustomIndexedVector<Trait::Type, bool> Trait::inverseTraitVector({
		false,	// energy_tank
		false,	// growth
		true,	// eggDevTime
		false,	// factorEggMass
		false,	// assim
		false,	// voracity
		false,	// speed
		false,	// search_area
		false,	// met_rate
		false,	// shock_resistance
		true,	// actE_met
		false,	// memoryDepth
		false,	// perception_area
		false,	// interaction_area
		true, 	// devTime
		false, 	// lengthAtMaturation
		true,	// pupaPeriodLength
		true	// timeAddedToMeetLastRepro
	}
);


Trait::Trait(const string& traitType, const json& info, const vector<string>& individualLevelTraitsOrder, 
			 const Temperature& tempFromLab, const vector<Locus*> &loci, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const vector<double>& rhoPerModule, const vector<unsigned int>& rhoRangePerModule)
	: type(EnumClass<Type>::stringToEnumValue(traitType)),
	  thermallyDependent((type == Trait::Type::energy_tank) ? false : static_cast<bool>(info["temperature"]["dependent"]))
{
	traitElements.push_back(TraitDefinitionSection::createInstance(traitType, info, individualLevelTraitsOrder, TraitDefinitionSection::Elements::TraitValue, loci, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule).release());

	if(isThermallyDependent())
	{
		if(type == Trait::Type::lengthAtMaturation)
		{
			temperatureSection = new TempSizeRuleTraitTemperatureSection(info["temperature"], tempFromLab);
		}
		else
		{
			temperatureSection = new DellsTraitTemperatureSection(traitType, info["temperature"], individualLevelTraitsOrder, isInverse(), loci, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule);

			traitElements.resize(EnumClass<TraitDefinitionSection::Elements>::size(), nullptr);
			traitElements[TraitDefinitionSection::Elements::ActivationEnergy] = static_cast<DellsTraitTemperatureSection*>(temperatureSection)->getActivationEnergy();
			traitElements[TraitDefinitionSection::Elements::EnergyDecay] = static_cast<DellsTraitTemperatureSection*>(temperatureSection)->getEnergyDecay();
			traitElements[TraitDefinitionSection::Elements::LowerThreshold] = static_cast<DellsTraitTemperatureSection*>(temperatureSection)->getLowerThreshold();
			traitElements[TraitDefinitionSection::Elements::UpperThreshold] = static_cast<DellsTraitTemperatureSection*>(temperatureSection)->getUpperThreshold();
			traitElements[TraitDefinitionSection::Elements::TemperatureOptimal] = static_cast<DellsTraitTemperatureSection*>(temperatureSection)->getTemperatureOptimal();
		}
	}
	else
	{
		temperatureSection = nullptr;
	}
}

Trait::~Trait() 
{
	delete traitElements[TraitDefinitionSection::Elements::TraitValue];

	if(isThermallyDependent())
	{
		delete temperatureSection;
	}
}

const CustomIndexedVector<TraitDefinitionSection::Elements, TraitDefinitionSection*>& Trait::getElements() const
{
	return traitElements;
}

CustomIndexedVector<TraitDefinitionSection::Elements, TraitDefinitionSection*>& Trait::getMutableElements()
{
	return traitElements;
}

const Trait::Type Trait::getType() const 
{ 
	return type;
}

const bool Trait::isInverse() const
{
	return inverseTraitVector[type];
}

const bool Trait::isThermallyDependent() const
{
	return thermallyDependent;
}

const TraitTemperatureSection& Trait::getTemperatureSection() const
{
	return *temperatureSection;
}

TraitTemperatureSection& Trait::getMutableTemperatureSection()
{
	return *temperatureSection;
}

double Trait::applyTemperatureDependency(const Temperature& temperature, const CustomIndexedVector<TraitDefinitionSection::Elements, double>& traitElements,
        const map<Temperature, pair<double, double>>& lowerTempSizeRuleConstantAccumulationVector, const double& lastLowerTempSizeRuleConstant, 
        const map<Temperature, pair<double, double>>& upperTempSizeRuleConstantAccumulationVector, const double& lastUpperTempSizeRuleConstant,
		const AnimalSpecies* const animalSpecies
	) const
{
	double finalTraitValue = getTemperatureSection().applyTemperatureDependency(
		temperature, traitElements, lowerTempSizeRuleConstantAccumulationVector, lastLowerTempSizeRuleConstant, 
		upperTempSizeRuleConstantAccumulationVector, lastUpperTempSizeRuleConstant, animalSpecies
	);

	if(getType() == Trait::Type::voracity ||
	   getType() == Trait::Type::search_area ||
	   getType() == Trait::Type::speed)
	{
		finalTraitValue = fmax(finalTraitValue, 0.0);
	}

	return finalTraitValue;
}