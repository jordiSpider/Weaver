#include "IBM/World/LivingBeings/Animals/AnimalGrowth.h"

#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "IBM/World/World.h"


using namespace std;


AnimalGrowth::AnimalGrowth(Animal* const animal, const double& factorEggMass, const bool growthModuleTemperatureDependent)
    : AnimalInterfaceGrowth(), animal(animal), individualInstarDevTimeVector(getAnimal()->getSpecies()->getInstarDevTimeVector().size(), 0.0),
      instarLengthsVector(getAnimal()->getSpecies()->getInstarDevTimeVector().size(), 0.0), 
      instarMassesVector(getAnimal()->getSpecies()->getInstarDevTimeVector().size(), 0.0),
      baseDevTimeProportion(animal->getBaseTraitValue(Trait::Type::devTime) / getAnimal()->getSpecies()->getInstarDevTimeVector().back()),
	  growthModuleTemperatureDependent(growthModuleTemperatureDependent)
{
	currentBodySizeInitialised = false;

    calculateValuesAtBirth(factorEggMass);

    individualGrowthModel = IndividualGrowthModel::createInstance(animal->getSpecies()->getGrowthModule()->getGrowthModelType()).release();
	individualGrowthModel->adjust(
		getAnimal()->getFinalTraitValue(Trait::Type::growth), getAnimal()->getFinalTraitValue(Trait::Type::lengthAtMaturation), 
		getAnimal()->getFinalTraitValue(Trait::Type::devTime), getLengthAtBirth()
	);

    calculateGrowthCurves();
}

AnimalGrowth::~AnimalGrowth()
{
	delete individualGrowthModel;
}

const Animal* const AnimalGrowth::getAnimal() const
{
    return animal;
}

Animal* const AnimalGrowth::getMutableAnimal() const
{
    return animal;
}

const double& AnimalGrowth::getInstarAge(const Instar& instar) const
{
    return individualInstarDevTimeVector[instar-1];
}

const CustomIndexedVector<Instar, double>& AnimalGrowth::getIndividualInstarDevTimeVector() const
{
    return individualInstarDevTimeVector;
}

void AnimalGrowth::calculateValuesAtBirth(const double& factorEggMass)
{
    eggDryMassAtBirth = getAnimal()->getSpecies()->getGrowthModule()->getEggDryMass() + getAnimal()->getSpecies()->getGrowthModule()->getEggDryMass()*factorEggMass;

	if(getAnimal()->getGender() == AnimalSpecies::Gender::FEMALE)
	{
		double maxMassAtCurrentAge = eggDryMassAtBirth + eggDryMassAtBirth*getAnimal()->getSpecies()->getMaxPlasticityKVonBertalanffy();
		double minMassAtCurrentAge = eggDryMassAtBirth - eggDryMassAtBirth*getAnimal()->getSpecies()->getMinPlasticityKVonBertalanffy();

		eggDryMassAtBirth = fmin(eggDryMassAtBirth, maxMassAtCurrentAge);
		eggDryMassAtBirth = fmax(eggDryMassAtBirth, minMassAtCurrentAge);
	}

    lengthAtBirth = getAnimal()->getSpecies()->calculateDryLength(eggDryMassAtBirth, false);
}

const double& AnimalGrowth::getEggDryMassAtBirth() const
{
	return eggDryMassAtBirth;
}

const double& AnimalGrowth::getLengthAtBirth() const
{
    return lengthAtBirth;
}

const IndividualGrowthModel* const AnimalGrowth::getIndividualGrowthModel() const
{
    return individualGrowthModel;
}

const bool AnimalGrowth::isGrowthModuleTemperatureDependent() const
{
    return growthModuleTemperatureDependent;
}

void AnimalGrowth::calculateGrowthCurves()
{
    double finalDevTimeProportion;

    if(isGrowthModuleTemperatureDependent())
    {
        individualGrowthModel->adjust(
			getAnimal()->getFinalTraitValue(Trait::Type::growth), getAnimal()->getFinalTraitValue(Trait::Type::lengthAtMaturation), 
			getAnimal()->getFinalTraitValue(Trait::Type::devTime), getLengthAtBirth()
		);

        finalDevTimeProportion = getAnimal()->getFinalTraitValue(Trait::Type::devTime) / (getAnimal()->getSpecies()->getInstarDevTimeVector().back() * baseDevTimeProportion);
    }
    else
    {
        finalDevTimeProportion = 1.0;
    }


	for(unsigned int i = 1; i <= getAnimal()->getSpecies()->getInstarDevTimeVector().size(); i++)
	{
		Instar instar(i);

		individualInstarDevTimeVector[instar] = getAnimal()->getSpecies()->getInstarDevTime(instar) * baseDevTimeProportion * finalDevTimeProportion;

        instarLengthsVector[instar] = getIndividualGrowthModel()->calculateLength(individualInstarDevTimeVector[instar], getAnimal()->getFinalTraitValue(Trait::Type::growth));
        
        instarMassesVector[instar] = getAnimal()->getSpecies()->calculateDryMass(
			instarLengthsVector[instar], 
			instar >= getAnimal()->getSpecies()->getGrowthModule()->getInstarFirstMaturation()
		);
	}
}

void AnimalGrowth::setDryMass(const double &newDryMass)
{
	double newEnergyTank, newCurrentBodySize;

	tie(newEnergyTank, newCurrentBodySize) = getAnimal()->getSpecies()->decomposeMassElements(newDryMass, getAnimal()->getFinalTraitValue(Trait::Type::energy_tank));

	setDryMass(
		newDryMass,
		newCurrentBodySize,
		newEnergyTank
	);
}

void AnimalGrowth::setDryMass(const double &newDryMass, const double& investment)
{
	double newEnergyTank, newCurrentBodySize;

	tie(newEnergyTank, newCurrentBodySize) = getAnimal()->getSpecies()->decomposeMassElements(newDryMass, investment, getAnimal()->getFinalTraitValue(Trait::Type::energy_tank));

	setDryMass(
		newDryMass,
		newCurrentBodySize,
		newEnergyTank
	);
}

void AnimalGrowth::setDryMass(const double &newDryMass, const double &newCurrentBodySize, const double& newEnergyTank)
{
	getMutableAnimal()->modifyEnergyTank(newEnergyTank);
	setCurrentBodySize(newCurrentBodySize);

	getMutableAnimal()->getMutableSpecies()->updateMaximumDryMassObserved(newDryMass);
}

#ifdef DEBUG
	void AnimalGrowth::testGetCurrentBodySize() const
	{
		if(!currentBodySizeInitialised)
		{
			throwLineInfoException("Trying to read an uninitialised variable.");
		}
	}
#endif

const double& AnimalGrowth::getCurrentBodySize() const 
{ 
	#ifdef DEBUG
		testGetCurrentBodySize();
	#endif

	return currentBodySize; 
}

#ifdef DEBUG
	void AnimalGrowth::testSetCurrentBodySize()
	{
		if(currentBodySizeInitialised)
		{
			if(currentBodySizeLastTimeStep == getAnimal()->getSpecies()->getWorld()->getActualTimeStep())
			{
				// throwLineInfoException("Variable modified twice in the same time step.");
			}
		}
		else
		{
			currentBodySizeInitialised = true;
		}

		currentBodySizeLastTimeStep = getAnimal()->getSpecies()->getWorld()->getActualTimeStep();
	}
#endif

void AnimalGrowth::setCurrentBodySize(const double& newCurrentBodySize)
{
	#ifdef DEBUG
		testSetCurrentBodySize();
	#endif

	currentBodySize = newCurrentBodySize;
}
