
#include "IBM/World/LivingBeings/Animals/AnimalNonStatisticalGrowth.h"

#include "IBM/World/LivingBeings/Animals/AnimalNonStatistical.h"
#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"
#include "IBM/World/Map/Map.h"
#include "IBM/World/World.h"

using namespace std;
using json = nlohmann::json;

AnimalNonStatisticalGrowth::AnimalNonStatisticalGrowth(AnimalNonStatistical* const animal, const double& factorEggMass, const double& actualTime)
    : AnimalGrowth(
		animal, factorEggMass, 
		animal->getSpecies()->getTrait(Trait::Type::growth).isThermallyDependent() ||
        animal->getSpecies()->getTrait(Trait::Type::lengthAtMaturation).isThermallyDependent() ||
        animal->getSpecies()->getTrait(Trait::Type::devTime).isThermallyDependent()
	  ), animal(animal)
{   
    pupaTimer = 0;

    diapauseTimeSteps = 0;

	tankAtGrowth = 0.0;

	actualMoltingTimeVector.reserve(getAnimal()->getSpecies()->getNumberOfInstars()-1);
	actualMoltingMassVector.reserve(getAnimal()->getSpecies()->getNumberOfInstars()-1);

	date_egg = actualTime;

	getMutableAnimal()->setBaseTraitElement(
		Trait::Type::eggDevTime, TraitDefinitionSection::Elements::TraitValue, 
		getDateEgg() + getAnimal()->getBaseTraitValue(Trait::Type::eggDevTime)
	);
}

AnimalNonStatisticalGrowth::~AnimalNonStatisticalGrowth()
{
    
}

const AnimalNonStatistical* const AnimalNonStatisticalGrowth::getAnimal() const
{
    return animal;
}

AnimalNonStatistical* const AnimalNonStatisticalGrowth::getMutableAnimal() const
{
    return animal;
}

void AnimalNonStatisticalGrowth::calculateTotalReproMass()
{
	totalReproMass = 0.0;
	for(Instar instar = getAnimal()->getSpecies()->getGrowthModule()->getInstarFirstReproduction(); instar <= getAnimal()->getSpecies()->getLastInstar(); ++instar)
	{
		totalReproMass += getInstarMass(instar);
	}
}

void AnimalNonStatisticalGrowth::tune(ostream& tuneTraitsFile, bool printGrowthData)
{
    calculateGrowthCurves();


	double lengthPredicted = getIndividualGrowthModel()->calculateLength(getAnimal()->getCurrentAge(), getAnimal()->getFinalTraitValue(Trait::Type::growth));

	double massPredicted = getAnimal()->getSpecies()->calculateDryMass(lengthPredicted, getAnimal()->isMature());


	// moulting only if realized mass is > predicted mass
	if(getAnimal()->getTerrainCellInterface()->getMap().getWorld()->isDinosaurs() && getAnimal()->getInstar() < getAnimal()->getSpecies()->getNumberOfInstars()){
		if(getAnimal()->calculateDryMass() > massPredicted){	
			setDryMass(getAnimal()->calculateDryMass());
		}
	}


	double minMassAtCurrentAge;

	// this below is for the cases of death from starvation - so it depends on the rule
	if(getAnimal()->getCurrentAge() > (0.15*getLongevity()))
	{
		minMassAtCurrentAge = massPredicted - massPredicted * getAnimal()->getSpecies()->getMinPlasticityKVonBertalanffy();
	}
	else
	{
    	minMassAtCurrentAge = massPredicted - massPredicted * 3 * getAnimal()->getSpecies()->getMinPlasticityKVonBertalanffy();
	}


	if(printGrowthData)
	{
		tuneTraitsFile << minMassAtCurrentAge << "\t";
	}
}

const CustomIndexedVector<Instar, double>& AnimalNonStatisticalGrowth::getInstarLengthsVector() const
{
    return instarLengthsVector;
}

const double& AnimalNonStatisticalGrowth::getInstarLength(const Instar& instar) const
{
    return instarLengthsVector[instar-1];
}

