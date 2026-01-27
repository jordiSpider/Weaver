#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"

#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"


using namespace std;
namespace fs = std::filesystem;




bool compareByEdibilityValue(const tuple<PreciseDouble, Edible*, DryMass>& firstElem, const tuple<PreciseDouble, Edible*, DryMass>& secondElem) {
  	return get<0>(firstElem) > get<0>(secondElem);
}

bool compareByPredationProbability(const pair<PreciseDouble, AnimalNonStatistical*>& firstElem, const pair<PreciseDouble, AnimalNonStatistical*>& secondElem) {
	return firstElem.first > secondElem.first;
}



AnimalNonStatistical::AnimalNonStatistical()
	: Animal()
{

}

AnimalNonStatistical::AnimalNonStatistical(const Instar &instar, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const Genome* const genome, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
	: Animal(EdibleID(0), mySpecies, terrainCell, genome, LifeStage::ACTIVE, actualTimeStep),
	  growthBuildingBlock( 
		&getMutableSpecies()->getMutableGrowthBuildingBlock(), instar, 
		getSpecies()->getGenetics().isGrowthTraitsThermallyDependent(), 
		actualTimeStep, getGender(), getGenetics().getBaseIndividualTraits(), getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue(),
		getSpecies()->getFemaleMaxReproductionEvents(), terrainCell->getPatchApplicator().getCellMoisture().getTemperature(), 
		mySpecies->getTempFromLab(), timeStepsPerDay
	  ),
	  decisions(this, getMutableSpecies()->getMutableDecisionsBuildingBlock(), timeStepsPerDay),
	  huntingMode(getSpecies()->getDefaultHuntingMode()), 
	  inBreedingZone(false), inHabitatShiftBeforeBreeding(false), inHabitatShiftAfterBreeding(false), atDestination(true), 
	  instarToEvaluateCells(instar)
{
	setOtherAttributes(Generation(0), Generation(0), EdibleID(), EdibleID());

	// Check that the value of growth is in the valid value range
	checkGrowthMinimumValue();
}


AnimalNonStatistical::AnimalNonStatistical(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& newFactorEggMassFromMom, const Generation& g_numb_prt_female,
		const Generation& g_numb_prt_male, EdibleID ID_prt_female, EdibleID ID_prt_male, AnimalSpecies* const mySpecies, Gender gender, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
	: Animal(EdibleID(0), mySpecies, parentTerrainCell, LifeStage::UNBORN, firstParentGamete, secondParentGamete, gender, actualTimeStep), 
	  growthBuildingBlock(
		&getMutableSpecies()->getMutableGrowthBuildingBlock(), Instar(), 
		getSpecies()->getGenetics().isGrowthTraitsThermallyDependent(), 
		actualTimeStep, getGender(), getGenetics().getBaseIndividualTraits(), newFactorEggMassFromMom,
		getSpecies()->getFemaleMaxReproductionEvents(), terrainCell->getPatchApplicator().getCellMoisture().getTemperature(), 
		mySpecies->getTempFromLab(), timeStepsPerDay
	  ),
	  decisions(this, getMutableSpecies()->getMutableDecisionsBuildingBlock(), timeStepsPerDay),
	  huntingMode(getSpecies()->getDefaultHuntingMode()),
	  inBreedingZone(false), inHabitatShiftBeforeBreeding(false), inHabitatShiftAfterBreeding(false), atDestination(true)
{
	setOtherAttributes(g_numb_prt_female, g_numb_prt_male, ID_prt_female, ID_prt_male);

	// Check that the value of growth is in the valid value range
	checkGrowthMinimumValue();
}


void AnimalNonStatistical::setOtherAttributes(const Generation& g_numb_prt_female, const Generation& g_numb_prt_male, EdibleID ID_prt_female, EdibleID ID_prt_male)
{
	getMutableSpecies()->increasePopulation();

	mated = false;
	genomeFromMatedMale = nullptr;

	steps = 0.0;
	exhausted = false;
	stepsAttempted = 0;
	sated = false;
	//encounters_prey = 0;

	totalPredationEncounters = 0u;
	predationEncountersCurrentDay = 0u;

	timeStepsWithoutFood = TimeStep(0);

	unbornTimeSteps = TimeStep(0);


	setAgeOfLastMoultOrReproduction(TimeStep(0));
	dateOfDeath = Day(-1);
	lastDayMoved = TimeStep(0);
	firstMovement = true;
	

	eatenToday = 0;
	
	generationNumberFromFemaleParent = g_numb_prt_female;
	generationNumberFromMaleParent = g_numb_prt_male;
	idFromFemaleParent = ID_prt_female; //TODO puntero al padre mejor? o no? esto se usa?
	idFromMaleParent = ID_prt_male;
	reproCounter = 0;
	fecundity = 0;
	ageOfFirstReproduction = TimeStep(0);


	const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, OntogeneticLink>>* const ontogeneticLinks = &getSpecies()->getOntogeneticLinksPerInstar(getGrowthBuildingBlock().getInstar());

	foodMassAssimilatedCurrentTimeStepPerSpecies.resize(ontogeneticLinks->size());
	foodMassEatenCurrentTimeStepPerSpecies.resize(ontogeneticLinks->size());

	for(size_t speciesId = 0; speciesId < ontogeneticLinks->size(); speciesId++)
	{
		foodMassAssimilatedCurrentTimeStepPerSpecies.getValue(speciesId).resize(ontogeneticLinks->getValue(speciesId).size(), DryMass(0.0));
		foodMassEatenCurrentTimeStepPerSpecies.getValue(speciesId).resize(ontogeneticLinks->getValue(speciesId).size(), DryMass(0.0));
	}

	foodMassAssimilatedCurrentTimeStep = DryMass(0.0);
	foodMassEatenCurrentTimeStep = DryMass(0.0);
}


AnimalNonStatistical::~AnimalNonStatistical()
{
	getMutableSpecies()->decreasePopulation();

	if(genomeFromMatedMale != nullptr) delete genomeFromMatedMale;
}

void AnimalNonStatistical::increaseAge(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay)
{
	getMutableGrowthBuildingBlock().setCurrentAge(getGrowthBuildingBlock().getCurrentAge() + TimeStep(1));

	//TODO PARA LOS CRECIMIENTO INDETERMINADO, SE DEJAN POR AHORA QUE SIGAN MOVIENDOSE INFINITO
	//TODO EN UN FUTURO SE HARÁ PARA LOS INDETERMINADO, DEJANDO QUE SIGAN CRECIENDO SI ALCANZAN EL TIEMPO DE LONGEVITY
	if(getGrowthBuildingBlock().getCurrentAge() > getGrowthBuildingBlock().getLongevity())
	{
		setNewLifeStage(landscape, LifeStage::SENESCED, numberOfTimeSteps, timeStepsPerDay);
	}
}

void AnimalNonStatistical::checkEnergyTank(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay)
{
	if(getGrowthBuildingBlock().getCurrentEnergyTank() <= 0.0)
	{
		if(getSpecies()->isSurviveWithoutFood()) {
			getMutableGrowthBuildingBlock().modifyEnergyTank(DryMass(0.1), numberOfTimeSteps);
		}
		else {
			setNewLifeStage(landscape, LifeStage::STARVED, numberOfTimeSteps, timeStepsPerDay);
		}
	}
}

void AnimalNonStatistical::tune(Landscape* const landscape, const bool saveMassInfo, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay)
{
	const Temperature temperature = getTerrainCell()->getPatchApplicator().getCellMoisture().getTemperature();
	const PreciseDouble relativeHumidity = getTerrainCell()->getPatchApplicator().getCellMoisture().getMoisture();


	getMutableGenetics().tune(temperature, numberOfTimeSteps, 
		getSpecies()->getGrowthBuildingBlock().getCoefficientForMassAforMature(), 
		getSpecies()->getGrowthBuildingBlock().getScaleForMassBforMature(), 
		getSpecies()->getTempFromLab()
	);

	// Check that the value of growth is in the valid value range
	checkGrowthMinimumValue();


	DryMass basalMetabolicDryMassLoss = landscape->getBasalMetabolicDryMassLossPerTimeStep(
		getGrowthBuildingBlock().getCurrentTotalWetMass(), 
		getGenetics().getBaseIndividualTraits(BaseTraitType::actE_met).getPhenotypicValue(), 
		getSpecies()->getGenetics().getBaseTraits(BaseTraitType::actE_met)->isThermallyDependent(),
		getGenetics().getBaseIndividualTraits(BaseTraitType::met_rate).getPhenotypicValue(), 
		getSpecies()->getGenetics().getBaseTraits(BaseTraitType::met_rate)->isThermallyDependent(),
		getSpecies()->getTempFromLab(), getSpecies()->getTempFromLab(), 
		getSpecies()->getGrowthBuildingBlock().getConversionToWetMass(getGrowthBuildingBlock().getInstar())
	);


	if(getLifeStage() != LifeStage::REPRODUCING)
	{
		getMutableGrowthBuildingBlock().calculateNextMassPredicted(getReproCounter(), getMaxReproductionEvents(), getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue(), getGender(), timeStepsPerDay);

		
		maxVoracityTimeStep = landscape->calculateMaxVoracityTimeStep(getGrowthBuildingBlock().getCurrentTotalWetMass(), getSpecies()->getGrowthBuildingBlock().getConversionToWetMass(getGrowthBuildingBlock().getInstar()), getGrowthBuildingBlock().hasCapitalBreeding(), basalMetabolicDryMassLoss, getSpecies()->getYodzisA(), getSpecies()->getYodzisB());



		DryMass predVor = getMutableGrowthBuildingBlock().calculatePredVor(basalMetabolicDryMassLoss, getGenetics().getBaseIndividualTraits(BaseTraitType::assim).getPhenotypicValue(), previousNetGrowth, numberOfTimeSteps);


		// DryMass preFinalVoracity = DryMass(fmin(maxVoracityTimeStep.getValue(), predVor.getValue()));
		DryMass preFinalVoracity = predVor;

		beyondCurve = (preFinalVoracity == predVor);

		if(preFinalVoracity < 0.0)
		{
			throwLineInfoException("Animal " + string(getId()) + ": Maximum voracity timeStep is negative.");
		}


		DryMass previousTarget = getGrowthBuildingBlock().calculatePreviousTarget(getGender(), getReproCounter());
		DryMass nextTarget = getGrowthBuildingBlock().calculateNextTarget(getGender());


		// enhanced h
		PreciseDouble h;

		if(getGrowthBuildingBlock().hasCapitalBreeding())
		{
			h = 0.0;
		}
		else
		{
			if(nextTarget == previousTarget)
			{
				h = 0.0;
			}
			else
			{
				h = 1-(getGrowthBuildingBlock().getCurrentTotalDryMass()-previousTarget).getValue()/(nextTarget-previousTarget).getValue();

				if(h < 0)
				{
					h = 0.0;
				}
			}
		}

			
		PreciseDouble voracityAfterApplyProportion = preFinalVoracity.getValue() * getGenetics().getBaseIndividualTraits(BaseTraitType::voracityProportion).getPhenotypicValue();

		
		voracityAfterApplyProportion = fmax(voracityAfterApplyProportion, 0.0);


		scopeAreaRadius = applyAllometricModel(
			getGenetics().getBaseIndividualTraits(BaseTraitType::coeffMassForScopeRadius).getPhenotypicValue(),
			getGenetics().getBaseIndividualTraits(BaseTraitType::scaleMassForScopeRadius).getPhenotypicValue()
		);

		interactionAreaRadius = applyAllometricModel(
			getGenetics().getBaseIndividualTraits(BaseTraitType::coeffMassForInteractionRadius).getPhenotypicValue(),
			getGenetics().getBaseIndividualTraits(BaseTraitType::scaleMassForInteractionRadius).getPhenotypicValue()
		);


		PreciseDouble searchAfterAllometric = applyAllometricModel(
			getGenetics().getBaseIndividualTraits(BaseTraitType::coeffMassForSearchRadius).getPhenotypicValue(),
			getGenetics().getBaseIndividualTraits(BaseTraitType::scaleMassForSearchRadius).getPhenotypicValue()
		);
		PreciseDouble searchAfterPlasticity = applyPlasticityDueToConditionToTrait(searchAfterAllometric, getSpecies()->getPlasticityDueToConditionSearch(), h);
		

		PreciseDouble speedAfterAllometric = applyAllometricModel(
			getGenetics().getBaseIndividualTraits(BaseTraitType::coeffMassForSpeed).getPhenotypicValue(),
			getGenetics().getBaseIndividualTraits(BaseTraitType::scaleMassForSpeed).getPhenotypicValue()
		);
		
		PreciseDouble postTspeed = landscape->calculatePostTSpeed(speedAfterAllometric, getGrowthBuildingBlock().getCurrentTotalWetMass());
		PreciseDouble speedAfterPlasticity = applyPlasticityDueToConditionToTrait(postTspeed, getSpecies()->getPlasticityDueToConditionSpeed(), h);


		
		PreciseDouble searchAfterEncounters;
		PreciseDouble vorAfterEncounters;

		if(getPredationEncountersCurrentDay() > 0)
		{
			PreciseDouble hEnc = 1.0-(static_cast<double>(getPredationEncountersCurrentDay())/static_cast<double>(getSpecies()->getMaximumPredationEncountersPerDay()));
		
			if(hEnc < 0)
			{
				hEnc = 0;
			}

			vorAfterEncounters = voracityAfterApplyProportion*(1-exp(-getSpecies()->getActivityUnderPredationRisk()*hEnc));
			searchAfterEncounters = searchAfterPlasticity*(1-exp(-getSpecies()->getActivityUnderPredationRisk()*hEnc));
		}
		else
		{
			vorAfterEncounters = voracityAfterApplyProportion;
			searchAfterEncounters = searchAfterPlasticity;
		}




		searchAreaRadius = searchAfterEncounters;

		speed = speedAfterPlasticity;

		voracity = vorAfterEncounters;



		if(getGrowthBuildingBlock().isMature() && getGender() == Gender::MALE)
		{
			searchAreaRadius = searchAreaRadius * getSpecies()->getMaleMobility();
		}


		//below it means that metabolic downregulation only exists for predators, such as spiders
		if(getHuntingMode() != HuntingMode::does_not_hunt && timeStepsWithoutFood >= getSpecies()->getTimeStepsWithoutFoodForMetabolicDownregulation())
		{
			voracity = voracity - getSpecies()->getPercentageCostForMetabolicDownregulationVoracity() * voracity;

			searchAreaRadius = searchAreaRadius - getSpecies()->getPercentageCostForMetabolicDownregulationSearchArea() * searchAreaRadius;

			speed = speed - getSpecies()->getPercentageCostForMetabolicDownregulationSpeed() * speed;
		}



		searchAreaRadius = searchAreaRadius * timeStepsPerDay;

		sated = (getVoracity() <= 0.0);

		exhausted = (getSearchAreaRadius() <= 0.0);
	}


	


	//In this version the shock_resistance trait involves Maximum Critical Temperature (CTmax)
	if(getGenetics().getBaseIndividualTraits(BaseTraitType::shock_resistance).getPhenotypicValue() < temperature.getTemperatureKelvin())
	{
		setNewLifeStage(landscape, LifeStage::SHOCKED, numberOfTimeSteps, timeStepsPerDay);
	}


	if(relativeHumidity < getSpecies()->getMinRelativeHumidityThreshold(getGrowthBuildingBlock().getInstar()))
	{
		setNewLifeStage(landscape, LifeStage::DIAPAUSE);
	}



	getMutableGrowthBuildingBlock().tune(
		getGenetics().getBaseIndividualTraits(), 
		numberOfTimeSteps, getSpecies()->getFemaleMaxReproductionEvents(), 
		temperature, getSpecies()->getTempFromLab(), timeStepsPerDay
	);




	Length growthCurveMassLength = getGrowthBuildingBlock().getIndividualGrowthModel()->calculateLength(Day(getGrowthBuildingBlock().getCurrentAge(), timeStepsPerDay), getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue());

	DryMass growthCurveMass = getSpecies()->getGrowthBuildingBlock().calculateDryMass(growthCurveMassLength, getGrowthBuildingBlock().isMature());


	DryMass instarTargetMass;
	TimeStep instarTargetAge;

	if(getGrowthBuildingBlock().getInstar() == getSpecies()->getGrowthBuildingBlock().getLastInstar())
	{
		instarTargetMass = DryMass(0.0);
		instarTargetAge = TimeStep(0);
	}
	else
	{
		instarTargetMass = getGrowthBuildingBlock().getInstarMass(getGrowthBuildingBlock().getInstar()+1);
		instarTargetAge = getGrowthBuildingBlock().getInstarAge(getGrowthBuildingBlock().getInstar()+1);
	}


	if(saveMassInfo)
	{
		std::ofstream infoMassFile;

		createOutputFile(infoMassFile, landscape->getResultFolder() / fs::path("massInfo") / fs::path("animal_" + string(getId()) + ".txt"), ios::app);

		ostringstream infoMassContent;

		infoMassContent << numberOfTimeSteps << "\t" << getGrowthBuildingBlock().getCurrentTotalDryMass() << "\t" << instarTargetMass << "\t" << getGrowthBuildingBlock().getMassForNextReproduction() << "\t" << growthCurveMass << "\t" << getGrowthBuildingBlock().getNextMassPredicted() << "\t" << getGrowthBuildingBlock().getCurrentAge() << "\t" << instarTargetAge << "\t" << getGrowthBuildingBlock().getAgeForNextReproduction() << endl;

		infoMassFile << infoMassContent.str();

		infoMassFile.close();
	}






	getMutableSpecies()->getMutableDecisionsBuildingBlock()->updateMaximumVoracity(getGrowthBuildingBlock().getInstar(), getVoracity());

	getMutableSpecies()->updateMaximumInteractionArea(getGrowthBuildingBlock().getInstar(), getInteractionAreaRadius());







	//TODO-SEMI FALTA ASOCIAR WeightPerLocus tiene que ser un código el cual indique qué locus de qué rasgo tiene un valor específico.
	//Está sin utilizar ahora mismo. Será una lista de entradas de la forma: mirar linked_aca

	//Output: "id\tspecies\tstate\tcurrent_age\ttarget_instar_devtime\tinstar\ttarget_instar\tbody_size\tenergy_tank\tbody_mass\ttarget_mass\tmin_expected_body_mass\texpected_body_mass\tmax_expected_body_mass\tcondition_dependence\tmax_condition_dependence\tpreT_voracity\tpreT_search_area\tpreT_speed\tmin_postT_voracity\tmin_postT_search_area\tmin_postT_speed\tpostT_voracity\tpostT_search_area\tpostT_speed\tmax_postT_voracity\tmax_postT_search_area\tmax_postT_speed\tconditioned_voracity\tconditioned_search_area\tconditioned_speed" << endl;

	//TODO Diapausa cuando se pone el huevo. diapauseTimer = pheno. Solamente se disminuye diapauseTimer si las condiciones (temperatura y/o humedad) lo permiten, por debajo de un umbral.
	//TODO Los numeros de individuos por instar ahora van a ser DENSIDADES por instar. Que pasaran a ser numero de individuos dependiendo del área habitable.

	//TODO Eliminar el hongo cuando sea <= 0, y borrar minimumresourcecapacity y getZeroResource.
	//TODO Añadir un parametro que decida si el animal crece por mudas o continuo (dinosaurios)
}


void AnimalNonStatistical::setScopeAreaRadius(const PreciseDouble& newValue)
{
	scopeAreaRadius = newValue;
}


const size_t& AnimalNonStatistical::getPredationEncountersCurrentDay() const
{
	return predationEncountersCurrentDay;
}




//TODO parametro para que se ejecuten cada X timesteps

void AnimalNonStatistical::transferAssimilatedFoodToEnergyTank(const TimeStep actualTimeStep)
{
	getMutableGrowthBuildingBlock().modifyEnergyTank(getGrowthBuildingBlock().getCurrentEnergyTank() + foodMassAssimilatedCurrentTimeStep, actualTimeStep);
}

PreciseDouble AnimalNonStatistical::calculateProportionOfTimeWasMoving() const
{
	return (getSearchAreaRadius() > 0.0) ? steps/getSearchAreaRadius() : 0.0;
}

void AnimalNonStatistical::metabolize(View* view, const Landscape* const landscape, const TimeStep& actualTimeStep)
{
	DryMass lastEnergyTank = getGrowthBuildingBlock().getCurrentEnergyTank();

	//double currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::eggDevTime] + 1.0/timeStepsPerDay;

	DryMass totalMetabolicDryMassLoss = landscape->getMetabolicDryMassLossPerTimeStep(
		getGrowthBuildingBlock().getCurrentTotalWetMass(), calculateProportionOfTimeWasMoving(), 
		getGenetics().getBaseIndividualTraits(BaseTraitType::actE_met).getPhenotypicValue(), 
		getSpecies()->getGenetics().getBaseTraits(BaseTraitType::actE_met)->isThermallyDependent(),
		getGenetics().getBaseIndividualTraits(BaseTraitType::met_rate).getPhenotypicValue(), 
		getSpecies()->getGenetics().getBaseTraits(BaseTraitType::met_rate)->isThermallyDependent(),
		getSearchAreaRadius(),
		getSpecies()->getTempFromLab(), getSpecies()->getTempFromLab(), 
		getSpecies()->getGrowthBuildingBlock().getConversionToWetMass(getGrowthBuildingBlock().getInstar())
	);

	//Downregulation only here, do not change this into getMetabolicDryMassLoss because it would alter the expected loss in tuneTraits
	if(getHuntingMode() != HuntingMode::does_not_hunt && timeStepsWithoutFood >= getSpecies()->getTimeStepsWithoutFoodForMetabolicDownregulation())
	{
		totalMetabolicDryMassLoss = totalMetabolicDryMassLoss - totalMetabolicDryMassLoss.getValue() * getSpecies()->getPercentageMetabolicDownregulation();
	}

	getMutableGrowthBuildingBlock().modifyEnergyTank(getGrowthBuildingBlock().getCurrentEnergyTank() - totalMetabolicDryMassLoss, actualTimeStep);


	previousNetGrowth = getGrowthBuildingBlock().getCurrentTotalDryMass();


	DryMass afterLossEnergyTank = getGrowthBuildingBlock().getCurrentEnergyTank();
	if(afterLossEnergyTank >= lastEnergyTank)
	{
		view->updateLog("The metabolic loss was 0 or positive:\n");
		view->updateLog({" - Animal: ", to_string(getId()), "(", getSpecies()->getScientificName(), ")\n"});
		view->updateLog({" - Last energy tank: ", lastEnergyTank.getValue().to_string(), "\n"});
		view->updateLog({" - After loss energy tank: ", afterLossEnergyTank.getValue().to_string(), "\n"});
	}
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

void AnimalNonStatistical::isReadyToBeBorn(Landscape* const landscape, const PreciseDouble& timeStepsPerDay)
{
	unbornTimeSteps = unbornTimeSteps + TimeStep(1);

	TimeStep eggDevTime(Day(getGenetics().getBaseIndividualTraits(BaseTraitType::eggDevTime).getPhenotypicValue()), timeStepsPerDay);

	if(unbornTimeSteps >= eggDevTime)
	{
		setNewLifeStage(landscape, LifeStage::ACTIVE);
		getMutableGrowthBuildingBlock().setCurrentAge(TimeStep(0));
	}
}


PreciseDouble AnimalNonStatistical::calculateLogMassRatio(DryMass preyDryMass) const
{
	return log(getGrowthBuildingBlock().getCurrentTotalDryMass().getValue()/preyDryMass.getValue());
}


void AnimalNonStatistical::increasePredationEncounters()
{
	totalPredationEncounters++;
	predationEncountersCurrentDay++;
	getMutableSpecies()->updateMaximumPredationEncountersPerDay(getPredationEncountersCurrentDay());
}

const TimeStep& AnimalNonStatistical::getAgeOfFirstReproduction() const 
{ 
    return ageOfFirstReproduction; 
}

const AnimalNonStatisticalGrowth& AnimalNonStatistical::getGrowthBuildingBlock() const
{
	return growthBuildingBlock;
}

AnimalNonStatisticalGrowth& AnimalNonStatistical::getMutableGrowthBuildingBlock()
{
	return growthBuildingBlock;
}

const unsigned int& AnimalNonStatistical::getReproCounter() const
{
	return reproCounter;
}

const unsigned int& AnimalNonStatistical::getFecundity() const
{
	return fecundity;
}

const Generation& AnimalNonStatistical::getGenerationNumberFromFemaleParent() const
{
	return generationNumberFromFemaleParent;
}

const Generation& AnimalNonStatistical::getGenerationNumberFromMaleParent() const
{
	return generationNumberFromMaleParent;
}

const EdibleID& AnimalNonStatistical::getIdFromFemaleParent() const
{
	return idFromFemaleParent;
}

const EdibleID& AnimalNonStatistical::getIdFromMaleParent() const
{
	return idFromMaleParent;
}

AnimalNonStatistical::operator std::string() const 
{
    std::ostringstream oss;

	oss << getId() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< EnumClass<Gender>::to_string(getGender());

	for(unsigned char axis = 0; axis < DIMENSIONS; axis++)
	{
		oss << "\t" << getPositionAxisValue(getPosition(), axis);
	}

	oss << "\t" << EnumClass<LifeStage>::to_string(getLifeStage()) << "\t"
	<< getGrowthBuildingBlock().getInstar() << "\t"
	<< getGrowthBuildingBlock().getCurrentAge() << "\t"
	<< getGenetics().getBaseIndividualTraits(BaseTraitType::eggDevTime).getConstitutiveValue() << "\t"
	<< getGrowthBuildingBlock().getDateEgg() << "\t"
	<< getAgeOfFirstReproduction() << "\t"
	<< getReproCounter() << "\t"
	<< getFecundity() << "\t"
	<< getDateOfDeath() << "\t"
	<< getGenerationNumberFromFemaleParent() << "\t"
	<< getGenerationNumberFromMaleParent() << "\t"
	<< getIdFromFemaleParent() << "\t"
	<< getIdFromMaleParent() << "\t"
	<< getPredationEncountersCurrentDay() << "\t"
	<< getTotalPredationEncounters() << "\t"
	<< getVoracity() << "\t"
	<< getSearchAreaRadius()<< "\t"
	<< getSpeed() << "\t"
	<< getGenetics().getBaseIndividualTraits(BaseTraitType::energy_tank).getConstitutiveValue() << "\t"
	<< getGenetics().getBaseIndividualTraits(BaseTraitType::eggDevTime).getConstitutiveValue() << "\t"
	<< getGrowthBuildingBlock().getCurrentBodySize() << "\t"
	<< getGrowthBuildingBlock().getCurrentTotalDryMass() << "\t"
	<< getGrowthBuildingBlock().getCurrentEnergyTank();

	oss << "\t" << getGenetics().to_string();

	return oss.str();
}

void AnimalNonStatistical::addSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay)
{
	foodMassAssimilatedCurrentTimeStepPerSpecies.emplace_back(numberOfInstars, DryMass(0.0));
	foodMassEatenCurrentTimeStepPerSpecies.emplace_back(numberOfInstars, DryMass(0.0));

	decisions.addSpecies(numberOfInstars, timeStepsPerDay);
}

void AnimalNonStatistical::addAnimalSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay)
{
	addSpecies(numberOfInstars, timeStepsPerDay);
}

void AnimalNonStatistical::addResourceSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay)
{
	addSpecies(numberOfInstars, timeStepsPerDay);
}

