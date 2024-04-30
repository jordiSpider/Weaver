#include "IBM/World/LivingBeings/Animals/AnimalStatisticalGrowth.h"

#include "IBM/World/LivingBeings/Animals/AnimalStatistical.h"


using namespace std;


AnimalStatisticalGrowth::AnimalStatisticalGrowth(AnimalStatistical* const animal, const double& factorEggMass)
	: AnimalGrowth(animal, factorEggMass, false), animal(animal)
{
	
}

AnimalStatisticalGrowth::~AnimalStatisticalGrowth()
{

}

const AnimalStatistical* const AnimalStatisticalGrowth::getAnimal() const
{
	return animal;
}

AnimalStatistical* const AnimalStatisticalGrowth::getMutableAnimal() const
{
	return animal;
}

void AnimalStatisticalGrowth::forceMolting()
{
    if(getAnimal()->getInstar() >= getAnimal()->getSpecies()->getGrowthModule()->getInstarFirstMaturation())
	{
		getMutableAnimal()->setMatureValue(true);
	}

	double ageAtInitialization = (getAnimal()->getInstar() > 1) ? getInstarAge(getAnimal()->getInstar()) : 0.0;

	getMutableAnimal()->setCurrentAge(ageAtInitialization);
	
	double expectedLengthAtCurrentAge = getIndividualGrowthModel()->calculateLength(ageAtInitialization, getAnimal()->getFinalTraitValue(Trait::Type::growth));
	double expectedMassAtCurrentAge = getAnimal()->getSpecies()->calculateDryLength(expectedLengthAtCurrentAge, getAnimal()->isMature());

	setDryMass(expectedMassAtCurrentAge);
}
