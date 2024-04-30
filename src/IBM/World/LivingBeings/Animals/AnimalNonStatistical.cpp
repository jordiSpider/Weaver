	/*
 * Animal.cpp
 *
 *  Created on: 20 Nov 2012
 *      Author: gabi
 */

#include "IBM/World/LivingBeings/Animals/AnimalNonStatistical.h"

#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"
#include "IBM/World/Map/Map.h"
#include "IBM/World/World.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCellInterface.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCellInterface.h"
#include "IBM/World/Map/TerrainCells/EdibleSearchParams.h"

using namespace std;
namespace bg = boost::geometry;




AnimalNonStatistical::AnimalNonStatistical(double actualTime, const Instar &instar, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface) 
	: Animal(mySpecies, terrainCellInterface, instar, LifeStage::ACTIVE), huntingMode(getSpecies()->getDefaultHuntingMode()), 
	  inHabitatShift(false), inBreedingZone(false), inHabitatShiftBeforeBreeding(false), inHabitatShiftAfterBreeding(false), atDestination(true), 
	  biomassExperiencedPerSpecies(AnimalSpecies::getSpeciesCounter(), 0.0),
	  meanExperiencesPerSpecies(AnimalSpecies::getSpeciesCounter(), 0.0)
{
	setOtherAttributes(g_numb_prt1, g_numb_prt2, ID_prt1, ID_prt2);

	animalGrowth = new AnimalNonStatisticalGrowth(this, getFinalTraitValue(Trait::Type::factorEggMass), actualTime);

	wetMassAtTheBeginningOfTheTimeStep = getAnimalGrowth()->getEggDryMassAtBirth()*getSpecies()->getConversionToWetMass();
}


AnimalNonStatistical::AnimalNonStatistical(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCellInterface* parentTerrainCellInterface, double newFactorEggMassFromMom, double actualTime, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender) 
	: Animal(mySpecies, parentTerrainCellInterface, Instar(), LifeStage::UNBORN, firstParentGamete, secondParentGamete, gender), huntingMode(getSpecies()->getDefaultHuntingMode()),
	  inHabitatShift(false), inBreedingZone(false), inHabitatShiftBeforeBreeding(false), inHabitatShiftAfterBreeding(false), atDestination(true), 
	  biomassExperiencedPerSpecies(AnimalSpecies::getSpeciesCounter(), 0.0),
	  meanExperiencesPerSpecies(AnimalSpecies::getSpeciesCounter(), 0.0)
{
	setOtherAttributes(g_numb_prt1, g_numb_prt2, ID_prt1, ID_prt2);

	animalGrowth = new AnimalNonStatisticalGrowth(this, newFactorEggMassFromMom, actualTime);
}


void AnimalNonStatistical::setOtherAttributes(int g_numb_prt1, int g_numb_prt2, int ID_prt1, int ID_prt2)
{
	updateLastTimeStep = UINT_MAX;

	mated = false;
	genomeFromMatedMale = NULL;
	idFromMatedMale = -1;
	setPredatedByID(-1);

	steps = 0.0;
	stepsAttempted = 0;
	oldSearchArea = 0; //this was created to keep the original search area before helping out of the cell
	foodMass = 0;
	finalJMaxVB = 0;
	sated = false;
	edibleToBePredatedProfitability = 1.0;
	setFoodMassLeftForNextTimeStep(0.0);
	lastHuntedAnimalDryMass = 1.0; //To avoid divisions by 0
	//encounters_prey = 0;
	todayEncountersWithPredators = 0;
	daysWithoutFood = 0.0;
	//total_prey_encs = 0;
	//totalEncountersWithPredators = 0;
    daysExperienced = 0;




	handlingTimer = 0.0;
	timeStepMaximumHandlingTimer = 1.0;
	setAgeOfLastMoultOrReproduction(-1);
	dateOfDeath = -1;
	lastDayMoved = -1;
	days_digest = 0;
	eatenToday = 0;
	
	this->generationNumberFromFemaleParent = g_numb_prt1;
	this->generationNumberFromMaleParent = g_numb_prt2;
	this->ID_prt1 = ID_prt1; //TODO puntero al padre mejor? o no? esto se usa?
	this->ID_prt2 = ID_prt2;
	reproCounter = 0;
	fecundity = 0;
	ageOfFirstReproduction = -1.0;

	capitalBreeding = getSpecies()->hasCapitalBreeding();
}


AnimalNonStatistical::~AnimalNonStatistical()
{
	if(genomeFromMatedMale != NULL)	delete genomeFromMatedMale;
}

void AnimalNonStatistical::increaseAge(int increase)
{
	setCurrentAge(getCurrentAge() + increase);
}

void AnimalNonStatistical::tune(const Temperature& temperature, const unsigned int numberOfTimeSteps, const double& relativeHumidity, ostream& tuneTraitsFile, bool printGrowthData)
{
	tuneTraits(temperature);


	double finalJMinVB = 0.0;
	double postTsearch = 0.0;		  
    double postTspeed = 0.0;
	double finalSearch = 0.0;
    double finalSpeed = 0.0;
	double finalVoracity = 0.0;


	double nextLengthPredicted;

	if(isMature())
	{
		if(!getSpecies()->getGrowthModule()->hasIndeterminateGrowth() && !hasCapitalBreeding() && getReproCounter() < getMaxReproductionEvents())
		{
			if(getAnimalGrowth()->getMassForNextReproduction() > calculateDryMass() && getAgeForNextReproduction() >= getCurrentAge())
			{
				// force currentAge-1 for calculations as to avoid nans
				double ageTarget = (getAgeForNextReproduction() > getCurrentAge()) ? getCurrentAge() : getCurrentAge()-1;

				double slopeTarget = (getAnimalGrowth()->getMassForNextReproduction() - calculateDryMass())/(getAgeForNextReproduction()-ageTarget);
				double interceptTarget = calculateDryMass()-slopeTarget*ageTarget;
				nextMassPredicted = interceptTarget + slopeTarget*(ageTarget+1);
			}
			else
			{
				// if the animal is already bigger - nextMassPredicted becomes 0
				if(getAnimalGrowth()->getMassForNextReproduction() <= calculateDryMass())
				{
					nextMassPredicted = calculateDryMass();
				}
				else if(getAnimalGrowth()->getMassForNextReproduction() > calculateDryMass() && getAgeForNextReproduction() < getCurrentAge())
				{
					// here we force a positive slope to allow animals to keep feeding until the target is met 
					double slopeTarget = (getAnimalGrowth()->getMassForNextReproduction()-calculateDryMass())/(getAgeForNextReproduction()-getCurrentAge());
					double interceptTarget = getAnimalGrowth()->getMassForNextReproduction()-slopeTarget*getAgeForNextReproduction();
					nextMassPredicted = interceptTarget + slopeTarget*getAgeForNextReproduction();
				}
			}
		}

		if(getSpecies()->getGrowthModule()->hasIndeterminateGrowth())
		{
			if(getAnimalGrowth()->getMassForNextReproduction() > calculateDryMass() && getAgeForNextReproduction() >= getCurrentAge() && getReproCounter() < getMaxReproductionEvents())
			{
				// force currentAge-1 for calculations as to avoid nans
				double ageTarget = (getAgeForNextReproduction() > getCurrentAge()) ? getCurrentAge() : getCurrentAge()-1;

				double slopeTarget = (getAnimalGrowth()->getMassForNextReproduction() - calculateDryMass())/(getAgeForNextReproduction()-ageTarget);
				double interceptTarget = calculateDryMass()-slopeTarget*ageTarget;
				nextMassPredicted = interceptTarget + slopeTarget*(ageTarget+1);
			}
			else
			{
				if(getAnimalGrowth()->getMassForNextReproduction() <= calculateDryMass() && getReproCounter() < getMaxReproductionEvents()-1)
				{
					nextMassPredicted = calculateDryMass();
				}
				else
				{
					double ageTarget = (getAnimalGrowth()->getMassForNextReproduction() > calculateDryMass()) ? getCurrentAge() : getCurrentAge()-1;

					// we swap currentAge to ageForNextReproduction
					double slopeTarget = (getAnimalGrowth()->getMassForNextReproduction()-calculateDryMass())/(ageTarget-getAgeForNextReproduction());
					double interceptTarget = calculateDryMass()-slopeTarget*getAgeForNextReproduction();
					nextMassPredicted = interceptTarget + slopeTarget*(ageTarget+1);
				}
			}
		}

		if(hasCapitalBreeding() && getReproCounter() < getMaxReproductionEvents())
		{
			if(getAgeForNextReproduction() >= getCurrentAge())
			{
				nextMassPredicted = calculateDryMass();
			}
			else
			{
				nextMassPredicted = calculateDryMass();
			}
		}

		if(getReproCounter() == getSpecies()->getNumberOfCapitalBreeds())
		{
			nextMassPredicted = calculateDryMass();
		}

		nextLengthPredicted = getSpecies()->calculateDryLength(nextMassPredicted, isMature());
	}
	else
	{
		nextLengthPredicted = getAnimalGrowth()->getIndividualGrowthModel()->calculateLength(getCurrentAge() + 1, getFinalTraitValue(Trait::Type::growth));
		nextMassPredicted = getSpecies()->calculateDryMass(nextLengthPredicted, isMature());
	}


	maxPostTsearch = getFinalTraitValue(Trait::Type::search_area) + getFinalTraitValue(Trait::Type::search_area) * getSpecies()->getPlasticityDueToConditionSearch();

	double proportionOfTimeTheAnimalWasMoving;
	if(lifeStage == LifeStage::SATIATED || lifeStage == LifeStage::HANDLING)
	{
		proportionOfTimeTheAnimalWasMoving = 0.0;
	}
	else
	{
		if(maxPostTsearch>0){
		    proportionOfTimeTheAnimalWasMoving = steps/maxPostTsearch;
		}else{
			proportionOfTimeTheAnimalWasMoving = 0.0;
		}
	}

	double minTotalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, proportionOfTimeTheAnimalWasMoving);
	
	double maxNextMassPredicted = getTerrainCellInterface()->getMap().getWorld()->calculateMaxMassPredicted(calculateDryMass(), calculateWetMass(calculateDryMass()), getSpecies()->getConversionToWetMass(), numberOfTimeSteps, hasCapitalBreeding(), minTotalMetabolicDryMassLoss, getSpecies()->getNewAAdult(), getSpecies()->getNewB());

	double targetMass = maxNextMassPredicted - calculateDryMass();

	if(isMature())
	{
		// this happens when nextMassPredicted = 0
		if(targetMass < 0.0)
		{
			if(getCurrentAge() > getAgeForNextReproduction())
			{
				targetMass = fabs(targetMass);
			}
			else
			{
				targetMass = 0.0;	
			}
		}
	}
	else
	{
		if(targetMass < 0.0)
		{
			targetMass = 0.0;
		}
	}


	if(targetMass > 0 && nextMassPredicted > 0.00000001) //when slope and interceptTarget = 0
	{
		// enhanced h
		double h = (1-(calculateDryMass()/nextMassPredicted))+getSpecies()->getH_Enhancement();
		
		if(h < 0){
			h = 0 ;
		}


		finalVoracity = applyPlasticityDueToConditionToTrait(getFinalTraitValue(Trait::Type::voracity), getSpecies()->getPlasticityDueToConditionVor(), h);
		finalVoracity = fmax(finalVoracity, 0.0);


		postTsearch = getFinalTraitValue(Trait::Type::search_area)*pow(calculateWetMass(calculateDryMass()), getSpecies()->getScaleForSearchArea());
		finalSearch = applyPlasticityDueToConditionToTrait(postTsearch, getSpecies()->getPlasticityDueToConditionSearch(), h);
		

		postTspeed = getTerrainCellInterface()->getMap().getWorld()->calculatePostTSpeed(
			getFinalTraitValue(Trait::Type::speed), calculateWetMass(calculateDryMass()),
			getSpecies()->getScaleForSpeed()
		);
		finalSpeed = applyPlasticityDueToConditionToTrait(postTspeed, getSpecies()->getPlasticityDueToConditionSpeed(), h);


		double vorAfterEncounters;
		double searchAfterEncounters;

		if(getTodayEncountersWithPredators() > 0)
		{
			double hEnc = 1-(getTodayEncountersWithPredators()/getSpecies()->getMaxEncountersPerDay());
		
			if(hEnc < 0)
			{
				hEnc = 0;
			}

			vorAfterEncounters = finalVoracity*(1-exp(-getSpecies()->getActivityUnderPredationRisk()*hEnc));
			searchAfterEncounters = finalSearch*(1-exp(-getSpecies()->getActivityUnderPredationRisk()*hEnc));
		}
		else
		{
			vorAfterEncounters = finalVoracity;
			searchAfterEncounters = finalSearch;
		}


		setFinalTraitValue(Trait::Type::voracity, vorAfterEncounters);

		setFinalTraitValue(Trait::Type::search_area, searchAfterEncounters);

		setFinalTraitValue(Trait::Type::speed, finalSpeed);
	}


	timeStepMaximumHandlingTimer = 1.0;
	timeStepMaximumSearchArea = getFinalTraitValue(Trait::Type::search_area);


	if(getCurrentEnergyTank() <= numeric_limits<double>::epsilon()) //Dinosaurs calculateDryMass() < minMassAtCurrentAge
	{
		if(getSpecies()->isSurviveWithoutFood()) {
			modifyEnergyTank(0.1);
		}
		else {
			setNewLifeStage(LifeStage::STARVED, numberOfTimeSteps);
		}
	}


	if(isMature() && getGender() == AnimalSpecies::Gender::MALE)
	{
		setFinalTraitValue(Trait::Type::search_area, getFinalTraitValue(Trait::Type::search_area) * getSpecies()->getMaleMobility());
	}


	//below it means that metabolic downregulation only exists for predators, such as spiders
	if(getHuntingMode() != HuntingMode::does_not_hunt && daysWithoutFood >= getSpecies()->getDaysWithoutFoodForMetabolicDownregulation())
	{
		setFinalTraitValue(Trait::Type::voracity, getFinalTraitValue(Trait::Type::voracity) - getSpecies()->getPercentageCostForMetabolicDownregulationVoracity() * getFinalTraitValue(Trait::Type::voracity));
		setFinalTraitValue(Trait::Type::search_area, getFinalTraitValue(Trait::Type::search_area) - getSpecies()->getPercentageCostForMetabolicDownregulationSearchArea() * getFinalTraitValue(Trait::Type::search_area));
		setFinalTraitValue(Trait::Type::speed, getFinalTraitValue(Trait::Type::speed) - getSpecies()->getPercentageCostForMetabolicDownregulationSpeed() * getFinalTraitValue(Trait::Type::speed));
	}


	//In this version the shock_resistance trait involves Maximum Critical Temperature (CTmax)
	if(getFinalTraitValue(Trait::Type::shock_resistance) < temperature.getTemperatureKelvin())
	{
		setNewLifeStage(LifeStage::SHOCKED, numberOfTimeSteps);
	}


	if(relativeHumidity < getSpecies()->getMinRelativeHumidityThreshold())
	{
		setNewLifeStage(LifeStage::DIAPAUSE);
	}


	if(printGrowthData)
	{
		tuneTraitsFile << this->getIdStr() << "\t"
		<< getSpecies()->getScientificName() << "\t"
		<< lifeStage << "\t"
		<< getCurrentAge() << "\t"
		<< getInstar() << "\t"
		<< getAnimalGrowth()->getCurrentBodySize() << "\t"
		<< getCurrentEnergyTank() << "\t"
		<< getAnimalGrowth()->getTankAtGrowth() << "\t" //done to document excessInvestment behavior
		<< calculateDryMass() << "\t"
		<< isMature() << "\t"
		<< finalJMinVB << "\t"
		<< finalJMaxVB << "\t"
		<< getBaseTraitValue(Trait::Type::voracity) << "\t"
		<< calculateDryMass()+finalJMinVB << "\t"
		<< calculateDryMass()+getFinalTraitValue(Trait::Type::voracity) << "\t"
		<< maxNextInstarMassFromVBPlasticity << "\t"
		<< calculateWetMass() << "\t"
		<< getBaseTraitValue(Trait::Type::search_area) << "\t"
		<< getBaseTraitValue(Trait::Type::speed) << "\t"
		<< postTsearch << "\t"
		<< postTspeed << "\t"
		<< minTotalMetabolicDryMassLoss << "\t"
		<< finalSearch << "\t"
		<< finalSpeed << "\t"
		<< finalVoracity << "\t"
		<< getFinalTraitValue(Trait::Type::voracity) << "\t"
		<< getFinalTraitValue(Trait::Type::search_area) << "\t"
		<< getFinalTraitValue(Trait::Type::speed) << "\t"
		<< getDateOfDeath() << "\t";
	}


	todayEncountersWithPredators = 0;


	getMutableAnimalGrowth()->tune(tuneTraitsFile, printGrowthData);


	//TODO PARA LOS CRECIMIENTO INDETERMINADO, SE DEJAN POR AHORA QUE SIGAN MOVIENDOSE INFINITO
	//TODO EN UN FUTURO SE HARÁ PARA LOS INDETERMINADO, DEJANDO QUE SIGAN CRECIENDO SI ALCANZAN EL TIEMPO DE LONGEVITY
	if(getCurrentAge() >= getAnimalGrowth()->getLongevity())
	{
		setNewLifeStage(LifeStage::SENESCED, numberOfTimeSteps);
	}


	if(printGrowthData)
	{
		tuneTraitsFile << endl;
	}


	//TODO-SEMI FALTA ASOCIAR WeightPerLocus tiene que ser un código el cual indique qué locus de qué rasgo tiene un valor específico.
	//Está sin utilizar ahora mismo. Será una lista de entradas de la forma: mirar linked_aca

	//Output: "id\tspecies\tstate\tcurrent_age\ttarget_instar_devtime\tinstar\ttarget_instar\tbody_size\tenergy_tank\tbody_mass\ttarget_mass\tmin_expected_body_mass\texpected_body_mass\tmax_expected_body_mass\tcondition_dependence\tmax_condition_dependence\tpreT_voracity\tpreT_search_area\tpreT_speed\tmin_postT_voracity\tmin_postT_search_area\tmin_postT_speed\tpostT_voracity\tpostT_search_area\tpostT_speed\tmax_postT_voracity\tmax_postT_search_area\tmax_postT_speed\tconditioned_voracity\tconditioned_search_area\tconditioned_speed" << endl;

	//TODO Diapausa cuando se pone el huevo. diapauseTimer = pheno. Solamente se disminuye diapauseTimer si las condiciones (temperatura y/o humedad) lo permiten, por debajo de un umbral.
	//TODO Los numeros de individuos por instar ahora van a ser DENSIDADES por instar. Que pasaran a ser numero de individuos dependiendo del área habitable.

	//TODO Eliminar el hongo cuando sea <= 0, y borrar minimumresourcecapacity y getZeroResource.
	//TODO Añadir un parametro que decida si el animal crece por mudas o continuo (dinosaurios)
}