PreciseDouble AnimalNonStatistical::getRemainingVoracity() const
{ 
	return fmax(getVoracity() - foodMassEatenCurrentTimeStep.getValue(), 0.0); 
}

void AnimalNonStatistical::setSpecies(Species* newSpecies)
{
	Animal::setSpecies(newSpecies);

	decisions.setAnimalSpeciesDecisions(getMutableSpecies()->getMutableDecisionsBuildingBlock());
}

void AnimalNonStatistical::printVoracities(const Landscape* const landscape, ostringstream& voracitiesContent, const PreciseDouble& timeStepsPerDay)
{
	DryMass totalMetabolicDryMassLossAfterAssim = landscape->getMetabolicDryMassLossPerTimeStep(
		getGrowthBuildingBlock().getCurrentTotalWetMass(), calculateProportionOfTimeWasMoving(), 
		getGenetics().getBaseIndividualTraits(BaseTraitType::actE_met).getPhenotypicValue(), 
		getSpecies()->getGenetics().getBaseTraits(BaseTraitType::actE_met)->isThermallyDependent(),
		getGenetics().getBaseIndividualTraits(BaseTraitType::met_rate).getPhenotypicValue(), 
		getSpecies()->getGenetics().getBaseTraits(BaseTraitType::met_rate)->isThermallyDependent(),
		getSearchAreaRadius(),
		getSpecies()->getTempFromLab(), getSpecies()->getTempFromLab(), 
		getSpecies()->getGrowthBuildingBlock().getConversionToWetMass(getGrowthBuildingBlock().getInstar())
	);
	
	if(totalMetabolicDryMassLossAfterAssim > 0.0)
	{
		if(getHuntingMode() != HuntingMode::does_not_hunt && timeStepsWithoutFood >= getSpecies()->getTimeStepsWithoutFoodForMetabolicDownregulation())
		{
			totalMetabolicDryMassLossAfterAssim = totalMetabolicDryMassLossAfterAssim - totalMetabolicDryMassLossAfterAssim.getValue() * getSpecies()->getPercentageMetabolicDownregulation();
		}
	}


  	Length lengthPredicted(0.0);

	if(getGrowthBuildingBlock().getInstar() > 1) {
		lengthPredicted = getGrowthBuildingBlock().getIndividualGrowthModel()->calculateLength(Day(getGrowthBuildingBlock().getCurrentAge(), timeStepsPerDay), getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue());
	}
	else {
		lengthPredicted = getGrowthBuildingBlock().getIndividualGrowthModel()->calculateLength(Day(0), getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue());
	}

	DryMass massPredicted = getSpecies()->getGrowthBuildingBlock().calculateDryMass(lengthPredicted, true);

    DryMass minMassAtCurrentAge = massPredicted - massPredicted.getValue() * getSpecies()->getGrowthBuildingBlock().getMinPlasticityKVonBertalanffy();


	TimeStep pupaPeriodTime(Day(getGenetics().getBaseIndividualTraits(BaseTraitType::pupaPeriodTime).getPhenotypicValue()), timeStepsPerDay);


	voracitiesContent << this->getId() << "\t"
	<< getSpecies()->getScientificName() << "\t"
	<< EnumClass<LifeStage>::to_string(lifeStage) << "\t"
	<< getGrowthBuildingBlock().getCurrentAge()-TimeStep(1) << "\t"
	<< getGrowthBuildingBlock().getInstar() << "\t"
	<< getGrowthBuildingBlock().isMature() << "\t"
	<< getGrowthBuildingBlock().getCurrentBodySize() << "\t"
	<< getGrowthBuildingBlock().getCurrentEnergyTank() << "\t"
	<< getGrowthBuildingBlock().getCurrentTotalDryMass() << "\t"
	<< getGrowthBuildingBlock().getCurrentTotalWetMass() << "\t"
	<< getGrowthBuildingBlock().getTankAtGrowth() << "\t"
	<< getGrowthBuildingBlock().getNextMassPredicted() << "\t"
	<< maxVoracityTimeStep << "\t"
	<< minMassAtCurrentAge << "\t"
	<< getVoracity() << "\t"
	<< getSearchAreaRadius() << "\t"
	<< getSpeed() << "\t"
	<< getVoracity() << "\t"
	<< getSearchAreaRadius() << "\t"
	<< getSpeed() << "\t"
	<< getGrowthBuildingBlock().getCurrentTotalDryMass()+getVoracity() << "\t"
	<< foodMassEatenCurrentTimeStep << "\t"
	<< getGrowthBuildingBlock().getCurrentTotalDryMass() + foodMassAssimilatedCurrentTimeStep << "\t"
	<< totalMetabolicDryMassLossAfterAssim << "\t"
	<< getSearchAreaRadius() << "\t"
	<< eatenToday << "\t"
	<< steps << "\t"
	<< stepsAttempted << "\t"
	<< getSearchAreaRadius() << "\t"
	<< sated << "\t"
	<< calculateProportionOfTimeWasMoving().to_string() << "\t"
	<< getVoracity() / getGrowthBuildingBlock().getCurrentTotalDryMass().getValue() << "\t"
	<< EnumClass<Gender>::to_string(getGender()) << "\t"
	<< mated << "\t"
	<< getGrowthBuildingBlock().getEggDryMassAtBirth() << "\t"
	<< getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue() << "\t"
	<< getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue() << "\t"
	<< getDateOfDeath() << "\t"
	<< getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime) << "\t"
	<< reproCounter << "\t";

	voracitiesContent << "\n";
}