const CustomIndexedVector<Instar, double>& AnimalNonStatisticalGrowth::getInstarMassesVector() const
{
    return instarMassesVector;
}

const double& AnimalNonStatisticalGrowth::getInstarMass(const Instar& instar) const
{
    return instarMassesVector[instar-1];
}

const double AnimalNonStatisticalGrowth::getAgeOfFirstMaturation() const
{
    return getInstarAge(getAnimal()->getSpecies()->getGrowthModule()->getInstarFirstMaturation()) + getAnimal()->getFinalTraitValue(Trait::Type::pupaPeriodLength);
}

void AnimalNonStatisticalGrowth::calculateLongevity()
{
    longevity = getAnimal()->getSpecies()->getLongevitySinceMaturation() * getAgeOfFirstMaturation();
}

const double AnimalNonStatisticalGrowth::calculateClutchDryMass(const double& individualDryMass) const
{
	return getAnimal()->getSpecies()->calculateEggsPerBatch(individualDryMass)*getEggDryMassAtBirth();
}

//TODO parametro para que se ejecuten cada X timesteps
void AnimalNonStatisticalGrowth::grow()
{
	#ifdef DEBUG
		double lastEnergyTank = getAnimal()->getCurrentEnergyTank();
		double lastBodySize = getAnimal()->getFinalTraitValue(Trait::Type::factorEggMass);
	#endif

	//Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::eggDevTime] + 1.0/timeStepsPerDay;

	if(getAnimal()->isMature())
	{
		//Jordi: the following is to force males to molt whether they mate or not - otherwise they stop growing if the do not mate
		if(getAnimal()->getGender() == AnimalSpecies::Gender::MALE)
		{
			if(getAnimal()->getSpecies()->getGrowthModule()->hasIndeterminateGrowth() && (getAnimal()->getInstar() < getAnimal()->getSpecies()->getNumberOfInstars())){
				if(getAnimal()->getCurrentAge() > getInstarAge(getAnimal()->getInstar())+1){//plus one day enough to mate
			    	getMutableAnimal()->setMatureValue(false);
				}
			}
		}
		else
		{
			double timeOfReproEvent;

			if(getAnimal()->hasCapitalBreeding())
			{
				// because the first one is on the pupaPerdiodLength
				double totalTimeBreedingCapitally = getAnimal()->getSpecies()->getTimeOfReproEventDuringCapitalBreeding()*(getAnimal()->getSpecies()->getNumberOfCapitalBreeds()-1);
				timeOfReproEvent = ((getLongevity() - totalTimeBreedingCapitally - getAgeOfFirstMaturation()) / (getAnimal()->getSpecies()->getFemaleMaxReproductionEvents() - getAnimal()->getSpecies()->getNumberOfCapitalBreeds())) - getAnimal()->getFinalTraitValue(Trait::Type::timeAddedToMeetLastRepro);
			}
			else
			{
				timeOfReproEvent = ((getLongevity() - getAgeOfFirstMaturation()) / getAnimal()->getSpecies()->getFemaleMaxReproductionEvents()) - getAnimal()->getFinalTraitValue(Trait::Type::timeAddedToMeetLastRepro);
			}


			double currentMass = getAnimal()->calculateDryMass();


			// Exception of capital breeders which comes from below at repro 0
			if(getAnimal()->hasCapitalBreeding() && 
			   getAnimal()->getReproCounter() < getAnimal()->getMaxReproductionEvents() && 
			   getAnimal()->getReproCounter() >= getAnimal()->getSpecies()->getNumberOfCapitalBreeds())
			{
				double clutchDryMass = calculateClutchDryMass(currentMass);

				getMutableAnimal()->setAgeForNextReproduction(getAnimal()->getCurrentAge() + timeOfReproEvent);
				setMassForNextReproduction(currentMass + clutchDryMass);

				getMutableAnimal()->disableCapitalBreeding();
			}


			if(!getAnimal()->getSpecies()->getGrowthModule()->hasIndeterminateGrowth() && 
			   !getAnimal()->hasCapitalBreeding() && 
			   getAnimal()->getReproCounter() < getAnimal()->getMaxReproductionEvents())
			{
				if(getAnimal()->getReproCounter() == 0 &&
				   getAnimal()->getCurrentAge() >= getAgeOfFirstMaturation())
				{
					double clutchDryMass = calculateClutchDryMass(currentMass);

					getMutableAnimal()->setAgeForNextReproduction(getAgeOfFirstMaturation() + timeOfReproEvent);
					setMassForNextReproduction(currentMass + clutchDryMass);
				}


				if(getMassForNextReproduction() <= currentMass)
				{
					double clutchDryMass = calculateClutchDryMass(currentMass);

					getMutableAnimal()->setAgeForNextReproduction(getAnimal()->getCurrentAge() + timeOfReproEvent);
					setMassForNextReproduction(currentMass + clutchDryMass);
				}
			}


			if(getAnimal()->getSpecies()->getGrowthModule()->hasIndeterminateGrowth())
			{
				if(getAnimal()->getReproCounter() == 0 &&
				   getAnimal()->getCurrentAge() >= getAgeOfFirstMaturation())
				{
					getMutableAnimal()->setAgeForNextReproduction(getAgeOfFirstMaturation() + timeOfReproEvent);
					
					
					double nextLength = getIndividualGrowthModel()->calculateLength(getAnimal()->getAgeForNextReproduction(), getAnimal()->getFinalTraitValue(Trait::Type::growth));
					double nextMass = getAnimal()->getSpecies()->calculateDryLength(nextLength, getAnimal()->isMature());
					
					double fractFirstRepro = getInstarMass(getAnimal()->getSpecies()->getGrowthModule()->getInstarFirstReproduction())/getTotalReproMass();

					double currentEggs = fractFirstRepro*getAnimal()->getSpecies()->getTotFec();

					double clutchDryMass = currentEggs*getEggDryMassAtBirth();

					setMassForNextReproduction(nextMass + clutchDryMass);
				}

				if(getMassForNextReproduction() <= currentMass &&
				   getAnimal()->getReproCounter() < getAnimal()->getMaxReproductionEvents()-1)
				{
					getMutableAnimal()->setAgeForNextReproduction(getAnimal()->getCurrentAge() + timeOfReproEvent);
					
					
					double nextLength = getIndividualGrowthModel()->calculateLength(getAnimal()->getAgeForNextReproduction(), getAnimal()->getFinalTraitValue(Trait::Type::growth));
					double nextMass = getAnimal()->getSpecies()->calculateDryLength(nextLength, getAnimal()->isMature());
					
					double fractFirstRepro = getInstarMass(getAnimal()->getSpecies()->getGrowthModule()->getInstarFirstReproduction()+getAnimal()->getReproCounter())/getTotalReproMass();

					double currentEggs = fractFirstRepro*getAnimal()->getSpecies()->getTotFec();

					double clutchDryMass = currentEggs*getEggDryMassAtBirth();

					setMassForNextReproduction(nextMass + clutchDryMass);
				}
			}


			if(getAnimal()->hasCapitalBreeding() &&
			   getAnimal()->getReproCounter() < getAnimal()->getMaxReproductionEvents())
			{
				if(getAnimal()->getReproCounter() == 0 &&
				   getAnimal()->getCurrentAge() >= getAgeOfFirstMaturation())
				{
					getMutableAnimal()->setAgeForNextReproduction(getAgeOfFirstMaturation());
					setMassForNextReproduction(currentMass);
				}

				if(getAnimal()->getAgeForNextReproduction() < getAnimal()->getCurrentAge())
				{
					if(getAnimal()->getReproCounter() < getAnimal()->getSpecies()->getNumberOfCapitalBreeds())
					{
						getMutableAnimal()->setAgeForNextReproduction(getAnimal()->getCurrentAge() + getAnimal()->getSpecies()->getTimeOfReproEventDuringCapitalBreeding());
						setMassForNextReproduction(currentMass);
					}
					else
					{
						setMassForNextReproduction(currentMass);
					}
				}
			}


			//Only FEMALES are coupled
			if(getAnimal()->calculateDryMass() >= getMassForNextReproduction() && getAnimal()->getCurrentAge() >= getAnimal()->getAgeForNextReproduction() && getAnimal()->getSpecies()->getEggsPerBatch() > 0)  //Arthropods post-dinos July 2021  && getSpecies()->getEggsPerBatch()>0
			{
				if(getAnimal()->getSpecies()->getSexualType() != SexualType::diploid || 
				   (getAnimal()->getSpecies()->getSexualType() == SexualType::diploid && getAnimal()->isMated()))
				{
					getMutableAnimal()->setNewLifeStage(LifeStage::REPRODUCING);
					getMutableAnimal()->setAgeOfLastMoultOrReproduction(getAnimal()->getCurrentAge());
				}
			}
		}
	}
	else
	{
		//arthro - assignedForMolt is considered for molting or investment could be lower than the actual target
		if(getAnimal()->calculateDryMass() >= getInstarMass(getAnimal()->getInstar()+1) && getAnimal()->getCurrentAge() >= getInstarAge(getAnimal()->getInstar()+1))
		{
            double investment = getAnimal()->calculateDryMass()*getAnimal()->getSpecies()->getAssignedForMolt();
			setDryMass(getInstarMass(getAnimal()->getInstar()+1), investment);

			setLastMassBeforeMaturationOrOviposition(getAnimal()->calculateDryMass());
			
			tankAtGrowth = getAnimal()->getCurrentEnergyTank();

			#ifdef DEBUG
				double afterMoultEnergyTank = getAnimal()->getCurrentEnergyTank();
				double afterMoultBodySize = getAnimal()->getFinalTraitValue(Trait::Type::factorEggMass);
				if(afterMoultEnergyTank >= lastEnergyTank)
				{
					cout << "The energy tank after moult was higher than before:" << endl;
					cout << " - Animal: " << getAnimal()->getId() << "(" << getAnimal()->getSpecies()->getScientificName() << ")" << endl;
					cout << " - Last energy tank: " << lastEnergyTank << endl;
					cout << " - After moult energy tank: " << afterMoultEnergyTank << endl;
				}
				if(afterMoultBodySize <= lastBodySize)
				{
					cout << "The body size after moult was lower than before: " << endl;
					cout << " - Animal: " << getAnimal()->getId() << "(" << getAnimal()->getSpecies()->getScientificName() << ")" << endl;
					cout << " - Last body size: " << lastBodySize << endl;
					cout << " - After moult body size: " << afterMoultBodySize << endl;
				}
			#endif
			
			getMutableAnimal()->moveOnNextInstar();
			actualMoltingTimeVector.push_back(getAnimal()->getCurrentAge()+1);
			getMutableAnimal()->setAgeOfLastMoultOrReproduction(getAnimal()->getCurrentAge());

			getMutableAnimal()->setMatureValue(getAnimal()->getInstar() >= getAnimal()->getSpecies()->getGrowthModule()->getInstarFirstMaturation());


			if(getAnimal()->isMature())
			{
                //TODO TIMESTEP
				//This gets decremented in activateAnimals at the beginning of each day.
				pupaTimer = getAnimal()->getFinalTraitValue(Trait::Type::pupaPeriodLength);
				
                //DONE RepFactor for different genders
				if(getAnimal()->getGender() == AnimalSpecies::Gender::FEMALE)
				{
					double clutchDryMass;

					if(getAnimal()->getSpecies()->getGrowthModule()->hasIndeterminateGrowth())
					{
						double fractFirstRepro = getInstarMass(getAnimal()->getSpecies()->getGrowthModule()->getInstarFirstReproduction())/getTotalReproMass();

						double currentEggs = fractFirstRepro*getAnimal()->getSpecies()->getTotFec();

						clutchDryMass = currentEggs*getEggDryMassAtBirth();
					}
					else
					{
						clutchDryMass = calculateClutchDryMass(getLastMassBeforeMaturationOrOviposition());
					}

					setMassForNextReproduction(getLastMassBeforeMaturationOrOviposition() + clutchDryMass);
				}
				else //is male
				{
					double currentLength = getIndividualGrowthModel()->calculateLength(getAnimal()->getCurrentAge(), getAnimal()->getFinalTraitValue(Trait::Type::growth));
					double currentMass = getAnimal()->getSpecies()->calculateDryMass(currentLength, getAnimal()->isMature());

					double nextReproLength = getIndividualGrowthModel()->calculateLength(getAnimal()->getCurrentAge()+1, getAnimal()->getFinalTraitValue(Trait::Type::growth));
					double nextReproMass = getAnimal()->getSpecies()->calculateDryMass(nextReproLength, getAnimal()->isMature());

					if(getAnimal()->getSpecies()->getGrowthModule()->hasIndeterminateGrowth())
					{
						if(getAnimal()->getInstar() < getAnimal()->getSpecies()->getNumberOfInstars())
						{
							setMassForNextReproduction(nextReproMass);
						}
						else
						{
							setMassForNextReproduction(currentMass);
						}
					}
					else
					{
						setMassForNextReproduction(getLastMassBeforeMaturationOrOviposition() + (nextReproMass-currentMass));
					}
				}

				getMutableAnimal()->setNewLifeStage(LifeStage::PUPA);
					
				getMutableAnimal()->setAgeOfLastMoultOrReproduction(getAnimal()->getCurrentAge());

				//dinos2023 - to ensure that ageForNextReproduction does not surpass the next instar (year)
				if(getAnimal()->getInstar() < getAnimal()->getSpecies()->getNumberOfInstars()){
					getMutableAnimal()->setAgeForNextReproduction(fmin(getAnimal()->getAgeForNextReproduction(), getInstarAge(getAnimal()->getInstar()) + round(0.9*(getInstarAge(getAnimal()->getInstar()+1)-getInstarAge(getAnimal()->getInstar())))));
				}
				else
				{
					getMutableAnimal()->setAgeForNextReproduction(getAnimal()->getCurrentAge() + getAnimal()->getFinalTraitValue(Trait::Type::pupaPeriodLength)*getAnimal()->getTerrainCellInterface()->getMap().getWorld()->getTimeStepsPerDay());
				}
				//end dinos2023
			}
		}
	}
}