const AnimalNonStatisticalGrowth* const AnimalNonStatistical::getAnimalGrowth() const
{
	return static_cast<const AnimalNonStatisticalGrowth* const>(animalGrowth);
}

AnimalNonStatisticalGrowth* const AnimalNonStatistical::getMutableAnimalGrowth()
{
	return static_cast<AnimalNonStatisticalGrowth* const>(animalGrowth);
}

const int AnimalNonStatistical::getTodayEncountersWithPredators() const
{
	return todayEncountersWithPredators;
}


double AnimalNonStatistical::getTotalMetabolicDryMassLoss(double wetMass, double proportionOfTimeTheAnimalWasMoving)
{
	double totalMetabolicDryMassLossPerDay = getTerrainCellInterface()->getMap().getWorld()->calculateTotalMetabolicDryMassLossPerDay(wetMass, proportionOfTimeTheAnimalWasMoving, this);

	double totalMetabolicDryMassLossPerTimeStep = totalMetabolicDryMassLossPerDay * getTerrainCellInterface()->getMap().getWorld()->getTimeStepsPerDay();

	return totalMetabolicDryMassLossPerTimeStep;
}

//TODO parametro para que se ejecuten cada X timesteps

void AnimalNonStatistical::metabolize()
{
	#ifdef DEBUG
		double lastEnergyTank = getCurrentEnergyTank();
	#endif

	//double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::eggDevTime] + 1.0/timeStepsPerDay;

	double proportionOfTimeTheAnimalWasMoving;
	if(lifeStage == LifeStage::SATIATED || lifeStage == LifeStage::HANDLING)
	{
		proportionOfTimeTheAnimalWasMoving = 0;
	}
	else
	{
		if(maxPostTsearch>0){
		    proportionOfTimeTheAnimalWasMoving = steps/maxPostTsearch;
		}else{
			proportionOfTimeTheAnimalWasMoving = 0;
		}
		
	
	}
	steps = 0.0;
	stepsAttempted = 0;

	double totalMetabolicDryMassLoss = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, proportionOfTimeTheAnimalWasMoving);

	//Downregulation only here, do not change this into getMetabolicDryMassLoss because it would alter the expected loss in tuneTraits
	if(getHuntingMode() != HuntingMode::does_not_hunt && daysWithoutFood >= getSpecies()->getDaysWithoutFoodForMetabolicDownregulation())
	{
		totalMetabolicDryMassLoss -= totalMetabolicDryMassLoss * getSpecies()->getPercentageMetabolicDownregulation();
	}

	modifyEnergyTank(getCurrentEnergyTank() - totalMetabolicDryMassLoss);

	#ifdef DEBUG
		double afterLossEnergyTank = getCurrentEnergyTank();
		if(afterLossEnergyTank >= lastEnergyTank)
		{
			cout << "The metabolic loss was 0 or positive:" << endl;
			cout << " - Animal: " << getId() << "(" << getSpecies()->getScientificName() << ")" << endl;
			cout << " - Last energy tank: " << lastEnergyTank << endl;
			cout << " - After loss energy tank: " << afterLossEnergyTank << endl;
		}
	#endif
}

double AnimalNonStatistical::getMeanExperience(const unsigned int speciesId) const
{
	return meanExperiencesPerSpecies.at(speciesId); 
}

bool AnimalNonStatistical::isActive()
{
	return lifeStage == LifeStage::ACTIVE;
}

/**
 * Activates the animal if it is ready. In order to activate, an animal needs
 * to be unborn and reach its day of phenology.
 * @param day the current day in the simulation
 */

void AnimalNonStatistical::isReadyToBeBorn(const unsigned int numberOfTimeSteps)
{
	const double timeStepsPerDay = getTerrainCellInterface()->getMap().getWorld()->getTimeStepsPerDay();

	double thisCurrentAge = ((numberOfTimeSteps-getAnimalGrowth()->getDiapauseTimeSteps())*timeStepsPerDay) - getFinalTraitValue(Trait::Type::eggDevTime) + 1.0*timeStepsPerDay;
	
	if(thisCurrentAge > 0.0)
	{
		setNewLifeStage(LifeStage::ACTIVE);
		setCurrentAge(round(thisCurrentAge));
		setAgeOfLastMoultOrReproduction(thisCurrentAge);
	}
}

void AnimalNonStatistical::isReadyToResumeFromHandlingOrDecreaseHandlingTimer()
{
	if (handlingTimer == 0)
	{
		setNewLifeStage(LifeStage::ACTIVE);
	}
	else
	{
		handlingTimer--;
	}
}


double AnimalNonStatistical::getInterpolatedHuntedVoracity(double huntedVoracity) const
{
	return MathFunctions::linearInterpolate01(huntedVoracity,getSpecies()->getMinVorHunted(),getSpecies()->getMaxVorHunted()); 
}


double AnimalNonStatistical::getInterpolatedHunterVoracity() const
{
	return MathFunctions::linearInterpolate01(getFinalTraitValue(Trait::Type::voracity),getSpecies()->getMinVorHunter(),getSpecies()->getMaxVorHunter()); 
	//return ((traits[Trait::Type::voracity] - getSpecies()->getMeanVoracityHunter()) / getSpecies()->getSdVoracityHunter());
}


double AnimalNonStatistical::getInterpolatedVoracityProduct(double huntedVoracity) const
{
	return MathFunctions::linearInterpolate01(getFinalTraitValue(Trait::Type::voracity)* huntedVoracity,getSpecies()->getMinVorXVor(),getSpecies()->getMaxVorXVor()); 
	/*return ((traits[Trait::Type::voracity] * huntedVoracity - getSpecies()->getMeanVoracityXVoracity())
			/ getSpecies()->getSdVoracityXVoracity());*/
}


double AnimalNonStatistical::getInterpolatedHuntedBodySize(double huntedBodySize) const
{
	return MathFunctions::linearInterpolate01(huntedBodySize,getSpecies()->getMinSizeHunted(),getSpecies()->getMaxSizeHunted()); 
}