PreciseDouble AnimalNonStatistical::applyPlasticityDueToConditionToTrait(const PreciseDouble& traitValue, const PreciseDouble& plasticityDueToCondition, const PreciseDouble& h) const
{
	return traitValue*(1-exp(-plasticityDueToCondition*h));
}


void AnimalNonStatistical::printTraits(ostringstream& traitsContent)
{
	traitsContent << getId() << "\t" << getSpecies()->getScientificName();

	for(unsigned char axis = 0; axis < DIMENSIONS; axis++)
	{
		traitsContent << "\t" << getPositionAxisValue(getPosition(), axis);
	}

	traitsContent << "\t" << generationNumberFromFemaleParent 
	<< "\t" << generationNumberFromMaleParent 
	<< "\t" << getIdFromFemaleParent() 
	<< "\t" << getIdFromMaleParent() 
	<< "\t" << getGrowthBuildingBlock().getDateEgg();

	traitsContent << "\t" << getGenetics().printTraits();

	traitsContent << "\n";
}

void AnimalNonStatistical::setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage)
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

		setInstarToEvaluateCells(Instar(1));
		break;
	case LifeStage::UNBORN:
	case LifeStage::ACTIVE:
	case LifeStage::STARVED:
	case LifeStage::PREDATED:
	case LifeStage::PUPA:
	case LifeStage::DIAPAUSE:
	case LifeStage::BACKGROUND:
	case LifeStage::SENESCED:
	case LifeStage::SHOCKED:
    	break;
	default:
		throwLineInfoException("Default case");
		break;
	}

	if(getLifeStage() == LifeStage::ACTIVE && getLifeStage() != newLifeStage)
	{
		landscape->getMutableMap()->decreaseTotalNumberOfActiveAnimals();
	}
	
	if(getLifeStage() != LifeStage::ACTIVE && newLifeStage == LifeStage::ACTIVE)
	{
		landscape->getMutableMap()->increaseTotalNumberOfActiveAnimals();
	}

	if(getLifeStage() == LifeStage::REPRODUCING && getLifeStage() != newLifeStage)
	{
		setInstarToEvaluateCells(getGrowthBuildingBlock().getInstar());
	}


	getMutableTerrainCell()->eraseAnimal(this);

	lifeStage = newLifeStage;

	getMutableTerrainCell()->insertAnimal(landscape, this);
}


void AnimalNonStatistical::setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay)
{
	setNewLifeStage(landscape, newLifeStage);

	switch (newLifeStage)
	{
	case LifeStage::STARVED:
		setDateOfDeath(Day(numberOfTimeSteps, timeStepsPerDay));
		break;
	case LifeStage::PREDATED:
		setDateOfDeath(Day(numberOfTimeSteps, timeStepsPerDay));
		break;
	case LifeStage::BACKGROUND:
		setDateOfDeath(Day(numberOfTimeSteps, timeStepsPerDay));
		break;
	case LifeStage::SENESCED:
		setDateOfDeath(Day(numberOfTimeSteps, timeStepsPerDay));
		break;
	case LifeStage::SHOCKED:
		setDateOfDeath(Day(numberOfTimeSteps, timeStepsPerDay));
		break;
	case LifeStage::UNBORN:
	case LifeStage::ACTIVE:
	case LifeStage::REPRODUCING:
	case LifeStage::PUPA:
	case LifeStage::DIAPAUSE:
		throwLineInfoException("In the new state the animal does not die");
		break;
	default:
		throwLineInfoException("Default case");
		break;
	}
}


