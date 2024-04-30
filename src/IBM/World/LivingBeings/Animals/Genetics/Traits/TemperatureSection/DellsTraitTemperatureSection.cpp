#include "IBM/World/LivingBeings/Animals/Genetics/Traits/TemperatureSection/DellsTraitTemperatureSection.h"


using namespace std;
using json = nlohmann::json;




DellsTraitTemperatureSection::DellsTraitTemperatureSection(const string& traitType, const json& info, const vector<string>& individualLevelTraitsOrder, const bool traitInverse, const vector<Locus*> &loci, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const vector<double>& rhoPerModule, const vector<unsigned int>& rhoRangePerModule)
	: TraitTemperatureSection(), 
	  activationEnergy(TraitDefinitionSection::createInstance(traitType, info["activationEnergy"], individualLevelTraitsOrder, TraitDefinitionSection::Elements::ActivationEnergy, loci, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule).release()), 
	  energyDecay(TraitDefinitionSection::createInstance(traitType, info["energyDecay"], individualLevelTraitsOrder, TraitDefinitionSection::Elements::EnergyDecay, loci, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule).release()), 
	  lowerThreshold(TraitDefinitionSection::createInstance(traitType, info["threshold"]["lower"], individualLevelTraitsOrder, TraitDefinitionSection::Elements::LowerThreshold, loci, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule).release()), 
	  upperThreshold(TraitDefinitionSection::createInstance(traitType, info["threshold"]["upper"], individualLevelTraitsOrder, TraitDefinitionSection::Elements::UpperThreshold, loci, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule).release()), 
	  temperatureOptimal(TraitDefinitionSection::createInstance(traitType, info["temperatureOptimal"], individualLevelTraitsOrder, TraitDefinitionSection::Elements::TemperatureOptimal, loci, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule).release()),
	  traitInverse(traitInverse)
{
	
}

DellsTraitTemperatureSection::~DellsTraitTemperatureSection() 
{
	delete activationEnergy;
    delete energyDecay;

    delete lowerThreshold;
    delete upperThreshold;

    delete temperatureOptimal;
}

TraitDefinitionSection* DellsTraitTemperatureSection::getActivationEnergy()
{
	return activationEnergy;
}

TraitDefinitionSection* DellsTraitTemperatureSection::getEnergyDecay()
{
	return energyDecay;
}

TraitDefinitionSection* DellsTraitTemperatureSection::getLowerThreshold()
{
	return lowerThreshold;
}

TraitDefinitionSection* DellsTraitTemperatureSection::getUpperThreshold()
{
	return upperThreshold;
}

TraitDefinitionSection* DellsTraitTemperatureSection::getTemperatureOptimal()
{
	return temperatureOptimal;
}

const bool DellsTraitTemperatureSection::isInverse() const
{
	return traitInverse;
}

unique_ptr<tuple<map<Temperature, pair<double, double>>, double, map<Temperature, pair<double, double>>, double>> DellsTraitTemperatureSection::calculateTempSizeRuleConstantAccumulationVector(const double& lengthAtTempFromLab, const AnimalSpecies* const animalSpecies) const
{
	throwLineInfoException("Error: Do not use this method with the class 'SpeciesLevelTrait'.");
}

double DellsTraitTemperatureSection::applyTemperatureDependency(const Temperature& temperature, const CustomIndexedVector<TraitDefinitionSection::Elements, double>& traitElements,
        const map<Temperature, pair<double, double>>& lowerTempSizeRuleConstantAccumulationVector, const double& lastLowerTempSizeRuleConstant, 
        const map<Temperature, pair<double, double>>& upperTempSizeRuleConstantAccumulationVector, const double& lastUpperTempSizeRuleConstant,
		const AnimalSpecies* const animalSpecies
    ) const
{
	double initialTraitValue = (isInverse()) ? 1.0 / traitElements[TraitDefinitionSection::Elements::TraitValue] : traitElements[TraitDefinitionSection::Elements::TraitValue];

	double finalTraitValue = MathFunctions::useJohnsonLewis(temperature, traitElements[TraitDefinitionSection::Elements::ActivationEnergy], 
		Temperature(traitElements[TraitDefinitionSection::Elements::LowerThreshold]), Temperature(traitElements[TraitDefinitionSection::Elements::UpperThreshold]), 
		Temperature(traitElements[TraitDefinitionSection::Elements::TemperatureOptimal]), traitElements[TraitDefinitionSection::Elements::EnergyDecay], 
		initialTraitValue
	);

	finalTraitValue = (isInverse()) ? 1.0 / finalTraitValue : finalTraitValue;

	return finalTraitValue;
}