double AnimalNonStatistical::getInterpolatedHunterBodySize() const
{
	return MathFunctions::linearInterpolate01(getAnimalGrowth()->getCurrentBodySize(),getSpecies()->getMinSizeHunter(),getSpecies()->getMaxSizeHunter()); 
}


double AnimalNonStatistical::getInterpolatedPDF(double probabilityDensityFunction) const
{
	return MathFunctions::linearInterpolate01(probabilityDensityFunction,getSpecies()->getMinProbabilityDensityFunction(),getSpecies()->getMaxProbabilityDensityFunction());
}


double AnimalNonStatistical::getInterpolatedSpeedRatio(double huntedSpeed) const
{

	return MathFunctions::linearInterpolate01(getFinalTraitValue(Trait::Type::speed)/huntedSpeed,getSpecies()->getMinSpeedRatio(),getSpecies()->getMaxSpeedRatio());
}


double calculateLogMassRatio(double hunterAnimalDryMass, double huntedAnimalDryMass)
{
	return log(hunterAnimalDryMass/huntedAnimalDryMass);
}


void AnimalNonStatistical::incrementEncountersWithPredator(const int &predatorId)
{
	encounterEvents.push_back(predatorId);//the ID of the predator that has encounter gets recorded in a vector
	todayEncountersWithPredators++;
	getMutableSpecies()->updateMaxEncountersPerDay(todayEncountersWithPredators);
}

const double& AnimalNonStatistical::getAgeOfFirstReproduction() const 
{ 
    return ageOfFirstReproduction; 
}

const unsigned int& AnimalNonStatistical::getReproCounter() const
{
	return reproCounter;
}

const int& AnimalNonStatistical::getFecundity() const
{
	return fecundity;
}

const int& AnimalNonStatistical::getGenerationNumberFromFemaleParent() const
{
	return generationNumberFromFemaleParent;
}

const int& AnimalNonStatistical::getGenerationNumberFromMaleParent() const
{
	return generationNumberFromMaleParent;
}

const int& AnimalNonStatistical::getID_prt1() const
{
	return ID_prt1;
}

const int& AnimalNonStatistical::getID_prt2() const
{
	return ID_prt2;
}

ostream& operator<<(std::ostream& os, const AnimalNonStatistical& animal)
{
	os << static_cast<const Edible&>(animal)
	<< animal.getSpecies()->getScientificName() << "\t"
	<< animal.getGender() << "\t";

	for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
	{
		os << getPositionAxisValue(animal.getPosition(), axis) << "\t";
	}

	os << animal.getLifeStage() << "\t"
	<< animal.getInstar() << "\t"
	<< animal.getBaseTraitValue(Trait::Type::eggDevTime) << "\t"
	<< animal.getAnimalGrowth()->getDateEgg() << "\t"
	<< animal.getAgeOfFirstReproduction() << "\t"
	<< animal.getReproCounter() << "\t"
	<< animal.getFecundity() << "\t"
	<< animal.getDateOfDeath() << "\t"
	<< animal.getGenerationNumberFromFemaleParent() << "\t"
	<< animal.getGenerationNumberFromMaleParent() << "\t"
	<< animal.getID_prt1() << "\t"
	<< animal.getID_prt2() << "\t"
	<< animal.getTodayEncountersWithPredators() << "\t"
	<< animal.getEncounterEvents()->size() << "\t"
	<< animal.getDaysDigest() << "\t"
	<< animal.getBaseTraitValue(Trait::Type::voracity) << "\t"
	<< animal.getBaseTraitValue(Trait::Type::search_area) << "\t"
	<< animal.getBaseTraitValue(Trait::Type::speed) << "\t"
	<< animal.getBaseTraitValue(Trait::Type::energy_tank) << "\t"
	<< animal.getBaseTraitValue(Trait::Type::eggDevTime) << "\t"
	<< animal.getAnimalGrowth()->getCurrentBodySize() << "\t"
	<< animal.calculateDryMass() << "\t";

	for (size_t i = 0; i < EnumClass<Trait::Type>::size(); ++i)
	{
		try
		{
			os << animal.getFinalTraitValue((Trait::Type)i) << "\t";
		}
		catch(const LineInfoException& e)
		{
			os << -1 << "\t";
		}
	}

	return os;
}


const double AnimalNonStatistical::getRemainingVoracity() const
{ 
	return fmax(getFinalTraitValue(Trait::Type::voracity) - foodMass, 0.0); 
}


void AnimalNonStatistical::printVoracities(ostream& voracitiesFile)
{
	//Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::eggDevTime] + 1.0/timeStepsPerDay;
	double prop_time_mov;

	if(lifeStage == LifeStage::SATIATED || lifeStage == LifeStage::HANDLING)
	{
		prop_time_mov = 0;
	}
	else
	{
		if(maxPostTsearch>0){
		  prop_time_mov = steps/maxPostTsearch;
		}else{
		  prop_time_mov = 0;	
		}
	}
	double totalMetabolicDryMassLossAfterAssim = getTotalMetabolicDryMassLoss(wetMassAtTheBeginningOfTheTimeStep, prop_time_mov);
	if(totalMetabolicDryMassLossAfterAssim)										
	if(getHuntingMode() != HuntingMode::does_not_hunt && daysWithoutFood >= getSpecies()->getDaysWithoutFoodForMetabolicDownregulation())
	{
		totalMetabolicDryMassLossAfterAssim -= totalMetabolicDryMassLossAfterAssim * getSpecies()->getPercentageMetabolicDownregulation();
	}



  	double lengthPredicted = 0;

	if(getInstar() > 1) {
		lengthPredicted = getAnimalGrowth()->getIndividualGrowthModel()->calculateLength(getCurrentAge(), getFinalTraitValue(Trait::Type::growth));
	}
	else {
		lengthPredicted = getAnimalGrowth()->getIndividualGrowthModel()->calculateLength(0, getFinalTraitValue(Trait::Type::growth));
	}

	double massPredicted = getSpecies()->calculateDryMass(lengthPredicted, true);

    double minMassAtCurrentAge = massPredicted - massPredicted * getSpecies()->getMinPlasticityKVonBertalanffy();



	voracitiesFile << this->getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< lifeStage << "\t"
	<< getCurrentAge()-1 << "\t" //Dinosaurs - it has been updated in tuneTraits
	<< getInstar() << "\t"
	<< getAnimalGrowth()->getCurrentBodySize() << "\t"
	<< getCurrentEnergyTank() << "\t"
	<< calculateDryMass() << "\t"
	<< nextMassPredicted << "\t" //Dinosaures
	<< minMassAtCurrentAge << "\t"
	<< getFinalTraitValue(Trait::Type::voracity) << "\t"
	<< foodMass << "\t"
	<< calculateDryMass() + foodMass << "\t"
	<< totalMetabolicDryMassLossAfterAssim << "\t"
	<< eatenToday << "\t"
	<< steps << "\t"
	<< stepsAttempted << "\t"
	<< oldSearchArea << "\t"
	<< sated << "\t"
	<< ((timeStepMaximumSearchArea > 0) ? steps / timeStepMaximumSearchArea : 0) << "\t"
	<< handlingTimer << "\t"
	<< getFinalTraitValue(Trait::Type::voracity) / calculateDryMass() << "\t"
	<< getGender() << "\t"
	<< mated << "\t"
	<< getAnimalGrowth()->getEggDryMassAtBirth() << "\t"
	<< getFinalTraitValue(Trait::Type::growth) << "\t"
	<< getFinalTraitValue(Trait::Type::factorEggMass) << "\t"
	<< getDateOfDeath() << "\t"
	<< getAnimalGrowth()->getAgeOfFirstMaturation() << "\t"
	<< reproCounter << "\t";

	voracitiesFile << endl;
}

double AnimalNonStatistical::applyPlasticityDueToConditionToTrait(const double& traitValue, const double& plasticityDueToCondition, const double& h) const
{
	return traitValue*(1-exp(-plasticityDueToCondition*h));
}

void AnimalNonStatistical::printGenetics(ostream& geneticsFile)
{
	geneticsFile << this->getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< generationNumberFromFemaleParent << "\t"
	<< generationNumberFromMaleParent << "\t"
	<< ID_prt1 << "\t"
	<< ID_prt2 << "\t";

	for (unsigned int i = 0; i < getSpecies()->getNumberOfIndividualLevelTraits(); ++i)
	{
		geneticsFile << *(genome.getHomologousChromosomes().at(i).first) << *(genome.getHomologousChromosomes().at(i).second);
	}
	geneticsFile << endl;
}


void AnimalNonStatistical::printTraits(ostream& traitsFile)
{
	traitsFile << this->getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< generationNumberFromFemaleParent << "\t"
	<< generationNumberFromMaleParent << "\t"
	<< ID_prt1 << "\t"
	<< ID_prt2 << "\t";

	for (size_t i = 0; i < EnumClass<Trait::Type>::size(); ++i)
	{
		try
		{
			traitsFile << getFinalTraitValue((Trait::Type)i) << "\t";
		}
		catch(const LineInfoException& e)
		{
			traitsFile << -1 << "\t";
		}
	}
	traitsFile << endl;
}

void AnimalNonStatistical::setNewLifeStage(const LifeStage newLifeStage)
{
	switch (newLifeStage)
	{
	case LifeStage::REPRODUCING:
		if(getSpecies()->occursHabitatShiftBeforeBreeding())
		{
			setInHabitatShiftBeforeBreeding(true);
		}
		else
		{
			setInBreedingZone(true);
		}
		break;
	default:
		break;
	}


	getMutableTerrainCellInterface()->eraseAnimal(this);

	lifeStage = newLifeStage;
	getMutableTerrainCellInterface()->insertAnimal(this);
}


void AnimalNonStatistical::setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps)
{
	setNewLifeStage(newLifeStage);

	const double dayOfDeath = numberOfTimeSteps*getTerrainCellInterface()->getMap().getWorld()->getTimeStepsPerDay();

	switch (newLifeStage)
	{
	case LifeStage::STARVED:
		setDateOfDeath(dayOfDeath);
		break;
	case LifeStage::PREDATED:
		setDateOfDeath(dayOfDeath);
		break;
	case LifeStage::BACKGROUND:
		setDateOfDeath(dayOfDeath);
		break;
	case LifeStage::SENESCED:
		setDateOfDeath(dayOfDeath);
		break;
	case LifeStage::SHOCKED:
		setDateOfDeath(dayOfDeath);
		break;
	default:
		throwLineInfoException("In the new state the animal does not die");
		break;
	}
}


void AnimalNonStatistical::setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps, int predatorId)
{
	setNewLifeStage(newLifeStage, numberOfTimeSteps);

	switch (newLifeStage)
	{
	case LifeStage::PREDATED:
		setPredatedByID(predatorId);
		break;
	default:
		throwLineInfoException("In the new state the animal is not depredated");
		break;
	}
}

void AnimalNonStatistical::setAgeForNextReproduction(const double& newAgeForNextReproduction)
{
	ageForNextReproduction = newAgeForNextReproduction;
}

void AnimalNonStatistical::setInstar(const Instar& newInstar)
{
	getMutableTerrainCellInterface()->eraseAnimal(this);

	Edible::setInstar(newInstar);

	updateDepth();

	setInHabitatShift(getSpecies()->habitatShiftOccurs(getInstar()));

	getMutableTerrainCellInterface()->insertAnimal(this);
}

void AnimalNonStatistical::moveOnNextInstar()
{
	Instar actualInstar = getInstar();
	actualInstar.moveOnNextInstar();
	setInstar(actualInstar);
}

void AnimalNonStatistical::setInHabitatShift(const bool newInHabitatShiftValue)
{
	inHabitatShift = newInHabitatShiftValue;
}

void AnimalNonStatistical::setInBreedingZone(const bool newInBreedingZoneValue)
{
	inBreedingZone = newInBreedingZoneValue;
}

void AnimalNonStatistical::setInHabitatShiftBeforeBreeding(const bool newInHabitatShiftBeforeBreedingValue)
{
	inHabitatShiftBeforeBreeding = newInHabitatShiftBeforeBreedingValue;
}

void AnimalNonStatistical::setInHabitatShiftAfterBreeding(const bool newInHabitatShiftAfterBreedingValue)
{
	inHabitatShiftAfterBreeding = newInHabitatShiftAfterBreedingValue;
}

void AnimalNonStatistical::setAtDestination(const bool newAtDestinationValue)
{
	atDestination = newAtDestinationValue;
}

void AnimalNonStatistical::setTargetNeighborToTravelTo(const std::pair<PointMap, PointContinuous> newTargetNeighborToTravelTo)
{
	targetNeighborToTravelTo = newTargetNeighborToTravelTo;
}