void AnimalNonStatistical::setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage, const TimeStep numberOfTimeSteps, EdibleID predatorId, const PreciseDouble& timeStepsPerDay)
{
	setNewLifeStage(landscape, newLifeStage, numberOfTimeSteps, timeStepsPerDay);

	switch (newLifeStage)
	{
	case LifeStage::PREDATED:
		setPredatedByID(predatorId);
		break;
	case LifeStage::UNBORN:
	case LifeStage::ACTIVE:
	case LifeStage::STARVED:
	case LifeStage::REPRODUCING:
	case LifeStage::PUPA:
	case LifeStage::DIAPAUSE:
	case LifeStage::BACKGROUND:
	case LifeStage::SENESCED:
	case LifeStage::SHOCKED:
		throwLineInfoException("In the new state the animal is not depredated");
		break;
	default:
		throwLineInfoException("Default case");
		break;
	}
}

void AnimalNonStatistical::setInBreedingZone(const bool newInBreedingZoneValue)
{
	inBreedingZone = newInBreedingZoneValue;
}

const DryMass& AnimalNonStatistical::getMassesFirstReproduction() const 
{ 
	return getGrowthBuildingBlock().getInstarMass(getSpecies()->getGrowthBuildingBlock().getInstarFirstReproduction()); 
}