void AnimalNonStatisticalGrowth::isReadyToResumeFromPupaOrDecreasePupaTimer()
{
	if(pupaTimer == 0)
	{
		getMutableAnimal()->setNewLifeStage(LifeStage::ACTIVE);
	}
	else
	{
		pupaTimer--;
	}
}

void AnimalNonStatisticalGrowth::isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(const double& relativeHumidity)
{
	if(relativeHumidity >= getAnimal()->getSpecies()->getMinRelativeHumidityThreshold())
	{
		if(pupaTimer > 0)
		{
			getMutableAnimal()->setNewLifeStage(LifeStage::PUPA);
		}
		else
		{
			getMutableAnimal()->setNewLifeStage(LifeStage::ACTIVE);
		}
	}
	else
	{
		diapauseTimeSteps++;
	}
}

const int AnimalNonStatisticalGrowth::getDiapauseTimeSteps() const
{
    return diapauseTimeSteps;
}

const double& AnimalNonStatisticalGrowth::getLongevity() const
{
    return longevity;
}

void AnimalNonStatisticalGrowth::calculateGrowthCurves()
{
	AnimalGrowth::calculateGrowthCurves();

	calculateLongevity();

	calculateTotalReproMass();
}

void AnimalNonStatisticalGrowth::forceMolting()
{
	ofstream noStream;

	double ageAtInitialization;

	if(getAnimal()->getInstar() == 1)
	{
		ageAtInitialization = Random::randomUniform(0.0, getInstarAge(Instar(2)));
	}
	else if((getAnimal()->getInstar() > 1) && (getAnimal()->getInstar() < getAnimal()->getSpecies()->getNumberOfInstars()))
	{
		ageAtInitialization = Random::randomUniform(getInstarAge(getAnimal()->getInstar()), getInstarAge(getAnimal()->getInstar()+1));
	}
	else
	{
		ageAtInitialization = Random::randomUniform(getInstarAge(getAnimal()->getInstar()), getLongevity());
	}


	if(getAnimal()->getInstar() > 1)
	{
		for(Instar actualInstar = Instar(2); actualInstar <= getAnimal()->getInstar(); ++actualInstar)
		{
			actualMoltingTimeVector.push_back(getInstarAge(actualInstar));
			actualMoltingMassVector.push_back(getInstarMass(actualInstar));
		}
	}


	getMutableAnimal()->setCurrentAge(ageAtInitialization);


	//La edad del animal en negativo para que los calculos de tuneTraits sean correctos
	setDateEgg(-ageAtInitialization - getAnimal()->getBaseTraitValue(Trait::Type::eggDevTime));
	getMutableAnimal()->setBaseTraitElement(Trait::Type::eggDevTime, TraitDefinitionSection::Elements::TraitValue, -ageAtInitialization);


	getMutableAnimal()->setAgeOfLastMoultOrReproduction((getAnimal()->getInstar() > 1) ? getInstarAge(getAnimal()->getInstar()) : 0);

	if(getAnimal()->getInstar() >= getAnimal()->getSpecies()->getGrowthModule()->getInstarFirstMaturation())
	{
		getMutableAnimal()->setMatureValue(true);
	}

	
	double expectedLengthAtCurrentAge = getIndividualGrowthModel()->calculateLength(ageAtInitialization, getAnimal()->getFinalTraitValue(Trait::Type::growth));
	double expectedMassAtCurrentAge = getAnimal()->getSpecies()->calculateDryLength(expectedLengthAtCurrentAge, getAnimal()->isMature());

	setDryMass(expectedMassAtCurrentAge);


	if(getAnimal()->isMature())
	{
		//TODO ADD HERE ADJUSTMENTS FOR ENERGY TANK WHEN FORCING ADULTS!!
		//Dinosaurs double currentAge = -traits[Trait::eggDevTime] + 1.0/timeStepsPerDay;
		setLastMassBeforeMaturationOrOviposition(getInstarMass(getAnimal()->getInstar()));

		if(getAnimal()->getGender() == AnimalSpecies::Gender::FEMALE)
		{
			double clutchDryMass;

			if(getAnimal()->getSpecies()->getGrowthModule()->hasIndeterminateGrowth())
			{
				double fractFirstRepro = getInstarMass(getAnimal()->getSpecies()->getGrowthModule()->getInstarFirstReproduction())/getTotalReproMass();

				double currentEggs = fractFirstRepro*getAnimal()->getSpecies()->getTotFec();

				clutchDryMass = currentEggs*getEggDryMassAtBirth();
			}
			else
			{
				clutchDryMass = calculateClutchDryMass(getLastMassBeforeMaturationOrOviposition());
			}

			setMassForNextReproduction(getAnimal()->calculateDryMass() + clutchDryMass);
			

			
			double timeOfReproEvent;

			if(getAnimal()->hasCapitalBreeding())
			{
				// because the first one is on the pupaPerdiodLength
				double totalTimeBreedingCapitally = getAnimal()->getSpecies()->getTimeOfReproEventDuringCapitalBreeding()*(getAnimal()->getSpecies()->getNumberOfCapitalBreeds()-1);
				timeOfReproEvent = ((getLongevity() - totalTimeBreedingCapitally - getAgeOfFirstMaturation()) / (getAnimal()->getSpecies()->getFemaleMaxReproductionEvents() - getAnimal()->getSpecies()->getNumberOfCapitalBreeds())) - getAnimal()->getFinalTraitValue(Trait::Type::timeAddedToMeetLastRepro);
			}
			else
			{
				timeOfReproEvent = ((getLongevity() - getAgeOfFirstMaturation()) / getAnimal()->getSpecies()->getFemaleMaxReproductionEvents()) - getAnimal()->getFinalTraitValue(Trait::Type::timeAddedToMeetLastRepro);
			}

			getMutableAnimal()->setAgeForNextReproduction(ageAtInitialization + Random::randomUniform(0.0, timeOfReproEvent));
		}
		else
		{
			double currentLength = getIndividualGrowthModel()->calculateLength(getAnimal()->getCurrentAge(), getAnimal()->getFinalTraitValue(Trait::Type::growth));
			double currentMass = getAnimal()->getSpecies()->calculateDryMass(currentLength, getAnimal()->isMature());

			double nextReproLength = getIndividualGrowthModel()->calculateLength(getAnimal()->getAgeForNextReproduction(), getAnimal()->getFinalTraitValue(Trait::Type::growth));
			double nextReproMass = getAnimal()->getSpecies()->calculateDryMass(nextReproLength, getAnimal()->isMature());

			if(getAnimal()->getSpecies()->getGrowthModule()->hasIndeterminateGrowth())
			{
				if(getAnimal()->getInstar() < getAnimal()->getSpecies()->getNumberOfInstars())
				{
					setMassForNextReproduction(nextReproMass);
				}
				else
				{
					setMassForNextReproduction(currentMass);
				}
			}
			else
			{
				setMassForNextReproduction(getAnimal()->calculateDryMass() + (nextReproMass-currentMass));
			}
		}

		getMutableAnimal()->setAgeOfLastMoultOrReproduction(ageAtInitialization);


		ofstream noStream2;
	}
	else
	{
		double daysForPseudoTargetReproduction;
		daysForPseudoTargetReproduction = 1.1*getAnimal()->getCurrentAge(); //1.1 because reproTimeFactor is no longer in use
		getMutableAnimal()->setAgeForNextReproduction(getAnimal()->getCurrentAge() + daysForPseudoTargetReproduction);


		double nextReproLength = getIndividualGrowthModel()->calculateLength(getAnimal()->getAgeForNextReproduction(), getAnimal()->getFinalTraitValue(Trait::Type::growth));
		double nextReproMass = getAnimal()->getSpecies()->calculateDryMass(nextReproLength, true);

		setMassForNextReproduction(nextReproMass);
	}
}