const bool AnimalNonStatistical::isInHabitatShift() const
{
	return inHabitatShift;
}

const bool AnimalNonStatistical::isInBreedingZone() const
{
	return inBreedingZone;
}

const bool AnimalNonStatistical::isInHabitatShiftBeforeBreeding() const
{
	return inHabitatShiftBeforeBreeding;
}

const bool AnimalNonStatistical::isInHabitatShiftAfterBreeding() const
{
	return inHabitatShiftAfterBreeding;
}

const bool AnimalNonStatistical::isAtDestination() const
{
	return atDestination;
}

const std::pair<PointMap, PointContinuous>& AnimalNonStatistical::getTargetNeighborToTravelTo() const
{
	return targetNeighborToTravelTo;
}

int AnimalNonStatistical::getPredatedByID() const 
{ 
	return predatedByID; 
}

void AnimalNonStatistical::setPredatedByID(int predatorId) 
{ 
	this->predatedByID = predatorId; 
}

void AnimalNonStatistical::updateDepth()
{
	TerrainCellInterface* terrainCellToMigrate = getMutableTerrainCellInterface();

	while(static_cast<const PointSpatialTree &>(terrainCellToMigrate->getPosition()).getDepth() != getCellDepthOnActualInstar())
	{
		TerrainCellInterface* newTerrainCellToMigrate = static_cast<SpatialTreeTerrainCellInterface *const>(terrainCellToMigrate)->getMutableParent();
	
		if(newTerrainCellToMigrate == nullptr && getAnimalGrowth()->getIndividualGrowthModel()->calculateLength(getCurrentAge(), getFinalTraitValue(Trait::Type::growth)) > newTerrainCellToMigrate->getSize())
		{
			throwLineInfoException("The animal's dimensions are greater than those of the map");
		}
		
		terrainCellToMigrate = newTerrainCellToMigrate;
	}

	setTerrainCellInterface(terrainCellToMigrate);
}

const bool AnimalNonStatistical::hasCapitalBreeding() const
{
	return capitalBreeding;
}

void AnimalNonStatistical::disableCapitalBreeding()
{
	capitalBreeding = false;
}