const DryMass& AnimalNonStatistical::getMassesLastInstar() const 
{ 
	return getGrowthBuildingBlock().getInstarMassesVector().back(); 
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

bool AnimalNonStatistical::isInBreedingZone() const
{
	return inBreedingZone;
}

bool AnimalNonStatistical::isInHabitatShiftBeforeBreeding() const
{
	return inHabitatShiftBeforeBreeding;
}

bool AnimalNonStatistical::isInHabitatShiftAfterBreeding() const
{
	return inHabitatShiftAfterBreeding;
}

bool AnimalNonStatistical::isAtDestination() const
{
	return atDestination;
}

const std::pair<PointMap, PointContinuous>& AnimalNonStatistical::getTargetNeighborToTravelTo() const
{
	return targetNeighborToTravelTo;
}

void AnimalNonStatistical::setPredatedByID(EdibleID predatorId) 
{ 
	predatedByID = predatorId; 
}

void AnimalNonStatistical::updateDepth(const PreciseDouble& timeStepsPerDay)
{
	TerrainCell* terrainCellToMigrate = getMutableTerrainCell();

	while(static_cast<const PointSpatialTree &>(terrainCellToMigrate->getPosition()).getDepth() != getCellDepthOnActualInstar())
	{
		TerrainCell* newTerrainCellToMigrate = static_cast<SpatialTreeTerrainCell *const>(terrainCellToMigrate)->getMutableParent();
	
		if(newTerrainCellToMigrate == nullptr && getGrowthBuildingBlock().getIndividualGrowthModel()->calculateLength(Day(getGrowthBuildingBlock().getCurrentAge(), timeStepsPerDay), getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue()) > newTerrainCellToMigrate->getSize())
		{
			throwLineInfoException("The animal's dimensions are greater than those of the map");
		}
		
		terrainCellToMigrate = newTerrainCellToMigrate;
	}

	setTerrainCell(terrainCellToMigrate);
}


bool AnimalNonStatistical::checkInitialMovementCellLeaving() const
{
	if(getLifeStage() == LifeStage::ACTIVE && decisions.isLastAssimilationEfficiencyLowerThanMean())
	{
		return true;
	}

	return false;
}


bool AnimalNonStatistical::checkStepCellLeaving() const
{
	if(decisions.isLastCumulativePredationProbabilityUpperThanMean())
	{
		return true;
	}

	return false;
}

ostringstream AnimalNonStatistical::printPreferenceInfo(const Species::ID &preySpeciesId, const Instar &preyInstar) const
{
	ostringstream preferenceInfo;

	preferenceInfo << decisions.getPreference(preySpeciesId, preyInstar) << "\t"
		<< decisions.getMeanExperience(preySpeciesId, preyInstar);

	return preferenceInfo;
}

void AnimalNonStatistical::evaluateExposedAttacks(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, ostringstream& edibilitiesContent, const bool saveAnimalsEachDayPredationProbabilities, std::ostringstream& predationProbabilitiesContent, const bool saveActivity, std::ostringstream& activityContent, const bool competitionAmongResourceSpecies)
{
	vector<pair<PreciseDouble, AnimalNonStatistical*>> predators;


	vector<pair<const AnimalSearchParams&, AnimalFunctions>> animalFunctions;

    animalFunctions.emplace_back(
        getSpecies()->getPredatorSearchParams(getGrowthBuildingBlock().getInstar()),
        AnimalFunctions{
            PreviousAnimalFunctions{},
            IndividualFunctions{
                [this, &predators](Animal& animal) { 
					AnimalNonStatistical* animalCast = static_cast<AnimalNonStatistical*>(&animal);

					const PreciseDouble predationProbability = animalCast->calculatePredationProbability(*this);

					if(predationProbability > 0.0)
					{
						predators.push_back(make_pair<>(predationProbability, animalCast));
					}
				}
            },
            PostAnimalFunctions{}
        }
    );

	vector<pair<const ResourceSearchParams&, ResourceFunctions>> resourceFunctions;


	getMutableTerrainCell()->applyFunctionToEdiblesInRadius(
		getPosition(), getSpecies()->getDecisionsBuildingBlock()->getMaximumPredatorInteractionArea(getGrowthBuildingBlock().getInstar()), 
		animalFunctions, resourceFunctions
	);


	for(const auto& [predationProbability, predator] : predators)
	{
		edibilitiesContent
			<< numberOfTimeSteps << "\t"
			<< predator->getId() << "\t"
			<< predator->getSpecies()->getScientificName() << "\t"
			<< foodMassEatenCurrentTimeStep.getValue() << "\t"
			<< predator->getId() << "\t"
			<< predator->getSpecies()->getScientificName() << "\t"
			<< predator->getGrowthBuildingBlock().getCurrentTotalDryMass() << "\t"
			<< getId() << "\t"
			<< getSpecies()->getScientificName() << "\t"
			<< getGrowthBuildingBlock().getCurrentTotalDryMass() << "\t"
			<< predationProbability << "\t"
			<< 1.0 << "\t"
			<< predator->printPreferenceInfo(getSpecies()->getId(), getGrowthBuildingBlock().getInstar()).str() << endl;
	}


	std::sort(predators.begin(), predators.end(), compareByPredationProbability);

	for(size_t i = 0; i < predators.size() && getLifeStage() != LifeStage::PREDATED; i++)
	{
		const PreciseDouble randomPredationProbability = Random::randomUniform();

		const PreciseDouble predationProbability = predators[i].first;

		list<const AnimalNonStatistical*> exposedAttackAnimalsHasTriedToPredate;

		predators[i].second->predateEdible(randomPredationProbability, predationProbability, false, true, landscape, *this, getGrowthBuildingBlock().getCurrentTotalDryMass(), numberOfTimeSteps, timeStepsPerDay, exposedAttackAnimalsHasTriedToPredate, saveAnimalsEachDayPredationProbabilities, predationProbabilitiesContent, saveActivity, activityContent, competitionAmongResourceSpecies);
	}
}


void AnimalNonStatistical::doInitialCellEvaluation()
{
	searchTargetToTravelTo(getScopeAreaRadius(), list<const AnimalNonStatistical*>());
	setAtDestination(true);
}


void AnimalNonStatistical::moveAnimal(View* view, Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, const bool saveAnimalsEachDayPredationProbabilities, ostringstream& predationProbabilitiesContent, bool, ostringstream& edibilitiesContent, const bool saveActivity, ostringstream& activityContent, const bool saveMovements, ostringstream& movementsContent, const bool competitionAmongResourceSpecies)
{
	// This condition checks if the animal has already been moved in the current timeStep
	if (lastDayMoved < numberOfTimeSteps || firstMovement)
	{
		list<const AnimalNonStatistical*> animalsHasTriedToPredate;

		bool withoutDestinations = false;

		bool completedReproductionInTheArea = false;

		if(getGrowthBuildingBlock().isInHabitatShift() || isInHabitatShiftBeforeBreeding() || isInHabitatShiftAfterBreeding())
		{
			if(getGrowthBuildingBlock().isInHabitatShift() && (isInHabitatShiftBeforeBreeding() || isInHabitatShiftAfterBreeding()))
			{
				getMutableGrowthBuildingBlock().setInHabitatShift(false);
			}


			PreciseDouble currentScopeAreaRadius = getScopeAreaRadius();

			if(getGrowthBuildingBlock().isInHabitatShift())
			{
				setScopeAreaRadius(getScopeAreaRadius() * getSpecies()->getGrowthBuildingBlock().getHabitatShiftFactor());

				getMutableGrowthBuildingBlock().setInHabitatShift(false);
			}
			else if(isInHabitatShiftBeforeBreeding())
			{
				setScopeAreaRadius(getScopeAreaRadius() * getSpecies()->getHabitatShiftBeforeBreedingFactor());

				setInHabitatShiftBeforeBreeding(false);
			}
			else if(isInHabitatShiftAfterBreeding())
			{
				setScopeAreaRadius(getScopeAreaRadius() * getSpecies()->getHabitatShiftAfterBreedingFactor());

				setInHabitatShiftAfterBreeding(false);
			}

				
			searchTargetToTravelTo(getScopeAreaRadius(), animalsHasTriedToPredate);
		

			PointSpatialTree cellPosition(getTargetNeighborToTravelTo().first.getAxisValues(), static_cast<const PointSpatialTree &>(getTerrainCell()->getPosition()).getDepth());

			TerrainCell * targetCell = getMutableTerrainCell()->getCell(cellPosition);


			if(getMutableTerrainCell() != targetCell)
			{
				getMutableTerrainCell()->migrateAnimalTo(landscape, this, targetCell, getTargetNeighborToTravelTo().second);
			}
			else
			{
				setPosition(getTargetNeighborToTravelTo().second);
			}
			
			setAtDestination(true);


			if(isAtDestination() && getLifeStage() == LifeStage::REPRODUCING)
			{
				setInBreedingZone(true);
			}


			setScopeAreaRadius(currentScopeAreaRadius);
		}


		
		if(!isExhausted())
		{
			if(numberOfTimeSteps == TimeStep(0))
			{
				doInitialCellEvaluation();
			}

			if(checkInitialMovementCellLeaving())
			{
				searchTargetToTravelTo(getScopeAreaRadius(), animalsHasTriedToPredate, withoutDestinations);

				if(!withoutDestinations)
				{
					moveOneStep(landscape, saveActivity, activityContent, saveMovements, movementsContent, numberOfTimeSteps, timeStepsPerDay, edibilitiesContent, saveAnimalsEachDayPredationProbabilities, predationProbabilitiesContent, competitionAmongResourceSpecies);
				}
			}
		}



		vector<tuple<PreciseDouble, Edible*, DryMass>> ediblesByEdibility;

		if(getLifeStage() != LifeStage::PREDATED)
		{
			if(!isExhausted())
			{
				// Assimilate leftover food
				assimilateCurrentPrey(saveActivity, activityContent, numberOfTimeSteps, timeStepsPerDay);

				searchAnimalsAndResourceToEat(ediblesByEdibility, numberOfTimeSteps, animalsHasTriedToPredate, edibilitiesContent);
			}

			if(getGrowthBuildingBlock().isMature() && !isMated())
			{
				searchAnimalToBreed(landscape, numberOfTimeSteps, timeStepsPerDay);
			}
		}
		

		while(
				!isExhausted() &&
				(
					lifeStage == LifeStage::ACTIVE ||
					(lifeStage == LifeStage::REPRODUCING && getSpecies()->occursHabitatShiftBeforeBreeding() && !inBreedingZone)
				) && // If it is active
				(
					!sated || // If it is not satisfied, it will have to look for food
					(sated && getGrowthBuildingBlock().isMature() && getGender() == Gender::MALE) || // If it is sated, mature and a male, so it will look for a mate to breed with
					(sated && getGrowthBuildingBlock().isMature() && getGender() == Gender::FEMALE && lifeStage == LifeStage::REPRODUCING && getSpecies()->occursHabitatShiftBeforeBreeding())
				) &&
				(!withoutDestinations)
			)
		{
			if(!completedReproductionInTheArea)
			{
				if(getGrowthBuildingBlock().isMature() && !isMated())
				{
					searchAnimalToBreed(landscape, numberOfTimeSteps, timeStepsPerDay);
				}

				completedReproductionInTheArea = true;
			}
			else
			{
				if(ediblesByEdibility.empty() || isSated())
				{
					if(isAtDestination())
					{
						searchTargetToTravelTo(getScopeAreaRadius(), animalsHasTriedToPredate, withoutDestinations);
					}


					if(!withoutDestinations)
					{
						moveOneStep(landscape, saveActivity, activityContent, saveMovements, movementsContent, numberOfTimeSteps, timeStepsPerDay, edibilitiesContent, saveAnimalsEachDayPredationProbabilities, predationProbabilitiesContent, competitionAmongResourceSpecies);
						ediblesByEdibility.clear();


						if(lifeStage == LifeStage::ACTIVE || lifeStage == LifeStage::REPRODUCING)
						{
							if(isAtDestination() && getLifeStage() == LifeStage::REPRODUCING)
							{
								setInBreedingZone(true);
							}


							if(!isExhausted())
							{
								searchAnimalsAndResourceToEat(ediblesByEdibility, numberOfTimeSteps, animalsHasTriedToPredate, edibilitiesContent);
							}
						}
					}
				}
				else
				{
					decisions.tryToEatEdible(landscape, ediblesByEdibility, numberOfTimeSteps, timeStepsPerDay, animalsHasTriedToPredate, saveAnimalsEachDayPredationProbabilities, predationProbabilitiesContent, saveActivity, activityContent, competitionAmongResourceSpecies);
				
					if(foodMassEatenCurrentTimeStep > getVoracity())
					{
						view->updateLog("The food mass eaten was higher than the voracity value:\n");
						view->updateLog({" - Animal: ", to_string(getId()), "(", getSpecies()->getScientificName(), ")\n"});
						view->updateLog({" - Food mass eaten: ", foodMassEatenCurrentTimeStep.getValue().to_string(), "\n"});
						view->updateLog({" - Voracity value: ", getVoracity().to_string(), "\n"});
					}
				}
			}
		}


		// Update the number of days the animal has been without food
		switch (getHuntingMode()) {
			case HuntingMode::does_not_hunt: {
				break;
			}
			case HuntingMode::sit_and_wait:
			case HuntingMode::grazer: {
				if(foodMassEatenCurrentTimeStep > 0.0)
				{
					timeStepsWithoutFood = TimeStep(0);
				}
				else
				{
					timeStepsWithoutFood = timeStepsWithoutFood + TimeStep(1);
				}
				break;
			}
			default: {
				throwLineInfoException("Default case");
				break;
			}
		}

		// It indicates that the animal has already moved in this timestep.
		lastDayMoved = numberOfTimeSteps;

		if(firstMovement) {
			firstMovement = false;
		}
	}
}

void AnimalNonStatistical::checkBreed(Landscape* const landscape, const TimeStep numberOfTimeSteps, AnimalNonStatistical &otherAnimal, const PreciseDouble& timeStepsPerDay)
{
	// Variables to identify the male and female at mating
	AnimalNonStatistical *matedMale, *matedFemale;

	// Choose the role of each animal according to the sex of the animal doing the searching
	switch(getGender())
	{
		case Gender::MALE:
			matedMale = this;
			matedFemale = &otherAnimal;
			break;
		case Gender::FEMALE:
			matedMale = &otherAnimal;
			matedFemale = this;
			break;
		case Gender::HERMAPHRODITE:
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
	if(matedMale->postBreeding(timeStepsPerDay))
	{
		// Change the status of the male to senesced
		matedMale->getMutableTerrainCell()->changeAnimalToSenesced(landscape, matedMale, numberOfTimeSteps);
	}
}

void AnimalNonStatistical::searchAnimalToBreed(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay)
{
	vector<pair<const AnimalSearchParams&, AnimalFunctions>> animalFunctions;

    animalFunctions.emplace_back(
        getSpecies()->getBreedSearchParams(getGender()),
        AnimalFunctions{
            PreviousAnimalFunctions{},
            IndividualFunctions{
                [this, &landscape, &numberOfTimeSteps, &timeStepsPerDay](Animal& animal) {
					if(getLifeStage() == LifeStage::ACTIVE && !isMated())
					{
						if(!static_cast<AnimalNonStatistical&>(animal).isMated())
						{
							checkBreed(landscape, numberOfTimeSteps, static_cast<AnimalNonStatistical&>(animal), timeStepsPerDay);
						}
					}
				}
            },
            PostAnimalFunctions{}
        }
    );

	vector<pair<const ResourceSearchParams&, ResourceFunctions>> resourceFunctions;


	getMutableTerrainCell()->applyFunctionToEdiblesInRadius(getPosition(), getInteractionAreaRadius(), animalFunctions, resourceFunctions);
}

bool AnimalNonStatistical::isSated() const
{ 
	return sated; 
}

bool AnimalNonStatistical::isExhausted() const 
{ 
	return exhausted; 
}

DryMass AnimalNonStatistical::calculateMassLoad() const
{
	return getGrowthBuildingBlock().calculateMassLoad(getGender(), getReproCounter());
}

const Instar& AnimalNonStatistical::getInstarToEvaluateCells() const
{
	return instarToEvaluateCells;
}

void AnimalNonStatistical::setInstarToEvaluateCells(const Instar& newInstarToEvaluateCells)
{
	instarToEvaluateCells = newInstarToEvaluateCells;
}

void AnimalNonStatistical::searchAnimalsAndResourceToEat(vector<tuple<PreciseDouble, Edible*, DryMass>>& ediblesByEdibility, const TimeStep numberOfTimeSteps, list<const AnimalNonStatistical*> &animalsHasTriedToPredate, ostringstream& edibilitiesContent)
{
	if(!isSated())
	{
		vector<pair<const AnimalSearchParams&, AnimalFunctions>> animalFunctions;

		animalFunctions.emplace_back(
			getSpecies()->getPreySearchParams(getGrowthBuildingBlock().getInstar()).getAnimalSearchParams(),
			AnimalFunctions{
				PreviousAnimalFunctions{},
				IndividualFunctions{
					[this, &ediblesByEdibility, &animalsHasTriedToPredate](Animal& animal) { 
						AnimalNonStatistical* animalCast = static_cast<AnimalNonStatistical*>(&animal);

						const PreciseDouble edibilityValue = calculateEdibilityValue(*animalCast, animalsHasTriedToPredate);

						if(edibilityValue > 0.0)
						{
							ediblesByEdibility.push_back(tuple<PreciseDouble, Edible*, DryMass>(edibilityValue, animalCast, animalCast->getGrowthBuildingBlock().getCurrentTotalDryMass()));
						}
					}
				},
				PostAnimalFunctions{}
			}
		);

		vector<pair<const ResourceSearchParams&, ResourceFunctions>> resourceFunctions;

		resourceFunctions.emplace_back(
			getSpecies()->getPreySearchParams(getGrowthBuildingBlock().getInstar()).getResourceSearchParams(),
			ResourceFunctions{
				[this, &ediblesByEdibility](CellResourceInterface& resource, bool fullCoverage, const PointContinuous* const sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea) {
					const DryMass dryMassAvailable = resource.calculateDryMassAvailable(fullCoverage, sourcePosition, radius, radiusArea);

					const PreciseDouble edibilityValue = calculateEdibilityValue(resource, dryMassAvailable);

					if(edibilityValue > 0.0)
					{
						ediblesByEdibility.push_back(tuple<PreciseDouble, Edible*, DryMass>(edibilityValue, &resource, dryMassAvailable));
					}
				}
			}
		);
		

		getMutableTerrainCell()->applyFunctionToEdiblesInRadius(getPosition(), getInteractionAreaRadius(), animalFunctions, resourceFunctions);
		


		// Sorting the elements by edibility
		std::sort(ediblesByEdibility.begin(), ediblesByEdibility.end(), compareByEdibilityValue);

		// Print interaction information
		for(auto ediblesIt = ediblesByEdibility.begin(); ediblesIt != ediblesByEdibility.end(); ediblesIt++)
		{
			edibilitiesContent
			<< numberOfTimeSteps << "\t"
			<< getId() << "\t"
			<< getSpecies()->getScientificName() << "\t"
			<< foodMassEatenCurrentTimeStep.getValue() << "\t"
			<< getId() << "\t"
			<< getSpecies()->getScientificName() << "\t"
			<< getGrowthBuildingBlock().getCurrentTotalDryMass() << "\t"
			<< get<1>(*ediblesIt)->getId() << "\t"
			<< get<1>(*ediblesIt)->getSpecies()->getScientificName() << "\t"
			<< get<2>(*ediblesIt) << "\t"
			<< decisions.calculatePredationProbability(*get<1>(*ediblesIt), get<2>(*ediblesIt)) << "\t"
			<< get<0>(*ediblesIt) << "\t"
			<< printPreferenceInfo(get<1>(*ediblesIt)->getSpecies()->getId(), get<1>(*ediblesIt)->getGrowthBuildingBlock().getInstar()).str() << endl;
		}
	}
}

void AnimalNonStatistical::setGenomeFromMatedMale(const AnimalNonStatistical* matedMale)
{
	mated = true;
	if(genomeFromMatedMale != nullptr) delete genomeFromMatedMale;
	genomeFromMatedMale = matedMale->getGenetics().cloneGenome();		
	idFromMatedMale = matedMale->getId();
	generationNumberFromMatedMale = matedMale->getGenerationNumberFromMaleParent();
}

void AnimalNonStatistical::doDefinitive(Landscape* const landscape, const bool saveGenetics, const bool saveMassInfo) { 
	id = landscape->generateEdibleId();

	landscape->generateAnimalId();
	
	setInfoMassFileHeader(landscape->getResultFolder(), saveMassInfo);

	if(saveGenetics)
	{
		ostringstream animalInfo;

		animalInfo << getId() << "\t"
			<< getSpecies()->getScientificName() << "\t"
			<< generationNumberFromFemaleParent << "\t"
			<< generationNumberFromMaleParent << "\t"
			<< getIdFromFemaleParent() << "\t"
			<< getIdFromMaleParent();

		getMutableSpecies()->getMutableGenetics().printGenetics(animalInfo, getGenetics().getGenome());
	}

	getMutableGenetics().deleteHomologousCorrelosomes();

	if(getLifeStage() == LifeStage::ACTIVE)
	{
		landscape->getMutableMap()->increaseTotalNumberOfActiveAnimals();
	}


	landscape->registerAnimal(this);
}

bool AnimalNonStatistical::postBreeding(const PreciseDouble& timeStepsPerDay)
{
	reproCounter++;

	setAgeOfLastMoultOrReproduction(getGrowthBuildingBlock().getCurrentAge());

	if(reproCounter == 1)
	{
		ageOfFirstReproduction = getGrowthBuildingBlock().getCurrentAge();
	}

	if(reproCounter >= getMaxReproductionEvents())
	{
		return true;
	}
	else
	{
		if(getGender() == Gender::FEMALE)
		{	
			TimeStep pupaPeriodTime(Day(getGenetics().getBaseIndividualTraits(BaseTraitType::pupaPeriodTime).getPhenotypicValue()), timeStepsPerDay);

			getMutableGrowthBuildingBlock().calculateMatureNextReproductionTargets(pupaPeriodTime, getSpecies()->getFemaleMaxReproductionEvents(), getReproCounter(), getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue(), getSpecies()->getTotFec(), true, timeStepsPerDay);
		}

		return false;
	}
}

const TimeStep AnimalNonStatistical::getAgeOfLastMoultOrReproduction() const
{
	return ageOfLastMoultOrReproduction;
}

void AnimalNonStatistical::setAgeOfLastMoultOrReproduction(const TimeStep newAgeOfLastMoultOrReproduction)
{
	ageOfLastMoultOrReproduction = newAgeOfLastMoultOrReproduction;
}

void AnimalNonStatistical::breedAsexually(list<AnimalNonStatistical*>& offspring, View* view, Landscape* const landscape, unsigned int objectiveEggsNumber, const TimeStep numberOfTimeSteps, const bool saveGenetics, const bool saveMassInfo, const PreciseDouble& timeStepsPerDay)
{
	DryMass offspringDryMass = getSpecies()->getGrowthBuildingBlock().getEggDryMass() + getSpecies()->getGrowthBuildingBlock().getEggDryMass().getValue()*getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue();

	DryMass totalOffspringDryMass(0.0);

	//Asexual animals are always females (or treated as females) and they DO NOT perform meiosis.
	Gender offSpringGender = Gender::FEMALE;
	unsigned int createdEggsNumber = 0;

	while( createdEggsNumber < objectiveEggsNumber && getGrowthBuildingBlock().getCurrentEnergyTank() > (totalOffspringDryMass+offspringDryMass) )
	{
		createdEggsNumber++;

		if(getGenetics().getBaseIndividualTraits(BaseTraitType::eggFertility).getPhenotypicValue() > Random::randomUniform())
		{
			Gamete* femaleGameteSelected = getGenetics().getGenome().cloneFirstGameteFromHaploid();
			Gamete* maleGameteSelected = getGenetics().getGenome().cloneSecondGameteFromHaploid();

			Generation generationFemale(generationNumberFromFemaleParent.getValue() + 1);
			Generation generationMale;

			EdibleID idFemale = getId();
			EdibleID idMale;

			AnimalNonStatistical* newOffspring = createOffspring(femaleGameteSelected, maleGameteSelected, getMutableTerrainCell(), getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue(), generationFemale, generationMale, idFemale, idMale, getMutableSpecies(), offSpringGender, numberOfTimeSteps, timeStepsPerDay);

			newOffspring->getMutableGrowthBuildingBlock().setDryMass(
				newOffspring->getGrowthBuildingBlock().getEggDryMassAtBirth(), newOffspring->getGrowthBuildingBlock().getEggDryMassAtBirth(), 
				newOffspring->getGenetics().getBaseIndividualTraits(BaseTraitType::energy_tank).getPhenotypicValue(), false, newOffspring->getGender(), false, numberOfTimeSteps
			);

			offspring.push_back(newOffspring);

			newOffspring->doDefinitive(landscape, saveGenetics, saveMassInfo);

			totalOffspringDryMass = totalOffspringDryMass + newOffspring->getGrowthBuildingBlock().getEggDryMassAtBirth();

			delete femaleGameteSelected;
			delete maleGameteSelected;
		}
		else
		{
			totalOffspringDryMass = totalOffspringDryMass + offspringDryMass;
		}
	}

	if(createdEggsNumber != objectiveEggsNumber)
	{
		view->updateLog({" - Animal: ", to_string(getId()), ": Partial breed ", to_string(createdEggsNumber), "/", to_string(objectiveEggsNumber), ".\n"});
	}

	
		
	fecundity += static_cast<unsigned int>(offspring.size());

	getMutableGrowthBuildingBlock().modifyEnergyTank(getGrowthBuildingBlock().getCurrentEnergyTank() - totalOffspringDryMass, numberOfTimeSteps);

	if(postBreeding(timeStepsPerDay))
	{
		getMutableTerrainCell()->changeAnimalToSenesced(landscape, this, numberOfTimeSteps);
	}
	else
	{
		setNewLifeStage(landscape, LifeStage::ACTIVE);
	}
}


void AnimalNonStatistical::breedSexually(list<AnimalNonStatistical*>& offspring, View* view, Landscape* const landscape, unsigned int objectiveEggsNumber, const TimeStep numberOfTimeSteps, const bool saveGenetics, const bool saveMassInfo, const PreciseDouble& timeStepsPerDay)
{
	DryMass offspringDryMass = getSpecies()->getGrowthBuildingBlock().getEggDryMass() + getSpecies()->getGrowthBuildingBlock().getEggDryMass().getValue()*getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue();

	DryMass totalOffspringDryMass(0.0);

	Gender offSpringGender = Gender::FEMALE;
	unsigned int createdEggsNumber = 0;

	Generation generationFemale;
	Generation generationMale;

	EdibleID idFemale;
	EdibleID idMale;

	while( createdEggsNumber < objectiveEggsNumber && getGrowthBuildingBlock().getCurrentEnergyTank() > (totalOffspringDryMass+offspringDryMass) )
	{
		createdEggsNumber++;  // DUDA: Debe hacerse antes o sólo si se crea el offspring?
		
		if(getGenetics().getBaseIndividualTraits(BaseTraitType::eggFertility).getPhenotypicValue() > Random::randomUniform())
		{
			Gamete* femaleGameteSelected = getMutableGenetics().getMutableGenome().getRandomGameteFromMeiosis();
			Gamete* maleGameteSelected = NULL;
			
			if(getSpecies()->getSexualType() == SexualType::diploid)
			{
				maleGameteSelected = genomeFromMatedMale->getRandomGameteFromMeiosis();
				//gender here depends on the species sexRatio
				offSpringGender = getSpecies()->getRandomGender();

				generationFemale = Generation(generationNumberFromFemaleParent.getValue() + 1); 
				generationMale = Generation(generationNumberFromMatedMale.getValue() + 1);	

				idFemale = getId();
				idMale = idFromMatedMale;
			}
			else if(getSpecies()->getSexualType() == SexualType::haplodiploid)
			{
				if(isMated())
				{
					//Gender here depends on the species sexRatio
					offSpringGender = getSpecies()->getRandomGender();
					if(offSpringGender == Gender::MALE)
					{
						//Males are haploid so they will use only the female gamete (duplicated for simplicity, in reality they have only one)
						maleGameteSelected = new Gamete(getSpecies()->getGenetics().getNumberOfChromosomes());
						for(unsigned int i = 0; i < femaleGameteSelected->size(); ++i)
						{
							maleGameteSelected->pushChromosome(femaleGameteSelected->getChromosome(i)->clone());
						}

						generationFemale = Generation(generationNumberFromFemaleParent.getValue() + 1); 

						idFemale = getId();
					}
					else
					{
						//Females are still diploid, but males contribute with their genes as they are (no meiosis involved here)
						maleGameteSelected = genomeFromMatedMale->cloneFirstGameteFromHaploid();

						generationFemale = Generation(generationNumberFromFemaleParent.getValue() + 1); 
						generationMale = Generation(generationNumberFromMatedMale.getValue() + 1);	

						idFemale = getId();
						idMale = idFromMatedMale;
					}
				}
				else //!isMated()
				{
					//Males are haploid so they will use only the female gamete (duplicated for simplicity, in reality they have only one)
					maleGameteSelected = new Gamete(getSpecies()->getGenetics().getNumberOfChromosomes());
					for(unsigned int i = 0; i < femaleGameteSelected->size(); ++i)
					{
						maleGameteSelected->pushChromosome(femaleGameteSelected->getChromosome(i)->clone());
					}
					//Gender here is always MALE
					offSpringGender = Gender::MALE;

					generationFemale = Generation(generationNumberFromFemaleParent.getValue() + 1); 	

					idFemale = getId();
				}
			}

			AnimalNonStatistical* newOffspring = createOffspring(femaleGameteSelected, maleGameteSelected, getMutableTerrainCell(), getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue(), generationFemale, generationMale, idFemale, idMale, getMutableSpecies(), offSpringGender, numberOfTimeSteps, timeStepsPerDay);


			newOffspring->getMutableGrowthBuildingBlock().setDryMass(
				newOffspring->getGrowthBuildingBlock().getEggDryMassAtBirth(), newOffspring->getGrowthBuildingBlock().getEggDryMassAtBirth(), 
				newOffspring->getGenetics().getBaseIndividualTraits(BaseTraitType::energy_tank).getPhenotypicValue(), false, newOffspring->getGender(), false, numberOfTimeSteps
			);
		
			offspring.push_back(newOffspring);
			
			newOffspring->doDefinitive(landscape, saveGenetics, saveMassInfo);

			totalOffspringDryMass = totalOffspringDryMass + newOffspring->getGrowthBuildingBlock().getEggDryMassAtBirth();

			delete femaleGameteSelected;
			delete maleGameteSelected;
		}
		else
		{
			totalOffspringDryMass = totalOffspringDryMass + offspringDryMass;
		}
	}

	if(createdEggsNumber != objectiveEggsNumber)
	{
		view->updateLog({" - Animal: ", to_string(getId()), ": Partial breed ", to_string(createdEggsNumber), "/", to_string(objectiveEggsNumber), ".\n"});
	}



	fecundity += static_cast<unsigned int>(offspring.size());

	getMutableGrowthBuildingBlock().modifyEnergyTank(getGrowthBuildingBlock().getCurrentEnergyTank() - totalOffspringDryMass, numberOfTimeSteps);

	if(postBreeding(timeStepsPerDay))
	{
		getMutableTerrainCell()->changeAnimalToSenesced(landscape, this, numberOfTimeSteps);
	}
	else
	{
		setNewLifeStage(landscape, LifeStage::ACTIVE);
	}
}

void AnimalNonStatistical::setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth)
{
	growthBuildingBlock.setSpeciesGrowth(newSpeciesGrowth);
}

void AnimalNonStatistical::isReadyToResumeFromPupaOrDecreasePupaTimer(Landscape* const landscape)
{
	if(getMutableGrowthBuildingBlock().isReadyToResumeFromPupaOrDecreasePupaTimer())
	{
		setNewLifeStage(landscape, LifeStage::ACTIVE);
	}
}

void AnimalNonStatistical::isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(Landscape* const landscape)
{
	auto result = getMutableGrowthBuildingBlock().isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(
		getTerrainCell()->getPatchApplicator().getCellMoisture().getMoisture(), 
		getSpecies()->getMinRelativeHumidityThreshold(getGrowthBuildingBlock().getInstar())
	);

	if(result.first)
	{
		if(result.second)
		{
			setNewLifeStage(landscape, LifeStage::PUPA);
		}
		else
		{
			setNewLifeStage(landscape, LifeStage::ACTIVE);
		}
	}
}

void AnimalNonStatistical::calculateGrowthModel(const PreciseDouble& timeStepsPerDay)
{
	getMutableGrowthBuildingBlock().calculateGrowthModel(
		getGenetics().getBaseIndividualTraits(), getTerrainCell()->getPatchApplicator().getCellMoisture().getTemperature(), 
		getSpecies()->getTempFromLab(), timeStepsPerDay
	);
}

void AnimalNonStatistical::calculateGrowthCurves(const PreciseDouble& timeStepsPerDay)
{
	getMutableGrowthBuildingBlock().calculateGrowthCurves(getGenetics().getBaseIndividualTraits(), getSpecies()->getFemaleMaxReproductionEvents(), timeStepsPerDay);
}

void AnimalNonStatistical::forceMolting(Landscape* const landscape, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
	auto result = getMutableGrowthBuildingBlock().forceMolting(getGenetics().getBaseIndividualTraits(), actualTimeStep, getGender(), timeStepsPerDay);
	
	setAgeOfLastMoultOrReproduction(result.first);

	if(result.second.first)
	{
		setNewLifeStage(landscape, result.second.second);
	}

	if(getGrowthBuildingBlock().isMature() && getGender() == Gender::FEMALE)
	{
		forceReproduction(actualTimeStep, timeStepsPerDay);

		if(getReproCounter() >= getSpecies()->getFemaleMaxReproductionEvents())
		{
			setNewLifeStage(landscape, LifeStage::SENESCED, actualTimeStep, timeStepsPerDay);
		}
	}
}

void AnimalNonStatistical::forceReproduction(const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
	TimeStep timeOfReproEvent(0);

	TimeStep pupaPeriodTime(Day(getGenetics().getBaseIndividualTraits(BaseTraitType::pupaPeriodTime).getPhenotypicValue()), timeStepsPerDay);

	// reproCounter
	if(getGrowthBuildingBlock().getCurrentAge() >= getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime))
	{
		if(getGrowthBuildingBlock().hasCapitalBreeding())
		{
			TimeStep totalTimeBreedingCapitally = getSpecies()->getGrowthBuildingBlock().getTimeOfReproEventDuringCapitalBreeding() * (getSpecies()->getGrowthBuildingBlock().getNumberOfCapitalBreeds()-1);

			if(getSpecies()->getFemaleMaxReproductionEvents() == getSpecies()->getGrowthBuildingBlock().getNumberOfCapitalBreeds()) {
				timeOfReproEvent = TimeStep(0);
			}
			else {
				timeOfReproEvent = (getGrowthBuildingBlock().getLongevity() - totalTimeBreedingCapitally - getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime) - TimeStep(2*getSpecies()->getFemaleMaxReproductionEvents())) / (getSpecies()->getFemaleMaxReproductionEvents() - getSpecies()->getGrowthBuildingBlock().getNumberOfCapitalBreeds());
			}

			
			if(getSpecies()->getFemaleMaxReproductionEvents() > 1)
			{
				if(getGrowthBuildingBlock().getCurrentAge() < getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime) + totalTimeBreedingCapitally)
				{
					reproCounter += ((getGrowthBuildingBlock().getCurrentAge() - getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime)) / getSpecies()->getGrowthBuildingBlock().getTimeOfReproEventDuringCapitalBreeding()).getValue() + 1;
				}
				else
				{
					unsigned int reproCounterPostCapitalBreeding = ((getGrowthBuildingBlock().getCurrentAge() - totalTimeBreedingCapitally - getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime)) / timeOfReproEvent).getValue();

					reproCounter += reproCounterPostCapitalBreeding;
				}
			}
		}
		else
		{
			timeOfReproEvent = (getGrowthBuildingBlock().getLongevity() - getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime) - TimeStep(2*getSpecies()->getFemaleMaxReproductionEvents())) / getSpecies()->getFemaleMaxReproductionEvents();
			
			reproCounter += ((getGrowthBuildingBlock().getCurrentAge() - getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime)) / timeOfReproEvent).getValue();
		}
	}

	if(timeOfReproEvent < TimeStep(0)){
		throwLineInfoException("'timeOfReproEvent' must have a positive value.");
	}

	// setAgeOfLastMoultOrReproduction

	bool lastEventIsMoult = true;

	TimeStep newAgeOfLastMoultOrReproduction(0);

	if(getReproCounter() > 0)
	{
		if(getGrowthBuildingBlock().hasCapitalBreeding())
		{
			if(getReproCounter() < getSpecies()->getGrowthBuildingBlock().getNumberOfCapitalBreeds())
			{
				newAgeOfLastMoultOrReproduction = getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime) + getSpecies()->getGrowthBuildingBlock().getTimeOfReproEventDuringCapitalBreeding() * (getReproCounter()-1);
			}
			else
			{
				TimeStep totalTimeBreedingCapitally = getSpecies()->getGrowthBuildingBlock().getTimeOfReproEventDuringCapitalBreeding() * (getSpecies()->getGrowthBuildingBlock().getNumberOfCapitalBreeds()-1);
				
				newAgeOfLastMoultOrReproduction = getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime) + totalTimeBreedingCapitally + timeOfReproEvent * (getReproCounter()-getSpecies()->getGrowthBuildingBlock().getNumberOfCapitalBreeds());
			}
		}
		else
		{
			newAgeOfLastMoultOrReproduction = getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime) + timeOfReproEvent * getReproCounter();
		}
	}

	if(newAgeOfLastMoultOrReproduction > getAgeOfLastMoultOrReproduction())
	{
		setAgeOfLastMoultOrReproduction(newAgeOfLastMoultOrReproduction);	

		lastEventIsMoult = false;
	}

	getMutableGrowthBuildingBlock().forceReproduction(getReproCounter(), getSpecies()->getFemaleMaxReproductionEvents(), timeOfReproEvent, getAgeOfLastMoultOrReproduction(), pupaPeriodTime, getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue(), actualTimeStep, getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue(), getSpecies()->getTotFec(), lastEventIsMoult, timeStepsPerDay);
}