const double& AnimalNonStatisticalGrowth::getMassForNextReproduction() const
{
	return massForNextReproduction;
}

void AnimalNonStatisticalGrowth::setMassForNextReproduction(const double& newMassForNextReproduction)
{
	massForNextReproduction = newMassForNextReproduction;
}

const double& AnimalNonStatisticalGrowth::getLastMassBeforeMaturationOrOviposition() const
{
	return lastMassBeforeMaturationOrOviposition;
}

void AnimalNonStatisticalGrowth::setLastMassBeforeMaturationOrOviposition(const double& newLastMassBeforeMaturationOrOviposition)
{
	lastMassBeforeMaturationOrOviposition = newLastMassBeforeMaturationOrOviposition;
}

const unsigned int AnimalNonStatisticalGrowth::computeEggBatchNumber() const
{
	return floor(clutchDryMass / (getAnimal()->getSpecies()->getGrowthModule()->getEggDryMass() + getAnimal()->getSpecies()->getGrowthModule()->getEggDryMass()*getAnimal()->getFinalTraitValue(Trait::Type::factorEggMass)));
}

const double& AnimalNonStatisticalGrowth::getTankAtGrowth() const
{
	return tankAtGrowth;
}

void AnimalNonStatisticalGrowth::setDateEgg(double newDateEgg) 
{ 
	date_egg = newDateEgg; 
}

const int& AnimalNonStatisticalGrowth::getDateEgg() const
{
	return date_egg;
}

const double& AnimalNonStatisticalGrowth::getTotalReproMass() const
{
	return totalReproMass;
}
