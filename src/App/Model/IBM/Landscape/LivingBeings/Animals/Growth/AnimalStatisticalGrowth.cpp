#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalStatisticalGrowth.h"


using namespace std;


AnimalStatisticalGrowth::AnimalStatisticalGrowth(AnimalSpeciesGrowth* const speciesGrowth, const Instar &instar, const bool temperatureDependent, Gender gender, const std::vector<IndividualTrait>& baseIndividualTraits, const PreciseDouble& factorEggMassValue, const unsigned int femaleMaxReproductionEvents, const Temperature& actualTemperature, const Temperature& tempFromLab, const PreciseDouble& timeStepsPerDay)
	: AnimalGrowth(speciesGrowth, instar, temperatureDependent, gender, baseIndividualTraits, factorEggMassValue, femaleMaxReproductionEvents, actualTemperature, tempFromLab, timeStepsPerDay)
{
	
}

AnimalStatisticalGrowth::~AnimalStatisticalGrowth()
{

}

void AnimalStatisticalGrowth::forceMolting(const std::vector<IndividualTrait>& baseIndividualTraits, Gender gender, const PreciseDouble& timeStepsPerDay)
{
    if(getInstar() >= getSpeciesGrowth()->getInstarFirstMaturation())
	{
		setMatureValue(true, gender);
	}

	TimeStep ageAtInitialization = (getInstar() > 1) ? getInstarAge(getInstar()) : TimeStep(0);

	setCurrentAge(ageAtInitialization);
	
	Length expectedLengthAtCurrentAge = getIndividualGrowthModel()->calculateLength(Day(ageAtInitialization, timeStepsPerDay), baseIndividualTraits[static_cast<size_t>(BaseTraitType::growth)].getPhenotypicValue());
	DryMass expectedMassAtCurrentAge = getSpeciesGrowth()->calculateDryMass(expectedLengthAtCurrentAge, isMature());

	setDryMass(expectedMassAtCurrentAge, expectedMassAtCurrentAge, baseIndividualTraits[static_cast<size_t>(BaseTraitType::energy_tank)].getPhenotypicValue(), (getInstar() > 1), gender, false, TimeStep(0));
}

bool AnimalStatisticalGrowth::hasCapitalBreeding() const
{
	return false;
}