void AnimalNonStatistical::grow(Landscape* const landscape, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
	TimeStep pupaPeriodTime(Day(getGenetics().getBaseIndividualTraits(BaseTraitType::pupaPeriodTime).getPhenotypicValue()), timeStepsPerDay);

	auto result = getMutableGrowthBuildingBlock().grow(landscape, this, getGender(), pupaPeriodTime, getReproCounter(), getSpecies()->getFemaleMaxReproductionEvents(), getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue(), getSpecies()->getTotFec(), getSpecies()->getSexualType(), isMated(), actualTimeStep, getGenetics().getBaseIndividualTraits(BaseTraitType::energy_tank).getPhenotypicValue(), timeStepsPerDay);

	if(result.first.first)
	{
		setAgeOfLastMoultOrReproduction(result.first.second);
	}

	if(result.second.first)
	{
		setNewLifeStage(landscape, result.second.second);
	}
}

void AnimalNonStatistical::breed(list<AnimalNonStatistical*>& offspring, View* view, Landscape* const landscape, const TimeStep numberOfTimeSteps, const bool saveGenetics, const bool saveMassInfo, const PreciseDouble& timeStepsPerDay)
{
	unsigned int currentEggBatchNumber = getGrowthBuildingBlock().computeEggBatchNumber(getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue());

	switch (getSpecies()->getSexualType())
	{
		case SexualType::asexual:
			breedAsexually(offspring, view, landscape, currentEggBatchNumber, numberOfTimeSteps, saveGenetics, saveMassInfo, timeStepsPerDay);
			break;
		case SexualType::diploid:
			breedSexually(offspring, view, landscape, currentEggBatchNumber, numberOfTimeSteps, saveGenetics, saveMassInfo, timeStepsPerDay);
			break;
		case SexualType::haplodiploid:
			breedSexually(offspring, view, landscape, currentEggBatchNumber, numberOfTimeSteps, saveGenetics, saveMassInfo, timeStepsPerDay);
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


void AnimalNonStatistical::dieFromBackground(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, const bool growthAndReproTest)
{
	if(!growthAndReproTest) {
		if(getSpecies()->getProbabilityDeathFromBackground(getGrowthBuildingBlock().getInstar()) > Random::randomUniform())
		{
			setNewLifeStage(landscape, LifeStage::BACKGROUND, numberOfTimeSteps, timeStepsPerDay);
		}
	}
}


void AnimalNonStatistical::assimilateFoodMass(const DryMass& foodMass)
{
	DryMass assimilatedMass = calculateAssimilatedMass(foodMass, getCurrentPrey().getId(), getCurrentPrey().getInstar());

	foodMassAssimilatedCurrentTimeStepPerSpecies.at(getCurrentPrey().getId()).at(getCurrentPrey().getInstar()) += assimilatedMass;
	foodMassAssimilatedCurrentTimeStep += assimilatedMass;

	foodMassEatenCurrentTimeStepPerSpecies.at(getCurrentPrey().getId()).at(getCurrentPrey().getInstar()) += foodMass;
	foodMassEatenCurrentTimeStep += foodMass;
		
	sated = getRemainingVoracity() <= 0.0;
}


unsigned int AnimalNonStatistical::getMaxReproductionEvents() const
{
	if(getGender() == Gender::FEMALE)
	{
		return getSpecies()->getFemaleMaxReproductionEvents();
	}
	else
	{
		return getSpecies()->getMaleMaxReproductionEvents();
	}
}


const size_t& AnimalNonStatistical::getTotalPredationEncounters() const
{
	return totalPredationEncounters;
}

DryMass AnimalNonStatistical::calculateAssimilatedMass(const DryMass& nonAssimilatedMass, const Species::ID& preySpeciesId, const Instar& preyInstar) const
{
	PreciseDouble profitability = getSpecies()->getEdibleProfitability(preySpeciesId, getGrowthBuildingBlock().getInstar(), preyInstar);

	return DryMass(nonAssimilatedMass.getValue() * (profitability + getGenetics().getBaseIndividualTraits(BaseTraitType::assim).getPhenotypicValue()));
}

DryMass AnimalNonStatistical::turnIntoDryMass(const DryMass &targetDryMass, const PreciseDouble&) const
{
	return targetDryMass;
}

const Prey& AnimalNonStatistical::getCurrentPrey() const
{
	return currentPrey;
}

void AnimalNonStatistical::setCurrentPrey(Edible& prey, Landscape* const landscape, const DryMass &targetDryMass, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, const bool competitionAmongResourceSpecies)
{
	DryMass preyDryMass = prey.turnIntoDryMass(targetDryMass, getRemainingVoracity());


	if(prey.getSpecies()->isMobile())
	{
        static_cast<Animal&>(prey).setNewLifeStage(landscape, LifeStage::PREDATED, numberOfTimeSteps, getId(), timeStepsPerDay);
	}
	else
	{
        static_cast<CellResourceInterface&>(prey).substractBiomass(preyDryMass, getPosition(), getInteractionAreaRadius(), competitionAmongResourceSpecies);
	}


	currentPrey = Prey(prey.getSpecies()->getId(), prey.getGrowthBuildingBlock().getInstar(), preyDryMass, computeHandlingTime(preyDryMass, timeStepsPerDay), timeStepsPerDay);


	getMutableSpecies()->addPredationEventOnOtherSpecies(prey.getSpecies()->getId());
	eatenToday++;
}

void AnimalNonStatistical::predateEdible(const PreciseDouble& randomProbability, const PreciseDouble& probabilityToCompare, const bool retaliation, const bool exposedAttack, Landscape* const landscape, Edible &prey, const DryMass &targetDryMass, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, list<const AnimalNonStatistical*> &animalsHasTriedToPredate, const bool saveAnimalsEachDayPredationProbabilities, ostringstream& predationProbabilitiesContent, const bool saveActivity, ostringstream& activityContent, const bool competitionAmongResourceSpecies)
{
	if(saveAnimalsEachDayPredationProbabilities)
	{
		predationProbabilitiesContent << randomProbability << "\t"
			<< probabilityToCompare << "\t"
			<< ((retaliation) ? 1 : 0) << "\t"
			<< ((exposedAttack) ? 1 : 0) << "\t"
			<< getId() << "\t"
			<< prey.getId() << "\t"
			<< getSpecies()->getScientificName() << "\t"
			<< prey.getSpecies()->getScientificName() << "\t"
			<< ((prey.isHunting()) ? 1 : 0) << "\t"
			<< getGrowthBuildingBlock().getCurrentTotalDryMass() << "\t"
			<< targetDryMass << "\t";
	}


	if(prey.getSpecies()->isMobile())
	{
		animalsHasTriedToPredate.push_back(&static_cast<AnimalNonStatistical&>(prey));
	}


	prey.increasePredationEncounters();


	if(probabilityToCompare >= randomProbability)
	{
		if(!retaliation && !exposedAttack)
		{
			decisions.addToCumulativePredationProbability(decisions.calculatePredationProbability(prey, targetDryMass));
		}
		else
		{
			decisions.addToCumulativePredationProbability(probabilityToCompare);
		}


		if(saveAnimalsEachDayPredationProbabilities)
		{
			predationProbabilitiesContent << 1 << "\n";
		}


		setCurrentPrey(prey, landscape, targetDryMass, numberOfTimeSteps, timeStepsPerDay, competitionAmongResourceSpecies);

		assimilateCurrentPrey(saveActivity, activityContent, numberOfTimeSteps, timeStepsPerDay);
	}
	else
	{
		if(saveAnimalsEachDayPredationProbabilities)
		{
			predationProbabilitiesContent << 0 << "\n";
		}


		if(!retaliation && !exposedAttack && prey.getSpecies()->isMobile())
		{
			const PreciseDouble predationProbability = static_cast<AnimalNonStatistical&>(prey).calculatePredationProbability(*this);

			if(predationProbability > 0.0)
			{
				const PreciseDouble randomPredationProbability = Random::randomUniform();

				list<const AnimalNonStatistical*> retaliationAnimalsHasTriedToPredate;

				static_cast<AnimalNonStatistical&>(prey).predateEdible(randomPredationProbability, predationProbability, true, false, landscape, *this, getGrowthBuildingBlock().getCurrentTotalDryMass(), numberOfTimeSteps, timeStepsPerDay, retaliationAnimalsHasTriedToPredate, saveAnimalsEachDayPredationProbabilities, predationProbabilitiesContent, saveActivity, activityContent, competitionAmongResourceSpecies);
			}
		}
	}
}

void AnimalNonStatistical::resetControlVariables(const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay)
{
	decisions.setLastIngestion(
		foodMassAssimilatedCurrentTimeStepPerSpecies, foodMassAssimilatedCurrentTimeStep, 
		foodMassEatenCurrentTimeStepPerSpecies, foodMassEatenCurrentTimeStep
	);
	decisions.setLastCumulativePredationProbability();



	for(size_t speciesId = 0; speciesId < foodMassAssimilatedCurrentTimeStepPerSpecies.size(); speciesId++)
	{
		for(size_t preyInstar = 0; preyInstar < foodMassAssimilatedCurrentTimeStepPerSpecies.getValue(speciesId).size(); preyInstar++)
		{
			foodMassAssimilatedCurrentTimeStepPerSpecies.getValue(speciesId).getValue(preyInstar) = DryMass(0.0);
			foodMassEatenCurrentTimeStepPerSpecies.getValue(speciesId).getValue(preyInstar) = DryMass(0.0);
		}
	}

	foodMassAssimilatedCurrentTimeStep = DryMass(0.0);
	foodMassEatenCurrentTimeStep = DryMass(0.0);


	if(timeStepsPerDay > 1.0)
	{
		predationEncountersCurrentDay = 0u;
	}
	else if(numberOfTimeSteps % TimeStep(Day(1), timeStepsPerDay) == 0)
	{
		predationEncountersCurrentDay = 0u;
	}


	eatenToday = 0;
	steps = 0.0;
	stepsAttempted = 0;
	exhausted = false;


	voracity = 0.0;
	sated = false;
}


void AnimalNonStatistical::assimilateCurrentPrey(const bool saveActivity, ostringstream& activityContent, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
	if(getCurrentPrey().isThereLeftoverFood() && !isSated())
	{
		DryMass foodMass = computeHandlingFoodMass();
		

		currentPrey.decreaseFoodMass(foodMass);


		if(getSpecies()->getActivatedHandling())
		{
			applyHandlingTime(foodMass, saveActivity, activityContent, actualTimeStep, timeStepsPerDay);
		}


		assimilateFoodMass(foodMass);


		if(!getSpecies()->getPreserveLeftovers() || !currentPrey.isThereLeftoverFood()) 
		{
			currentPrey = Prey();
		}
	}
}

DryMass AnimalNonStatistical::computeHandlingFoodMass() const
{
	if(getSpecies()->getActivatedHandling())
	{
		return DryMass(fmin((steps / getSearchAreaRadius()) * currentPrey.getFoodDryMassPerTimeStep().getValue(), getRemainingVoracity()));
	}
	else
	{
		return DryMass(fmin(getCurrentPrey().getFoodDryMass().getValue(), getRemainingVoracity()));
	}
}

void AnimalNonStatistical::applyHandlingTime(const DryMass& foodMass, const bool saveActivity, ostringstream& activityContent, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
	Day initialDay = Day(actualTimeStep, timeStepsPerDay) + Day((steps / getSearchAreaRadius()) * timeStepsPerDay);

	PreciseDouble distanceToAdd = (foodMass.getValue() / currentPrey.getFoodDryMassPerTimeStep().getValue()) * getSearchAreaRadius();

	increaseSteps(distanceToAdd);

	Day finalDay = Day(actualTimeStep, timeStepsPerDay) + Day((steps / getSearchAreaRadius()) * timeStepsPerDay);

	if(saveActivity)
	{
		activityContent << getId() << "\t"
			<< getSpecies()->getScientificName() << "\t"
			<< "Handling" << "\t"
			<< initialDay << "\t"
			<< finalDay << "\t"
			<< (finalDay-initialDay) << "\n";
	}
}

void AnimalNonStatistical::increaseSteps(const PreciseDouble& stepsToAdd)
{
	steps += stepsToAdd;

	exhausted = (steps >= getSearchAreaRadius());
}

Day AnimalNonStatistical::computeHandlingTime(const DryMass& preyDryMass, const PreciseDouble& timeStepsPerDay) const
{
	if(getSpecies()->getActivatedHandling())
	{
		PreciseDouble kelvinTemperature = getTerrainCell()->getPatchApplicator().getCellMoisture().getTemperature().getTemperatureKelvin();

		PreciseDouble log_ratio = calculateLogMassRatio(preyDryMass);
		PreciseDouble lnHandlingTime = -1814 + 0.7261*log_ratio + 12.04*kelvinTemperature + (-0.02006)*pow(kelvinTemperature, 2);

		return Day((1.0/exp(lnHandlingTime)) / (60 * 60 * 24));
	}
	else
	{
		return Day(timeStepsPerDay);
	}
}


const PreciseDouble& AnimalNonStatistical::getVoracity() const 
{ 
	return voracity; 
}

const PreciseDouble& AnimalNonStatistical::getSpeed() const
{
	return speed;
}

const PreciseDouble& AnimalNonStatistical::getScopeAreaRadius() const
{
	return scopeAreaRadius;
}

const PreciseDouble& AnimalNonStatistical::getSearchAreaRadius() const
{
	return searchAreaRadius;
}

const PreciseDouble& AnimalNonStatistical::getInteractionAreaRadius() const
{
	return interactionAreaRadius;
}


void AnimalNonStatistical::setInfoMassFileHeader(const fs::path& resultFolder, const bool saveMassInfo) const
{
	if(saveMassInfo)
	{
		std::ofstream infoMassFile;

		createOutputFile(infoMassFile, resultFolder / fs::path("massInfo") / fs::path("animal_" + string(getId()) + ".txt"));

		infoMassFile << "TimeStep\tCurrentMass\tMoltingMassTarget\tReproductionMassTarget\tGrowthCurve\tMassPredicted\tCurrentAge\tMoltingAgeTarget\tReproductionAgeTarget\n";

		infoMassFile.close();
	}
}

const PreciseDouble& AnimalNonStatistical::getPreference(const Species::ID &preySpeciesId, const Instar &preyInstar) const
{
	return decisions.getPreference(preySpeciesId, preyInstar);
}

void AnimalNonStatistical::updateVariablesAssociatedWithInstar()
{
	decisions.updateVariablesAssociatedWithInstar();
}

void AnimalNonStatistical::setMaximumCellEvaluation(const PreciseDouble& totalEdibilityValue, const PreciseDouble& totalPredatoryRiskEdibilityValue, const PreciseDouble& totalConspecificBiomass)
{
	decisions.setMaximumPatchEdibilityValueIndividual(totalEdibilityValue);
    decisions.setMaximumPatchPredationRiskIndividual(totalPredatoryRiskEdibilityValue);
    decisions.setMaximumPatchConspecificBiomassIndividual(totalConspecificBiomass);
}

PreciseDouble AnimalNonStatistical::getMaximumPatchEdibilityValue() const
{
	return decisions.getMaximumPatchEdibilityValue();
}

PreciseDouble AnimalNonStatistical::getMaximumPatchPredationRisk() const
{
	return decisions.getMaximumPatchPredationRisk();
}

PreciseDouble AnimalNonStatistical::getMaximumPatchConspecificBiomass() const
{
	return decisions.getMaximumPatchConspecificBiomass();
}

PreciseDouble AnimalNonStatistical::calculatePredationRisk(AnimalNonStatistical& predator) const
{
	return decisions.calculatePredationRisk(predator);
}

PreciseDouble AnimalNonStatistical::calculatePredationProbability(const AnimalNonStatistical& prey)
{
	if(getId() != prey.getId())
	{
		if(!isSated() && !isExhausted())
		{
			return decisions.calculatePredationProbability(prey);
		}
	}

	return 0.0;
}

DryMass AnimalNonStatistical::calculateConspecificBiomass(const AnimalNonStatistical& otherAnimal) const
{
	if(getId() != otherAnimal.getId())
	{
		return decisions.calculateConspecificBiomass(otherAnimal);
	}
	else
	{
		return DryMass(0.0);
	}
}

PreciseDouble AnimalNonStatistical::calculateCellQuality(const AnimalNonStatistical& prey, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate)
{
	if(getId() != prey.getId() && canEatAnimal(prey, animalsHasTriedToPredate))
	{
		return decisions.calculateCellQuality(prey);
	}
	else
	{
		return 0.0;
	}
}

PreciseDouble AnimalNonStatistical::calculateCellQuality(const CellResourceInterface& prey, const DryMass& preyDryMass)
{
	if(canEatResource(preyDryMass))
	{
		return decisions.calculateCellQuality(prey, preyDryMass);
	}
	else
	{
		return 0.0;
	}
}

PreciseDouble AnimalNonStatistical::calculateEdibilityValue(const AnimalNonStatistical& prey, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate)
{
	if(getId() != prey.getId() && canEatAnimal(prey, animalsHasTriedToPredate))
	{
		return decisions.calculateEdibilityValue(prey);
	}
	else
	{
		return 0.0;
	}
}

PreciseDouble AnimalNonStatistical::calculateEdibilityValue(const CellResourceInterface& prey, const DryMass& preyDryMass)
{
	if(canEatResource(preyDryMass))
	{
		return decisions.calculateEdibilityValue(prey, preyDryMass);
	}
	else
	{
		return 0.0;
	}
}

bool AnimalNonStatistical::hasTriedToHunt(const AnimalNonStatistical& prey, const list<const AnimalNonStatistical*> &animalsHasTriedToPredate) const
{
	return find(animalsHasTriedToPredate.cbegin(), animalsHasTriedToPredate.cend(), &prey) != animalsHasTriedToPredate.cend();
}

bool AnimalNonStatistical::canEatAnimal(const AnimalNonStatistical& prey, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate) const
{
	return !hasTriedToHunt(prey, animalsHasTriedToPredate);
}

bool AnimalNonStatistical::canEatResource(const DryMass &dryMass) const
{
	return dryMass > 0.0;
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(AnimalNonStatistical)

template <class Archive>
void AnimalNonStatistical::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<Animal>(*this);

	ar & previousNetGrowth;

	ar & currentPrey;

	ar & unbornTimeSteps;
	
	ar & eatenToday;

	ar & growthBuildingBlock;

	ar & decisions;

	if (Archive::is_loading::value) {
		decisions.setOwner(this);
	}

	ar & huntingMode;

	ar & voracity;

	ar & speed;

	ar & scopeAreaRadius;
	ar & searchAreaRadius;
	ar & interactionAreaRadius;

	ar & inBreedingZone;

	ar & inHabitatShiftBeforeBreeding;
	ar & inHabitatShiftAfterBreeding;

	ar & atDestination;
	ar & targetNeighborToTravelTo;

	ar & idFromMatedMale;
	ar & generationNumberFromMatedMale;
	ar & mated;
	ar & genomeFromMatedMale;

	ar & instarToEvaluateCells;

	ar & exhausted;

	ar & steps;

	ar & stepsAttempted;

	ar & totalPredationEncounters;
	ar & predatedByID;

	ar & predationEncountersCurrentDay;

	ar & timeStepsWithoutFood;
	ar & sated;
	ar & ageOfLastMoultOrReproduction;
	ar & dateOfDeath;
	ar & lastDayMoved;
	ar & firstMovement;

	ar & generationNumberFromFemaleParent;
	ar & generationNumberFromMaleParent;
	ar & idFromFemaleParent;
	ar & idFromMaleParent;
	ar & reproCounter;
	ar & fecundity;
	ar & ageOfFirstReproduction;
}             

// // Specialisation
template void AnimalNonStatistical::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalNonStatistical::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalNonStatistical::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalNonStatistical::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