void AnimalNonStatistical::moveAnimal(const unsigned int numberOfTimeSteps, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{


	#ifdef DEBUG
	//Output::coutFlush("moving animal... \n");
	#endif

	// This condition checks if the animal has already been moved in the current timeStep
	if (lastDayMoved < static_cast<int>(numberOfTimeSteps))
	{
		updateBiomassExperiencedPerSpecies();
		handlingTimer = 0.0;
		
		sated = false; //Dinosaurs and arthropods after Dinos - do not know why¿? 

		// Assimilate leftover food
		assimilateLastHuntedAnimalAndComputeHandlingTime();

		// Movement occurs when there is no food mass left
		if(!isThereLeftoverFood()) 
		{
			oldSearchArea = getFinalTraitValue(Trait::Type::search_area);

			double biomassTrackedToday = 0.0;
			list<const EdibleInterface*> ediblesHasTriedToPredate;

			bool animalIsSenesced = searchAnimalToBreed(numberOfTimeSteps);

			if(!animalIsSenesced) {
				searchAnimalsAndResourceToEat(numberOfTimeSteps, biomassTrackedToday, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
			}

			//Mature MALES will move now even when they are sated

			if(isInHabitatShift() && (isInHabitatShiftBeforeBreeding() || isInHabitatShiftAfterBreeding()))
			{
				setInHabitatShift(false);
			}

			while (handlingTimer < timeStepMaximumHandlingTimer && // No me queda claro para que sirve
					steps < getFinalTraitValue(Trait::Type::search_area) && // If it hasn't moved beyond its search area
					lifeStage == LifeStage::ACTIVE && // If it is active
					days_digest == 0 && // Esta variable no se está utilizando
					(
						!sated || // If it is not satisfied, it will have to look for food
						(sated && mature && getGender() == AnimalSpecies::Gender::MALE) // If it is sated, mature and a male, so it will look for a mate to breed with
					)
			)
			{
				double currentPerceptionArea = getFinalTraitValue(Trait::Type::perception_area);

				if(isInHabitatShift() || isInHabitatShiftBeforeBreeding() || isInHabitatShiftAfterBreeding())
				{
					if(isInHabitatShift())
					{
						currentPerceptionArea *= getSpecies()->getHabitatShiftFactor();
						setInHabitatShift(false);
					}
					else if(isInHabitatShiftBeforeBreeding())
					{
						currentPerceptionArea *= getSpecies()->getHabitatShiftBeforeBreedingFactor();
						setInHabitatShiftBeforeBreeding(false);
					}
					else if(isInHabitatShiftAfterBreeding())
					{
						currentPerceptionArea *= getSpecies()->getHabitatShiftAfterBreedingFactor();
						setInHabitatShiftAfterBreeding(false);
					}

					setAtDestination(true);
				}


				if(isAtDestination())
				{
					searchTargetToTravelTo(currentPerceptionArea, ediblesHasTriedToPredate, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
					setAtDestination(false);
				}

				moveOneStep();

				if(isAtDestination() && getLifeStage() == LifeStage::REPRODUCING)
				{
					setInBreedingZone(true);
				}
				
				animalIsSenesced = searchAnimalToBreed(numberOfTimeSteps);

				if(!animalIsSenesced) {
					searchAnimalsAndResourceToEat(numberOfTimeSteps, biomassTrackedToday, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
				}
			}

			// IMPORTANT! steps might be higher than search_area just when the animal travels the LAST STEP in a diagonal direction.
			// Set the final number of steps performed in the movement
			if(steps > getFinalTraitValue(Trait::Type::search_area))
			{
				steps = getFinalTraitValue(Trait::Type::search_area);
			}

			// Update the number of days the animal has been without food
			switch (getHuntingMode()) {
				case HuntingMode::does_not_hunt: {
					break;
				}
				case HuntingMode::sit_and_wait:
				case HuntingMode::active_hunting: {
					if(foodMass > 0.0)
					{
						daysWithoutFood = 0.0;
					}
					else
					{
						daysWithoutFood += getTerrainCellInterface()->getMap().getWorld()->getTimeStepsPerDay();
					}
					break;
				}
				default: {
					throwLineInfoException("Default case");
					break;
				}
			}
		}

		// It indicates that the animal has already moved in this timestep.
		lastDayMoved = numberOfTimeSteps;
	}
}

bool edibleCompare(const tuple<double, pair<Animal*, EdibleInterface*>, double>& firstElem, const tuple<double, pair<Animal*, EdibleInterface*>, double>& secondElem) {
  return get<0>(firstElem) > get<0>(secondElem);
}

bool AnimalNonStatistical::checkBreed(const unsigned int numberOfTimeSteps, AnimalNonStatistical &otherAnimal)
{
	// Variables to identify the male and female at mating
	AnimalNonStatistical *matedMale, *matedFemale;

	// Choose the role of each animal according to the sex of the animal doing the searching
	switch(getGender())
	{
		case AnimalSpecies::Gender::MALE:
			matedMale = this;
			matedFemale = &otherAnimal;
			break;
		case AnimalSpecies::Gender::FEMALE:
			matedMale = &otherAnimal;
			matedFemale = this;
			break;
		case AnimalSpecies::Gender::HERMAPHRODITE:
			matedMale = &otherAnimal;
			matedFemale = this;
			break;
		default:
			throwLineInfoException("Default case");
			break;
	}

	// The female retains the male genome
	matedFemale->setGenomeFromMatedMale(matedMale);

	// if the male has exceeded the maximum reproductive limit
	if(matedMale->postBreeding())
	{
		// Change the status of the male to senesced
		matedMale->getMutableTerrainCellInterface()->changeAnimalToSenesced(matedMale, numberOfTimeSteps);

		// If the male is the animal conducting the search, death of the animal is indicated
		if(matedMale == this)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

const double& AnimalNonStatistical::getAgeForNextReproduction() const
{
	return ageForNextReproduction;
}

bool AnimalNonStatistical::searchAnimalToBreed(const unsigned int numberOfTimeSteps)
{
	// If the animal is mature and has not mated, find another animal to mate with
	if(isMature() && !isMated())
	{
		unique_ptr<PartialCoverageAnimals> partialCoverageAnimals;
		unique_ptr<FullCoverageAnimals> fullCoverageAnimals;

		tie(partialCoverageAnimals, fullCoverageAnimals) = getMutableTerrainCellInterface()->getAnimalsOnRadius(
			getPosition(), getFinalTraitValue(Trait::Type::interaction_area), getSpecies()->getBreedSearchParams(getGender())
		);

		// Try to mate the animal with one of the possible animals
		for(auto &terrainCellAnimals : *fullCoverageAnimals)
		{
			for(auto &animalsVector : *terrainCellAnimals)
			{
				for(auto &animal : *animalsVector)
				{
					// If the mating conditions are met
					if(!static_cast<AnimalNonStatistical*>(animal)->isMated())
					{
						return checkBreed(numberOfTimeSteps, *static_cast<AnimalNonStatistical*>(animal));
					}
				}
			}
		}

		for(auto &terrainCellAnimals : *partialCoverageAnimals)
		{
			for(auto &animal : *terrainCellAnimals)
			{
				// If the mating conditions are met
				if(!static_cast<AnimalNonStatistical*>(animal)->isMated())
				{
					return checkBreed(numberOfTimeSteps, *static_cast<AnimalNonStatistical*>(animal));
				}
			}
		}
	}

	return false;
}

unsigned int AnimalNonStatistical::getUpdateLastTimeStep() const
{
	return updateLastTimeStep;
}

void AnimalNonStatistical::setUpdateLastTimeStep(const unsigned int newUpdateLastTimeStep)
{
	updateLastTimeStep = newUpdateLastTimeStep;
}

bool AnimalNonStatistical::isSated() const
{ 
	return sated; 
}

bool AnimalNonStatistical::isExhausted() const 
{ 
	return steps >= getFinalTraitValue(Trait::Type::search_area); 
}

pair<bool, bool> AnimalNonStatistical::checkAnimalInteraction(const AnimalNonStatistical &otherAnimal, list<const EdibleInterface*> &ediblesHasTriedToPredate, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	// Variables indicating the type of interaction between the two animals
	bool setIsPredated = false, setPreyingOn = false;
	bool isPredated, preyingOn;

	// Check if the animal qualifies as a predator
	if(isSated() || isExhausted())
	{
		setPreyingOn = true;
		preyingOn = false;
	}
	else
	{
		//Dinosaurs but elsewhere - to prevent unreasonable encounters and ingestion of very large prey
		double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/otherAnimal.calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	
		if(probabilityDensityFunction < pdfThreshold) //Dinosaurs but elsewhere
		{
			setPreyingOn = true;
			preyingOn = false;
		}
	}

	// Check if the selected animal qualifies as a predator
	if(otherAnimal.isSated() || otherAnimal.isExhausted())
	{
		setIsPredated = true;
		isPredated = false;
	}
	else
	{
		//Dinosaurs but elsewhere - to prevent unreasonable encounters and ingestion of very large prey
		double probabilityDensityFunction = exp(-0.5 * pow((log(otherAnimal.calculateWetMass()/calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	
		if(probabilityDensityFunction < pdfThreshold) //Dinosaurs but elsewhere
		{
			setIsPredated = true;
			isPredated = false;
		}
	}


	// Complete the interaction information if it is incomplete
	if(!setIsPredated || !setPreyingOn)
	{
		// Check if the animal is prey for the selected animal
		bool isPrey = otherAnimal.canEatEdible(this, ediblesHasTriedToPredate, calculateDryMass());
		// Check if the selected animal is prey for the animal
		bool isPredator = canEatEdible(&otherAnimal, ediblesHasTriedToPredate, otherAnimal.calculateDryMass());

		// If both without predators of the other
		if (isPrey && isPredator)
		{
			// Based on a probability, it is decided who will hunt whom
			if(otherAnimal.calculatePredationProbability(*this, calculateDryMass(), false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW) > calculatePredationProbability(otherAnimal, otherAnimal.calculateDryMass(), false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW))
			{
				isPredator = false;
			}
			else
			{
				isPrey = false;
			}
		}

		if(!setIsPredated)
		{
			// If the selected animal hunts the animal
			if(isPrey)
			{
				isPredated = true;
			}
			else
			{
				isPredated = false;
			}
		}

		if(!setPreyingOn)
		{
			// If the animal hunts the selected animal
			if(isPredator)
			{
				preyingOn = true;
			}
			else
			{
				preyingOn = false;
			}
		}
	}

	return make_pair(isPredated, preyingOn);
}

bool AnimalNonStatistical::searchAnimalsAndResourceToEat(const unsigned int numberOfTimeSteps, double &biomassTrackedToday, list<const EdibleInterface*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	vector<tuple<double, pair<AnimalNonStatistical*, EdibleInterface*>, double>> ediblesByEdibility;

	// Get all the animals that can interact within the perception area.
	auto searchableEdibles = getTerrainCellInterface()->getMutableMap().getEdiblesOnRadius(
		getMutableTerrainCellInterface(), getPosition(), getFinalTraitValue(Trait::Type::interaction_area), 
		getSpecies()->getEdibleSearchParams(getInstar())
	);

	// Identify all possible interactions between the animal and the other edibles in the perception radius
	auto searchableResources = move(searchableEdibles.first);
	auto searchableAnimals = move(searchableEdibles.second);

	auto partialCoverageAnimals = move(searchableAnimals.first);
	auto fullCoverageAnimals = move(searchableAnimals.second);

	for(auto &terrainCellAnimals : *fullCoverageAnimals)
	{
		for(auto &animalsVector : *terrainCellAnimals)
		{
			for(auto &animal : *animalsVector)
			{
				AnimalNonStatistical* animalCast = static_cast<AnimalNonStatistical*>(animal);

				if(animalCast != this)
				{
					bool isPredated, preyingOn;
					tie(isPredated, preyingOn) = checkAnimalInteraction(*animalCast, ediblesHasTriedToPredate, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);

					// Add interaction if possible
					if(isPredated)
					{
						ediblesByEdibility.push_back(tuple<double, pair<AnimalNonStatistical*, EdibleInterface*>, double>(animalCast->calculateEdibilityValue(*this, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(animalCast, this), calculateDryMass()));
					}
					else if(preyingOn)
					{
						ediblesByEdibility.push_back(tuple<double, pair<AnimalNonStatistical*, EdibleInterface*>, double>(calculateEdibilityValue(*animalCast, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(this, animalCast), animalCast->calculateDryMass()));
						biomassTrackedToday += animalCast->calculateDryMass();
					}
				}
			}
		}
	}

	for(auto &terrainCellAnimals : *partialCoverageAnimals)
	{
		for(auto &animal : *terrainCellAnimals)
		{
			AnimalNonStatistical* animalCast = static_cast<AnimalNonStatistical*>(animal);

			if(animalCast != this)
			{
				bool isPredated, preyingOn;
				tie(isPredated, preyingOn) = checkAnimalInteraction(*animalCast, ediblesHasTriedToPredate, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);

				// Add interaction if possible
				if(isPredated)
				{
					ediblesByEdibility.push_back(tuple<double, pair<AnimalNonStatistical*, EdibleInterface*>, double>(animalCast->calculateEdibilityValue(*this, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(animalCast, this), calculateDryMass()));
				}
				else if(preyingOn)
				{
					ediblesByEdibility.push_back(tuple<double, pair<AnimalNonStatistical*, EdibleInterface*>, double>(calculateEdibilityValue(*animalCast, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(this, animalCast), animalCast->calculateDryMass()));
					biomassTrackedToday += animalCast->calculateDryMass();
				}
			}
		}
	}

	if(!isSated() && !isExhausted())
	{
		for(auto& [resource, dryMass] : *searchableResources)
		{
			// If the selected resource is prey for the animal
			if(canEatEdible(resource, ediblesHasTriedToPredate, dryMass))
			{
				ediblesByEdibility.push_back(tuple<double, pair<AnimalNonStatistical*, EdibleInterface*>, double>(calculateEdibilityValue(*resource, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH), make_pair(this, resource), dryMass));
				biomassTrackedToday += dryMass;
			}
		}
	}
	


	// Sorting the elements by edibility
	std::sort(ediblesByEdibility.begin(), ediblesByEdibility.end(), edibleCompare);

	// Print interaction information
	for(auto ediblesIt = ediblesByEdibility.begin(); ediblesIt != ediblesByEdibility.end(); ediblesIt++)
	{
		edibilitiesFile
		<< numberOfTimeSteps << "\t"
		<< getId() << "\t"
		<< getSpecies()->getScientificName() << "\t"
		<< foodMass << "\t"
		<< get<1>(*ediblesIt).first->getId() << "\t"
		<< get<1>(*ediblesIt).first->getSpecies()->getScientificName() << "\t"
		<< get<1>(*ediblesIt).first->calculateDryMass() << "\t"
		<< get<1>(*ediblesIt).second->getId() << "\t"
		<< get<1>(*ediblesIt).second->getSpecies()->getScientificName() << "\t"
		<< get<2>(*ediblesIt) << "\t"
		<< get<1>(*ediblesIt).first->calculateEncounterProbability(*get<1>(*ediblesIt).second, get<1>(*ediblesIt).second->calculateDryMass(), muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH) << "\t"
		<< get<1>(*ediblesIt).first->calculatePredationProbability(*get<1>(*ediblesIt).second, get<1>(*ediblesIt).second->calculateDryMass(), false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW) << "\t"
		<< get<1>(*ediblesIt).first->getSpecies()->getEdiblePreference(get<1>(*ediblesIt).second->getSpecies()->getId(), get<1>(*ediblesIt).first->getInstar(), get<1>(*ediblesIt).second->getInstar()) << "\t"
		<< get<1>(*ediblesIt).first->getMeanExperience(get<1>(*ediblesIt).second->getSpecies()->getId()) << "\t"
		<< get<0>(*ediblesIt) << endl;
	}
	

	// Variable indicating foraging success or failure
	bool hasEatenOnce = false;

	unique_ptr<Ring> perceptionArea;

	if(!searchableResources->empty())
	{
		perceptionArea = Geometry::makeSphere(getPosition(), getFinalTraitValue(Trait::Type::interaction_area));
	}
	else
	{
		perceptionArea = nullptr;
	}

	// Search for the edible to eat
	for(auto ediblesIt = ediblesByEdibility.begin(); !hasEatenOnce && ediblesIt != ediblesByEdibility.end(); ediblesIt++)
	{
		auto huntingAnimal = get<1>(*ediblesIt).first;
		auto targetAnimal = get<1>(*ediblesIt).second;
		auto targetDryMass = get<2>(*ediblesIt);


		// Calculate the probability of interaction of the animal with the edible it interacts with.
		float attackOrExposedAttackProbability = (huntingAnimal == this) ? huntingAnimal->getSpecies()->getAttackProbability() : targetAnimal->getSpecies()->getExposedAttackProbability();
		
		// Check if the predator can find the prey
		bool searchWasSuccessful = huntingAnimal->encounterEdible(*targetAnimal, targetDryMass, attackOrExposedAttackProbability, encounterProbabilitiesFile, predationProbabilitiesFile, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
		
		if(searchWasSuccessful)
		{
			//Steps only goes up when at least the animal HAS TRIED TO HUNT the prey. It goes up regardless of the success
			//The steps penalization is assumed depending on the search area and the longest diagonal
			
			// Check if the predator can hunt the prey
			bool huntWasSuccessful = huntingAnimal->predateEdible(*targetAnimal, targetDryMass, perceptionArea.get(), numberOfTimeSteps, false, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea);
			
			if(huntWasSuccessful)
			{
				// Successful search is indicated
				hasEatenOnce = true;

				// If the prey is the animal conducting the search, death of the animal is indicated
				if(targetAnimal == this)
				{
					//The searcher was cleaned up from the animals to move vector in its origin cell.
					//Therefore we do not need to clean the searcher up again here (it does not exist in this position's animals to move vector)
					//Searcher died, so return true
					return true;
				}
			}
			else // Hunt was NOT successful
			{
				// Counter attack only takes place if the target animal was not the current moving animal.
				// Check if the prey can become the predator
				if(targetAnimal->canEatEdible(huntingAnimal, ediblesHasTriedToPredate, huntingAnimal->calculateDryMass()) && targetAnimal != this)
				{
					// Check if the new predator can hunt the old predator
					bool counterHuntWasSuccessful = targetAnimal->predateEdible(*huntingAnimal, huntingAnimal->calculateDryMass(), perceptionArea.get(), numberOfTimeSteps, true, ediblesHasTriedToPredate, encounterProbabilitiesFile, predationProbabilitiesFile, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea);
					
					if(counterHuntWasSuccessful)
					{
						// Successful search is indicated
						hasEatenOnce = true;

						// If the old predator is the animal conducting the search, death of the animal is indicated
						if(huntingAnimal == this)
						{
							//The searcher was cleaned up from the animals to move vector in its origin cell.
							//Therefore we do not need to clean the searcher up again here (it does not exist in this position's animals to move vector)
							//Searcher died, so return true
							return true;
						}
					}
				}
			}
		}
	}

	#ifdef DEBUG
		if(foodMass > getFinalTraitValue(Trait::Type::voracity))
		{
			cout << "The food mass eaten was higher than the voracity value:" << endl;
			cout << " - Animal: " << getId() << "(" << getSpecies()->getScientificName() << ")" << endl;
			cout << " - Food mass eaten: " << foodMass << endl;
			cout << " - Voracity value: " << getFinalTraitValue(Trait::Type::voracity) << endl;
		}
	#endif

	// If the food found covers the animal's voracity
	if(foodMass >= getFinalTraitValue(Trait::Type::voracity))
	{
		// The amount of food is truncated to the animal's current voracity.
		foodMass = getFinalTraitValue(Trait::Type::voracity);

		// It indicates that the animal is sated
		sated = true;
	}

	// Searcher did not die, so return false
	return false;
}


void AnimalNonStatistical::setGenomeFromMatedMale(const AnimalNonStatistical* matedMale)
{
	mated = true;
	if(genomeFromMatedMale != NULL)	delete genomeFromMatedMale;
	genomeFromMatedMale = matedMale->getGenome().clone(getSpecies()->getRandomlyCreatedPositionsForChromosomes());										   
	//genomeFromMatedMale = matedGenome->clone();
	idFromMatedMale = matedMale->getId();
	generationNumberFromMatedMale = matedMale->getGenerationNumberFromMaleParent();
}


void AnimalNonStatistical::clearGenomeFromMatedMale()
{
	mated = false;
	delete genomeFromMatedMale;
	genomeFromMatedMale = NULL;
	idFromMatedMale = -1;
	generationNumberFromMatedMale = -1;
}

void AnimalNonStatistical::doDefinitive() { 
	id = generateId();
	generateIdStr();
}

bool AnimalNonStatistical::postBreeding()
{
	reproCounter++;

	//Dinosaurs double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::eggDevTime] + 1.0/timeStepsPerDay;



	setAgeOfLastMoultOrReproduction(getCurrentAge());

	if (reproCounter == 1)
	{
		ageOfFirstReproduction = getCurrentAge();
	}
	if (reproCounter >= getMaxReproductionEvents())
	{

		//cout << "reproCounter: " << reproCounter << endl;
		//cout << "max rep: "<< getMaxReproductionEvents() << endl;
		//cout << getSpecies()->getScientificName() << endl;

		return true;

	}
	else
	{
		//this below is allow females with indeterminate growth to keep growing while reproducing
		//Dinos2023 - animals can now follow mature patterns, be mature while growing, does it work?
		//if we comment below indeterminate growth stops - thus, we need another formula to avoid excessive food targets
	 	if(getInstar() < getSpecies()->getNumberOfInstars())
		{
			//Mature will be false only if is NOT the last instar, otherwise the animal will reproduce again
			setMatureValue(false);
		}  
		//end Dinos2023
		return false;
	}
}

const int AnimalNonStatistical::getAgeOfLastMoultOrReproduction() const
{
	return ageOfLastMoultOrReproduction;
}

void AnimalNonStatistical::setAgeOfLastMoultOrReproduction(const int newAgeOfLastMoultOrReproduction)
{
	ageOfLastMoultOrReproduction = newAgeOfLastMoultOrReproduction;
}

unique_ptr<list<AnimalNonStatistical*>> AnimalNonStatistical::breedAsexually(int objectiveEggsNumber, const unsigned int numberOfTimeSteps, Temperature temperature, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
	double totalOffspringDryMass = 0.0;

	auto offspring = make_unique<list<AnimalNonStatistical*>>();

	//Asexual animals are always females (or treated as females) and they DO NOT perform meiosis.
	AnimalSpecies::Gender::GenderValue offSpringGender = AnimalSpecies::Gender::FEMALE;
	int createdEggsNumber = 0;
	int generationDam;
	int generationSire;

	/*if(nextOffspringToBeBorn != NULL)
	{
		// Update both position and date of egg, to allocate the new animal in the correct space and time
		nextOffspringToBeBorn->setPosition(position);
		nextOffspringToBeBorn->setDateEgg(round(((double)timeStep/(double)timeStepsPerDay)));
		offspring->push_back(nextOffspringToBeBorn);
		totalOffspringDryMass += nextOffspringToBeBorn->getEggDryMassAtBirth();
		nextOffspringToBeBorn = NULL;
	}*/

	while( createdEggsNumber < objectiveEggsNumber )
	{
		createdEggsNumber ++;
		generationDam = generationNumberFromFemaleParent + 1;
		generationSire = generationNumberFromFemaleParent + 1;

		Gamete* femaleGameteSelected = genome.cloneFirstGameteFromHaploid();
		Gamete* maleGameteSelected = genome.cloneFirstGameteFromHaploid();

		AnimalNonStatistical* newOffspring = createOffspring(femaleGameteSelected, maleGameteSelected, getMutableTerrainCellInterface(), getFinalTraitValue(Trait::Type::factorEggMass), numberOfTimeSteps*getTerrainCellInterface()->getMap().getWorld()->getTimeStepsPerDay(), generationNumberFromFemaleParent + 1, generationNumberFromMatedMale + 1, getId(), idFromMatedMale, getMutableSpecies(), offSpringGender);

		delete femaleGameteSelected;
		delete maleGameteSelected;

		//TODO ALWAYS print the traits after interpolating
		if(saveAnimalConstitutiveTraits) {
			newOffspring->printTraits(constitutiveTraitsFile);
		}

		totalOffspringDryMass += newOffspring->getAnimalGrowth()->getEggDryMassAtBirth();

		newOffspring->getMutableAnimalGrowth()->setDryMass(newOffspring->getAnimalGrowth()->getEggDryMassAtBirth());

		//There will be a number of falses in isViableOffspring when generating new offsprings
		//PLEASE NEVER CONSIDER AGAIN creating a new offSpring for each offspring we erased.
		//This erasing behaviour is good for evolution because it stops inviable phenotypes from growing in number
		
		offspring->push_back(newOffspring);
		newOffspring->doDefinitive();
		newOffspring->setExperiencePerSpecies();
	}

	
		
	fecundity += offspring->size();

	modifyEnergyTank(getCurrentEnergyTank() - totalOffspringDryMass);

	if(postBreeding())
	{
		getMutableTerrainCellInterface()->changeAnimalToSenesced(this, numberOfTimeSteps);
	}
	else
	{
		setNewLifeStage(LifeStage::ACTIVE);
		//clearGenomeFromMatedMale();
		//CARE delete only when in TerrainCell
		//position->insertAnimal(this);
		//position->eraseAnimal(this, REPRODUCING);
	}

	return offspring;
}


unique_ptr<list<AnimalNonStatistical*>> AnimalNonStatistical::breedSexually(int objectiveEggsNumber, const unsigned int numberOfTimeSteps, Temperature temperature, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
	double totalOffspringDryMass = 0.0;

	auto offspring = make_unique<list<AnimalNonStatistical*>>();

	AnimalSpecies::Gender::GenderValue offSpringGender = AnimalSpecies::Gender::FEMALE;
	int createdEggsNumber = 0;
	int generationDam;
	int generationSire;

	while( createdEggsNumber < objectiveEggsNumber )
	{
		createdEggsNumber ++;  // DUDA: Debe hacerse antes o sólo si se crea el offspring?
		
		Gamete* femaleGameteSelected = genome.cloneFirstGameteFromHaploid();
		Gamete* maleGameteSelected = NULL;
		
		if(getSpecies()->getSexualType() == SexualType::diploid)
		{
			maleGameteSelected = genomeFromMatedMale->getRandomGameteFromMeiosis();
			//gender here depends on the species sexRatio
			offSpringGender = getSpecies()->getRandomGender();
		}
		else if(getSpecies()->getSexualType() == SexualType::haplodiploid)
		{
			if(isMated())
			{
				//Gender here depends on the species sexRatio
				offSpringGender = getSpecies()->getRandomGender();
				if(offSpringGender == AnimalSpecies::Gender::MALE)
				{
					//Males are haploid so they will use only the female gamete (duplicated for simplicity, in reality they have only one)
					maleGameteSelected = new Gamete(getSpecies()->getNumberOfChromosomes());
					for(unsigned int i = 0; i < femaleGameteSelected->size(); ++i)
					{
						maleGameteSelected->pushChromosome(femaleGameteSelected->getChromosome(i)->clone());
					}
				}
				else
				{
					//Females are still diploid, but males contribute with their genes as they are (no meiosis involved here)
					maleGameteSelected = genomeFromMatedMale->cloneFirstGameteFromHaploid();
				}
			}
			else //!isMated()
			{
				//Males are haploid so they will use only the female gamete (duplicated for simplicity, in reality they have only one)
				maleGameteSelected = new Gamete(getSpecies()->getNumberOfChromosomes());
				for(unsigned int i = 0; i < femaleGameteSelected->size(); ++i)
				{
					maleGameteSelected->pushChromosome(femaleGameteSelected->getChromosome(i)->clone());
				}
				//Gender here is always MALE
				offSpringGender = AnimalSpecies::Gender::MALE;
			}
		}

		generationDam = generationNumberFromFemaleParent + 1; 
		generationSire = generationNumberFromMatedMale + 1;

		AnimalNonStatistical* newOffspring = createOffspring(femaleGameteSelected, maleGameteSelected, getMutableTerrainCellInterface(), getFinalTraitValue(Trait::Type::factorEggMass), numberOfTimeSteps*getTerrainCellInterface()->getMap().getWorld()->getTimeStepsPerDay(), generationNumberFromFemaleParent + 1, generationNumberFromMatedMale + 1, getId(), idFromMatedMale, getMutableSpecies(), offSpringGender);

		delete femaleGameteSelected;
		delete maleGameteSelected;

		//TODO ALWAYS print the traits after interpolating
		if(saveAnimalConstitutiveTraits) {
			newOffspring->printTraits(constitutiveTraitsFile);
		}

		totalOffspringDryMass += newOffspring->getAnimalGrowth()->getEggDryMassAtBirth();

		newOffspring->getMutableAnimalGrowth()->setDryMass(newOffspring->getAnimalGrowth()->getEggDryMassAtBirth());

		//There will be a number of falses in isViableOffspring when generating new offsprings
		//PLEASE NEVER CONSIDER AGAIN creating a new offSpring for each offspring we erased.
		//This erasing behaviour is good for evolution because it stops inviable phenotypes from growing in number
		
		offspring->push_back(newOffspring);
		newOffspring->doDefinitive();
		newOffspring->setExperiencePerSpecies();
	}

	fecundity += offspring->size();

	modifyEnergyTank(getCurrentEnergyTank() - totalOffspringDryMass);

	if(postBreeding())
	{
		getMutableTerrainCellInterface()->changeAnimalToSenesced(this, numberOfTimeSteps);
	}
	else
	{
		setNewLifeStage(LifeStage::ACTIVE);
		//clearGenomeFromMatedMale();
		//CARE delete only when in TerrainCell
		//position->insertAnimal(this);
		//position->eraseAnimal(this, REPRODUCING);
	}

	return offspring;
}


unique_ptr<list<AnimalNonStatistical*>> AnimalNonStatistical::breed(const unsigned int numberOfTimeSteps, Temperature temperature, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
	unsigned int currentEggBatchNumber = getAnimalGrowth()->computeEggBatchNumber();

	switch (getSpecies()->getSexualType())
	{
		case SexualType::asexual:
			return breedAsexually(currentEggBatchNumber, numberOfTimeSteps, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
			break;
		case SexualType::diploid:
			return breedSexually(currentEggBatchNumber, numberOfTimeSteps, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
			break;
		case SexualType::haplodiploid:
			return breedSexually(currentEggBatchNumber, numberOfTimeSteps, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
			break;
		default:
			throwLineInfoException("Default case");
			break;
	}

	if(getSpecies()->occursHabitatShiftAfterBreeding())
	{
		setInHabitatShiftAfterBreeding(true);
	}
}


void AnimalNonStatistical::dieFromBackground(const unsigned int numberOfTimeSteps)
{
	if(!getTerrainCellInterface()->getMap().getWorld()->isGrowthAndReproTest()) {
		if(getSpecies()->getProbabilityDeathFromBackground() > Random::randomUniform())
		{
			setNewLifeStage(LifeStage::BACKGROUND, numberOfTimeSteps);
		}
	}
}


void AnimalNonStatistical::assimilateFoodMass()
{
	if(foodMass > 0)
	{
		modifyEnergyTank(getCurrentEnergyTank() + foodMass);//*0.2; YA ESTÁ RESUELTO CUANDO COMEN!!

		
		if(foodMass < getFinalTraitValue(Trait::Type::voracity))
		{
			sated = false;
		}
		else
		{
			sated = true;
		}

		foodMass = 0;
		eatenToday = 0;
	}
}


void AnimalNonStatistical::becomePredated(int timeStep)
{
	setNewLifeStage(LifeStage::PREDATED, timeStep, getId());
}


int AnimalNonStatistical::getDaysDigest() const
{
	return days_digest;
}


const unsigned int AnimalNonStatistical::getMaxReproductionEvents() const
{
	if(getGender() == AnimalSpecies::Gender::FEMALE)
	{
		return getSpecies()->getFemaleMaxReproductionEvents();
	}
	else
	{
		return getSpecies()->getMaleMaxReproductionEvents();
	}
}


const list<int> * const AnimalNonStatistical::getEncounterEvents() const
{
	return &encounterEvents;
}


double AnimalNonStatistical::calculatePredationProbability(const EdibleInterface &edibleAnimal, const double &targetDryMass, bool retaliation, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW) const
{
	double predationProbability = 1.0;
	if(edibleAnimal.getSpecies()->isMobile())
	{
		double interpolatedHunterVoracity = getInterpolatedHunterVoracity();

		double interpolatedSpeedRatio;

		if(edibleAnimal.getSpeed()>0.0){
		  interpolatedSpeedRatio = getInterpolatedSpeedRatio(edibleAnimal.getSpeed());
	    }else{//this is to circunvent NANs - the ratio is made 1
	      interpolatedSpeedRatio = getInterpolatedSpeedRatio(getFinalTraitValue(Trait::Type::speed));
	    }

		double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/edibleAnimal.calculateWetMass(targetDryMass)) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
		double interpolatedPDF = getInterpolatedPDF(probabilityDensityFunction);

/* 		if(getSpecies()->getScientificName()=="Tyrannosaurus_sp"){
			cout << probabilityDensityFunction << endl;
			cout << normalizedPDF << endl;
		}
 */


		if (getHuntingMode() == HuntingMode::active_hunting || retaliation)
		{
			predationProbability = 
					(predationSpeedRatioAH * interpolatedSpeedRatio +
					predationHunterVoracityAH * interpolatedHunterVoracity +
					predationProbabilityDensityFunctionAH * interpolatedPDF)/3;

		}
		else if (getHuntingMode() == HuntingMode::sit_and_wait)
		{
			predationProbability = 
					(predationSpeedRatioSAW * interpolatedSpeedRatio + //##spd_ratio does not matter anymore
					predationHunterVoracitySAW * interpolatedHunterVoracity +
					predationProbabilityDensityFunctionSAW * interpolatedPDF)/3;
		}
	}

/* 		if(getSpecies()->getScientificName()=="Tyrannosaurus_sp"){
			cout << predationProbability << endl;
		} */



	return predationProbability;
}


bool AnimalNonStatistical::predateEdible(EdibleInterface &edibleToBePredated, const double &targetDryMass, const Ring* const perceptionArea, const unsigned int numberOfTimeSteps, bool retaliation, list<const EdibleInterface*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea)
{
	if(edibleToBePredated.getSpecies()->isMobile())
	{
		ediblesHasTriedToPredate.push_back(&edibleToBePredated);
	}
	bool huntWasSuccessful = false;
	//Steps only goes up when at least the animal HAS TRIED TO HUNT the prey. It goes up regardless of the success
	//The steps penalization is assumed depending on the search area and the longest diagonal

	if(!retaliation && edibleToBePredated.getSpecies()->isMobile())
	{
		double halfLongestDiagonalInsideCurrentCell = sqrt(3 * pow(this->getTerrainCellInterface()->getSize(), 2));
		double distanceToAdd = halfLongestDiagonalInsideCurrentCell*getFinalTraitValue(Trait::Type::search_area)/maxSearchArea;
		steps += distanceToAdd;
		if(timeStepMaximumSearchArea > 0.0)
		{
			timeStepMaximumHandlingTimer -= distanceToAdd / timeStepMaximumSearchArea;
		} else {
			timeStepMaximumHandlingTimer = 0.0;
		}
		timeStepMaximumHandlingTimer = max(timeStepMaximumHandlingTimer, 0.0);
		steps = fmin(steps, getFinalTraitValue(Trait::Type::search_area));
	}

	//arthros and for dinos!!!!
	double randomPredationProbability = Random::randomUniform();

	#ifdef DEBUG
	if(find(huntedAnimalsToday.begin(), huntedAnimalsToday.end(), edibleToBePredated.getId()) != huntedAnimalsToday.end())
	{
		//getTerrainCellInterface()->increaseMultipleSameHuntedAnimalToday();


		//cout << "The animal hunted off the same animal at least twice the same day: " << endl;
		//cout << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
		//cout << " - Animal hunted off: " << huntedAnimal->getId() << "(" << huntedAnimal->getSpecies()->getScientificName() << ")" << endl;
	}
	huntedAnimalsToday.push_back(edibleToBePredated.getId());
	#endif
	//if (predationProbability > randomPredationProbability)
	//TODO attackProb y killProb van EN CADA ESPECIE

//getSpecies()->getKillProbability() > randomPredationProbability

    double prob = calculatePredationProbability(edibleToBePredated, targetDryMass, retaliation, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW);//Dinosaur
//prob*
	if (((getSpecies()->getKillProbability() > randomPredationProbability) && edibleToBePredated.getSpecies()->isMobile()) || !edibleToBePredated.getSpecies()->isMobile()) ///arthropods + Dinosaur //
	{
		#ifdef DEBUG
		if(find(predatedAnimalsToday.begin(), predatedAnimalsToday.end(), edibleToBePredated.getId()) != predatedAnimalsToday.end())
		{
			//getTerrainCellInterface()->increaseMultipleSamePredatedAnimalToday();


			//cerr << "The animal predated the same animal at least twice the same day: " << endl;
			//cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
			//cerr << " - Animal predated: " << huntedAnimal->getId() << "(" << huntedAnimal->getSpecies()->getScientificName() << ")" << endl;
		}
		predatedAnimalsToday.push_back(edibleToBePredated.getId());
		#endif

		getMutableSpecies()->addPredationEventOnOtherSpecies(edibleToBePredated.getSpecies()->getId());
		eatenToday++;
		huntWasSuccessful = true;

		edibleToBePredatedProfitability = getMutableSpecies()->getEdibleProfitability(edibleToBePredated.getSpecies()->getId(), getInstar(), edibleToBePredated.getInstar());

		//arthro and for dinos
	    double forNotToDepleteResource = getFinalTraitValue(Trait::Type::voracity)/(edibleToBePredatedProfitability+getFinalTraitValue(Trait::Type::assim));
		double leftovers;

        if(edibleToBePredated.isDepleted(forNotToDepleteResource, targetDryMass)){ //resource leftover to partially fulfill voracity
	      leftovers = edibleToBePredated.anyLeft(targetDryMass)*(edibleToBePredatedProfitability+getFinalTraitValue(Trait::Type::assim));
        }else{ //the entire voracity is available
		  leftovers = forNotToDepleteResource;
		}  

		//The target animal is cleaned up from its source lifestage vector. Or the resource is substracted.
		double fullDryMassToBeEaten = getMutableTerrainCellInterface()->turnEdibleIntoDryMassToBeEaten(edibleToBePredated, targetDryMass, perceptionArea, numberOfTimeSteps, this, leftovers);
		//end arthro
		

		//double profitableDryMassToBeEaten = fullDryMassToBeEaten * edibleToBePredatedProfitability;
		biomassExperiencedPerSpecies[edibleToBePredated.getSpecies()->getId()] += fullDryMassToBeEaten;
		
		//Assim with handling time will be calculated according to the FULL dry mass
		//It will be reduced to only profitableMass after handling calculations
		setFoodMassLeftForNextTimeStep(fullDryMassToBeEaten);
		lastHuntedAnimalDryMass = fullDryMassToBeEaten;
		assimilateLastHuntedAnimalAndComputeHandlingTime();
	}

	predationProbabilitiesFile << this->getIdStr() << "\t"
	<< edibleToBePredated.getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< edibleToBePredated.getSpecies()->getScientificName() << "\t"
	<< ((edibleToBePredated.isHunting()) ? 1 : 0) << "\t"
	<< calculateDryMass() << "\t"
	<< targetDryMass << "\t"
	<< randomPredationProbability << "\t"
	<< prob << "\t"
	<< ((prob*getSpecies()->getKillProbability() > randomPredationProbability) ? 1 : 0) << "\t"
	<< endl;


	return huntWasSuccessful;
}


void AnimalNonStatistical::setExperiencePerSpecies()
{
	/*
		Se debe llamar a esta función después de cada forceMolting o después de la 
		creación de los animales en el breed. Ya que debe estar definido el atributo "instar"
		para fijar la experiencia de cada especie
	*/

	for(const auto &[edibleResourceSpecies, edibleInstars] : getSpecies()->getInstarEdibleResourceSpecies(getInstar()))
	{
		double preference = getSpecies()->getEdiblePreference(edibleResourceSpecies->getId(), getInstar());
		
		biomassExperiencedPerSpecies[edibleResourceSpecies->getId()] = preference;
		meanExperiencesPerSpecies[edibleResourceSpecies->getId()] = preference;
	}

	for(const auto &[edibleAnimalSpecies, edibleInstars] : getSpecies()->getInstarEdibleAnimalSpecies(getInstar()))
	{
		double preference = getSpecies()->getEdiblePreference(edibleAnimalSpecies->getId(), getInstar());

		biomassExperiencedPerSpecies[edibleAnimalSpecies->getId()] = preference;
		meanExperiencesPerSpecies[edibleAnimalSpecies->getId()] = preference;
	}
}


void AnimalNonStatistical::updateBiomassExperiencedPerSpecies()
{
	float totalExperiencedBiomass = 0;
	daysExperienced++;

	for(const auto &biomassExperienced : biomassExperiencedPerSpecies)
	{
		totalExperiencedBiomass += biomassExperienced;
	}

	if(totalExperiencedBiomass > 0.0)
	{
		for(auto &biomassExperienced : biomassExperiencedPerSpecies)
		{
			biomassExperienced /= totalExperiencedBiomass;
		}
	}

	double experienceInfluencePerTimeStep = getSpecies()->getExperienceInfluencePerDay() * getTerrainCellInterface()->getMap().getWorld()->getTimeStepsPerDay();

	/*
		To update the experience of a species, a weighted average is performed, where the variable 'experienceInfluencePerTimeStep' 
		determines whether recent experience or past experience is more important. If the value 'experienceInfluencePerTimeStep' is 
		close to 1, recent experience is more important.
	*/
	for(unsigned int i = 0; i < meanExperiencesPerSpecies.size(); i++)
	{
		//1.0 will be the base value, meaning it has not come across any animal yet
		meanExperiencesPerSpecies[i] = biomassExperiencedPerSpecies[i] * experienceInfluencePerTimeStep + meanExperiencesPerSpecies[i] * (1.0 - experienceInfluencePerTimeStep);
		//cout << (*it).second << endl;
	}

	//Jordi - 23/03/2022 - this "biomassExperiencedPerSpecies" needs to turn it back to the original biomass scale, otherwise only the last food item matters added to a 0-1 value
	if(totalExperiencedBiomass > 0.0)
	{
		for(auto &biomassExperienced : biomassExperiencedPerSpecies)
		{
			if(daysExperienced <= static_cast<unsigned int>(getFinalTraitValue(Trait::Type::memoryDepth))){ //this 5 needs to be taken out to the Json file - was already declared somwhere bu Gabi!!!
				biomassExperienced *= totalExperiencedBiomass; //however, to do this well, it really needs to take account of all food items in the last 5 days
			}else{
				biomassExperienced *= totalExperiencedBiomass; 
				biomassExperienced /= static_cast<unsigned int>(getFinalTraitValue(Trait::Type::memoryDepth)); ///experience averaged over the last 5 days and continue experiencing from 0 days
				daysExperienced = 0;	
			}
		}
	}
}


const double& AnimalNonStatistical::getFoodMassLeftForNextTimeStep() const
{
	return foodMassLeftForNextTimeStep;
}


void AnimalNonStatistical::setFoodMassLeftForNextTimeStep(const double &foodMassLeftForNextTimeStep)
{
	this->foodMassLeftForNextTimeStep = foodMassLeftForNextTimeStep;
}


void AnimalNonStatistical::decreaseFoodMassLeftForNextTimeStep(const double &foodToEliminate)
{
	foodMassLeftForNextTimeStep -= foodToEliminate;
}


bool AnimalNonStatistical::isThereLeftoverFood() const
{
	return getFoodMassLeftForNextTimeStep() > DBL_EPSILON;
}


void AnimalNonStatistical::assimilateLastHuntedAnimalAndComputeHandlingTime()
{
	if(isThereLeftoverFood() && timeStepMaximumHandlingTimer > 0.0)
	{
		double kelvinTemperature = this->getTerrainCellInterface()->getMoistureInfo()->getTemperature().getTemperatureKelvin();
		double log_ratio = calculateLogMassRatio(calculateDryMass(), lastHuntedAnimalDryMass);
		double lnHandlingTime = -1814 + 0.7261*log_ratio + 12.04*kelvinTemperature + (-0.02006)*pow(kelvinTemperature, 2);
		double handlingTime = (1.0/exp(lnHandlingTime)) / (60 * 60 * 24); //Converted here to DAYS

		//TODO generalizar a timestep
		double handlingTimeToAdd = handlingTime * (getFoodMassLeftForNextTimeStep() / lastHuntedAnimalDryMass);

		double addedWouldBeHandlingTimer = handlingTimer + handlingTimeToAdd;
		double foodMassToAdd;
		if(addedWouldBeHandlingTimer <= timeStepMaximumHandlingTimer)
		{
			foodMassToAdd = getFoodMassLeftForNextTimeStep();
			//cout << "first" << endl;
		}
		else //addedWouldBeHandlingTimer > timeStepMaximumHandlingTimer
		{
			if((timeStepMaximumHandlingTimer - handlingTimer) > 0.0)
			{
				foodMassToAdd = (timeStepMaximumHandlingTimer - handlingTimer) * (getFoodMassLeftForNextTimeStep() / handlingTimeToAdd);
			} else {
				foodMassToAdd = 0.0;
			}

			//cout << "second" << endl;

		}

		//Converting the full dry mass to only profitable mass; that is applying assimilation efficiency and profitability
		double profitableFoodMassToAdd = foodMassToAdd * (edibleToBePredatedProfitability + getFinalTraitValue(Trait::Type::assim));

		double foodMassToAssimilate;

		double addedWouldBeFoodMass = foodMass + profitableFoodMassToAdd;
		if(addedWouldBeFoodMass >= getFinalTraitValue(Trait::Type::voracity))
		{

			foodMassToAssimilate = getFinalTraitValue(Trait::Type::voracity) - foodMass;
			decreaseFoodMassLeftForNextTimeStep(foodMassToAssimilate / (edibleToBePredatedProfitability + getFinalTraitValue(Trait::Type::assim)));
			foodMass = getFinalTraitValue(Trait::Type::voracity);
			sated = true;

			//cout << "SECOND   " << endl;

			//cout << sated << endl; 
		}
		else //0 mass left on the ground OR do not eat anything at all
		{
			foodMassToAssimilate = profitableFoodMassToAdd;
			decreaseFoodMassLeftForNextTimeStep(foodMassToAdd);
			foodMass += foodMassToAssimilate;
			if(getRemainingVoracity() < 0.0000000001)
			{
				foodMass = getFinalTraitValue(Trait::Type::voracity);
				sated = true;
			}
		}

		double handlingTimeToAssimilate = handlingTime * ((foodMassToAssimilate / (edibleToBePredatedProfitability + getFinalTraitValue(Trait::Type::assim))) / lastHuntedAnimalDryMass);

		//traits[Trait::energy_tank] += traits[Trait::assim] * foodMassToAssimilate;
		handlingTimer += handlingTimeToAssimilate;
		if(timeStepMaximumHandlingTimer > 0.0)
		{
			setFinalTraitValue(Trait::Type::search_area, getFinalTraitValue(Trait::Type::search_area) - (handlingTimeToAssimilate / timeStepMaximumHandlingTimer) * timeStepMaximumSearchArea);
		}

		if(::isnan(foodMass) || ::isnan(handlingTimer))
		{
			cout << "Animal id: " << getId() << " (" << this->getSpecies()->getScientificName() << ") - The assimilation resulted in a NaN value for foodMass or handlingTimer." << endl;
		}

		if(!getSpecies()->getPreserveLeftovers()) 
		{
			setFoodMassLeftForNextTimeStep(0.0);
		}
	}
}


const double AnimalNonStatistical::getInterpolatedDryMass(const unsigned int evaluationDepth, const double &dryMass) const
{
	return MathFunctions::linearInterpolate01(dryMass, getSpecies()->getMaximumDryMassObserved());
}


double AnimalNonStatistical::calculatePredatoryRiskEdibilityValue(const EdibleInterface &edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	double edibilityValue = (calculateEncounterProbability(edibleToBeEvaluated, edibleToBeEvaluated.calculateDryMass(), muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH)
			+ calculatePredationProbability(edibleToBeEvaluated, edibleToBeEvaluated.calculateDryMass(), false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW)
			+ getSpecies()->getEdiblePreference(edibleToBeEvaluated.getSpecies()->getId(), getInstar(), edibleToBeEvaluated.getInstar())
			+ getMeanExperience(edibleToBeEvaluated.getSpecies()->getId()))/4;

	return edibilityValue;
}


double AnimalNonStatistical::calculateEdibilityValue(const EdibleInterface &edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	double edibilityValue = (calculateEncounterProbability(edibleToBeEvaluated, edibleToBeEvaluated.calculateDryMass(), muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH)
			+ calculatePredationProbability(edibleToBeEvaluated, edibleToBeEvaluated.calculateDryMass(), false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW)
			+ getSpecies()->getEdiblePreference(edibleToBeEvaluated.getSpecies()->getId(), getInstar(), edibleToBeEvaluated.getInstar())
			+ getMeanExperience(edibleToBeEvaluated.getSpecies()->getId()))/4;

	return edibilityValue;
}


double AnimalNonStatistical::calculateEdibilityValueWithMass(const EdibleInterface &edibleToBeEvaluated, const double &dryMass, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	double edibilityValue = edibleToBeEvaluated.getInterpolatedDryMass(static_cast<const PointSpatialTree &>(getTerrainCellInterface()->getPosition()).getDepth(), dryMass)
			* ((calculateEncounterProbability(edibleToBeEvaluated, dryMass, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH)
			+ calculatePredationProbability(edibleToBeEvaluated, dryMass, false, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW)
			+ getSpecies()->getEdiblePreference(edibleToBeEvaluated.getSpecies()->getId(), getInstar(), edibleToBeEvaluated.getInstar())
			+ getMeanExperience(edibleToBeEvaluated.getSpecies()->getId()))/4);

	return edibilityValue;
}


double AnimalNonStatistical::calculateEncounterProbability(const EdibleInterface &edibleToBeEncountered, const double &dryMass, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	double encounterProbability = 1.0;
	if(edibleToBeEncountered.getSpecies()->isMobile())
	{
		double interpolatedHuntedVoracity = getInterpolatedHuntedVoracity(edibleToBeEncountered.getVoracity());
		double interpolatedHunterVoracity = getInterpolatedHunterVoracity();
		double interpolatedVoracityProduct = getInterpolatedVoracityProduct(edibleToBeEncountered.getVoracity());

		double interpolatedHuntedBodySize = getInterpolatedHuntedBodySize(static_cast<const Animal&>(edibleToBeEncountered).getAnimalGrowth()->getCurrentBodySize());
		double interpolatedHunterBodySize = getInterpolatedHunterBodySize();

		double probabilityDensityFunction = exp(-0.5 * pow((log(calculateWetMass()/edibleToBeEncountered.getSpecies()->convertToWetMass(dryMass)) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
		double interpolatedPDF = getInterpolatedPDF(probabilityDensityFunction);

		switch (getHuntingMode()) {
			case HuntingMode::sit_and_wait: {
				encounterProbability = 
					(encounterHuntedVoracitySAW * interpolatedHuntedVoracity +
					encounterHunterVoracitySAW * interpolatedHunterVoracity +
					encounterVoracitiesProductSAW * interpolatedVoracityProduct +
					encounterHunterSizeSAW * interpolatedHunterBodySize +
					encounterHuntedSizeSAW * interpolatedHuntedBodySize +
					encounterProbabilityDensityFunctionSAW * interpolatedPDF)/6;
				break;
			}
			case HuntingMode::active_hunting: {
				encounterProbability = 
					(encounterHuntedVoracityAH * interpolatedHuntedVoracity +
					encounterHunterVoracityAH * interpolatedHunterVoracity +
					encounterVoracitiesProductAH * interpolatedVoracityProduct +
					encounterHunterSizeAH * interpolatedHunterBodySize +
					encounterHuntedSizeAH * interpolatedHuntedBodySize +
					encounterProbabilityDensityFunctionAH * interpolatedPDF)/6;
				break;
			}
			case HuntingMode::does_not_hunt: {
				break;
			}
			default: {
				throwLineInfoException("Default case");
				break;
			}
		}
	}

	return encounterProbability;
}


bool AnimalNonStatistical::encounterEdible(EdibleInterface &edibleToBeEncountered, const double &targetDryMass, float attackOrExposedAttackProbability, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const
{
	double randomEncounterProbability = Random::randomUniform();

	encounterProbabilitiesFile << this->getIdStr() << "\t"
	<< edibleToBeEncountered.getIdStr() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< edibleToBeEncountered.getSpecies()->getScientificName() << "\t"
	<< ((edibleToBeEncountered.isHunting()) ? 1 : 0) << "\t"
	<< calculateDryMass() << "\t"
	<< targetDryMass << "\t"
	<< randomEncounterProbability << "\t"
	<< calculateEncounterProbability(edibleToBeEncountered, targetDryMass, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH) << "\t"
	<< attackOrExposedAttackProbability << "\t"											 
	<< ((attackOrExposedAttackProbability*calculateEncounterProbability(edibleToBeEncountered, targetDryMass, muForPDF, sigmaForPDF, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH) > randomEncounterProbability)?1:0) << "\t"
	<< endl;

	#ifdef DEBUG
	if(find(searchedAnimalsToday.begin(), searchedAnimalsToday.end(), edibleToBeEncountered.getId()) != searchedAnimalsToday.end())
	{
		//getTerrainCellInterface()->increaseMultipleSameSearchedAnimalToday();


		//cerr << "The animal searched for the same animal at least twice the same day: " << endl;
		//cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
		//cerr << " - Animal searched for: " << searchedAnimal->getId() << "(" << searchedAnimal->getSpecies()->getScientificName() << ")" << endl;
	}


	//searchedAnimalsToday.push_back(edibleToBeEncountered.getId());
	#endif
	//if (encounterProbability > randomEncounterProbability)
	//*calculateEncounterProbability(edibleToBeEncountered)
	if ((attackOrExposedAttackProbability > randomEncounterProbability) || !edibleToBeEncountered.getSpecies()->isMobile())
	{
		#ifdef DEBUG
		if(find(encounteredAnimalsToday.begin(), encounteredAnimalsToday.end(), edibleToBeEncountered.getId()) != encounteredAnimalsToday.end())
		{
			//getTerrainCellInterface()->increaseMultipleSameEncounteredAnimalToday();


			//cerr << "The animal encountered the same animal at least twice the same day: " << endl;
			//cerr << " - Animal: " << id << "(" << getSpecies()->getScientificName() << ")" << endl;
			//cerr << " - Animal encountered: " << searchedAnimal->getId() << "(" << searchedAnimal->getSpecies()->getScientificName() << ")" << endl;
		}

		
		//encounteredAnimalsToday.push_back(edibleToBeEncountered.getId());
		#endif
		// if encounter occurs
		if(edibleToBeEncountered.getSpecies()->isMobile()){
		edibleToBeEncountered.incrementEncountersWithPredator(getId());
		}
		//incrementEncounters_prey();
		return true;
	}

	return false;
}


bool AnimalNonStatistical::canEatEdible(const EdibleInterface* const &edibleToCheck, const list<const EdibleInterface*> &ediblesHasTriedToPredate, const double &dryMass) const
{
	if(getSpecies()->canEatEdible(edibleToCheck->getSpecies()->getId(), getInstar(), edibleToCheck->getInstar()))
	{
		//this was incorrect in Gabi's version, he used just resource biomass vs min spore parameter
		//now considering everything the animal takes what is left by if available by decresing its voracity value
		double forNotToDepleteResource = getFinalTraitValue(Trait::Type::voracity)/(getSpecies()->getEdibleProfitability(edibleToCheck->getSpecies()->getId(), getInstar(), edibleToCheck->getInstar())+getFinalTraitValue(Trait::Type::assim));

		//this to to feed on what is left if he entire voracity cannot be satisfied
		//Warning: this prevents this animal to fulfill entirely its original voracity (voracity shrinks) with and alternative
		//resource species if it were available - need to test this behavior
		
		
		//arthros and for dinos - this allows feeding on the leftovers without surpassing
		//the limit imposed by the minspores... so resource can grow back
		bool newVor = false;
		if(edibleToCheck->isDepleted(forNotToDepleteResource, dryMass))
		{
			if(edibleToCheck->anyLeft(dryMass)*(getSpecies()->getEdibleProfitability(edibleToCheck->getSpecies()->getId(), getInstar(), edibleToCheck->getInstar())+getFinalTraitValue(Trait::Type::assim))>0){
				newVor = true;
			}
		}
			
		return !sated && !isExhausted() &&  
				(!edibleToCheck->isDepleted(forNotToDepleteResource, dryMass) || newVor == true) &&
				edibleToCheck->getPredatedByID() == -1 && predatedByID == -1 &&
				!hasTriedToHunt((Animal*)edibleToCheck, ediblesHasTriedToPredate);
	}

	return false;
}


bool AnimalNonStatistical::hasTriedToHunt(Animal* edibleToCheck, const list<const EdibleInterface*> &ediblesHasTriedToPredate) const
{
	return find(ediblesHasTriedToPredate.cbegin(), ediblesHasTriedToPredate.cend(), edibleToCheck) != ediblesHasTriedToPredate.cend();
}

template <class Archive>
void AnimalNonStatistical::serialize(Archive &ar, const unsigned int version) {

    #ifdef DEBUG
	ar & searchedAnimalsToday;
	ar & encounteredAnimalsToday;
	ar & huntedAnimalsToday;
	ar & predatedAnimalsToday;
	#endif

	ar & daysExperienced;
	ar & foodMass;
	ar & finalJMaxVB;
	ar & maxNextInstarMassFromVBPlasticity;
	ar & edibleToBePredatedProfitability;
	ar & foodMassLeftForNextTimeStep;
	ar & lastHuntedAnimalDryMass;
	ar & days_digest;


	ar & eatenTodayList;
	ar & eatenToday;
	ar & wetMassAtTheBeginningOfTheTimeStep;
	ar & oldSearchArea;

	ar & huntingMode;
	ar & genome;

	ar & currentPostTempGrowth;
	ar & currentPostTempLinf;

	ar & maxPostTvor;
	ar & minPostTvor;
	ar & maxPostTsearch;
	ar & minPostTsearch;
	ar & maxPostTspeed;
	ar & minPostTspeed;

	ar & inHabitatShift;

	ar & inBreedingZone;

	ar & inHabitatShiftBeforeBreeding;
	ar & inHabitatShiftAfterBreeding;

	ar & atDestination;
	ar & targetNeighborToTravelTo;

	ar & ageForNextReproduction;
	ar & idFromMatedMale;
	ar & generationNumberFromMatedMale;
	ar & mated;
	ar & genomeFromMatedMale;

	ar & steps;

	ar & stepsAttempted;

	ar & lifeStage;

	ar & encounterEvents;
	ar & predatedByID;
	ar & biomassExperiencedPerSpecies;
	ar & meanExperiencesPerSpecies;

	ar & todayEncountersWithPredators;
	ar & daysWithoutFood;
	ar & mature;
	ar & sated;
	ar & handlingTimer;
	ar & timeStepMaximumHandlingTimer;
	ar & timeStepMaximumSearchArea;
	ar & ageOfLastMoultOrReproduction;
	ar & dateOfDeath;
	ar & lastDayMoved;

	ar & assim_ini;

	ar & generationNumberFromFemaleParent;
	ar & generationNumberFromMaleParent;
	ar & ID_prt1; //TODO puntero al padre mejor? o no? esto se usa?
	ar & ID_prt2;
	ar & reproCounter;
	ar & fecundity;
}             
