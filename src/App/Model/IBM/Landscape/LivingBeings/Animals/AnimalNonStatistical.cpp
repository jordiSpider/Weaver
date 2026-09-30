#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"

#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"
#include "App/Manager/LogManager.h"

#include <fmt/compile.h>
#include <fmt/format.h>
#include <iterator>


using namespace std;
namespace fs = std::filesystem;







bool compareByEdibilityValue(const tuple<PreciseDouble, Edible*, DryMass, bool>& firstElem, const tuple<PreciseDouble, Edible*, DryMass, bool>& secondElem) {
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
	: Animal(id_type(0), mySpecies, terrainCell, genome, LifeStage::ACTIVE),
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
	setOtherAttributes(Generation(0), Generation(0), id_type(), id_type());

	// Check that the value of growth is in the valid value range
	checkGrowthMinimumValue();

	updateSignature();
}


AnimalNonStatistical::AnimalNonStatistical(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& newFactorEggMassFromMom, const Generation& g_numb_prt_female,
		const Generation& g_numb_prt_male, id_type ID_prt_female, id_type ID_prt_male, AnimalSpecies* const mySpecies, Gender gender, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
	: Animal(id_type(0), mySpecies, parentTerrainCell, LifeStage::UNBORN, firstParentGamete, secondParentGamete, gender), 
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

	updateSignature();
}


void AnimalNonStatistical::setOtherAttributes(const Generation& g_numb_prt_female, const Generation& g_numb_prt_male, id_type ID_prt_female, id_type ID_prt_male)
{
	mated = false;
	genomeFromMatedMale = nullptr;

	distanceTravelled = 0.0;
	exhausted = false;
	stepsAttempted = 0;
	sated = false;
	//encounters_prey = 0;

	totalPredationEncounters = 0u;
	predationEncountersCurrentDay = 0u;
	actionsCurrentTimeStep = 0u;

	timeStepsWithoutFood = TimeStep(0);

	unbornTimeSteps = TimeStep(0);


	setAgeOfLastMoultOrReproduction(TimeStep(0));
	dateOfDeath = Day(-1);
	

	eatenToday = 0;
	
	generationNumberFromFemaleParent = g_numb_prt_female;
	generationNumberFromMaleParent = g_numb_prt_male;
	idFromFemaleParent = ID_prt_female; //TODO puntero al padre mejor? o no? esto se usa?
	idFromMaleParent = ID_prt_male;
	reproCounter = 0;
	fecundity = 0;
	ageOfFirstReproduction = TimeStep(0);
}


AnimalNonStatistical::~AnimalNonStatistical()
{
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

void AnimalNonStatistical::tune(Landscape* const landscape, const bool saveMassInfo, fmt::memory_buffer& infoMassText, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, CustomIndexedVector<Instar, PreciseDouble>& maximumInteractionArea, CustomIndexedVector<Instar, PreciseDouble>& maximumVoracity)
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
			throwLineInfoException("Animal " + std::to_string(getId()) + ": Maximum voracity timeStep is negative.");
		}


		DryMass previousTarget = getGrowthBuildingBlock().calculatePreviousTarget(getGender(), getReproCounter());
		DryMass nextTarget = getGrowthBuildingBlock().calculateNextTarget(getGender());


		// enhanced h
		if(getGrowthBuildingBlock().hasCapitalBreeding())
		{
			enhancedH = 0.0;
		}
		else
		{
			if (getGender() == Gender::MALE && getGrowthBuildingBlock().isMature()) {
				enhancedH = 1.0;
			}
			else {
				if (nextTarget == previousTarget)
				{
					enhancedH = 0.0;
				}
				else
				{
					enhancedH = 1 - (getGrowthBuildingBlock().getCurrentTotalDryMass() - previousTarget).getValue() / (nextTarget - previousTarget).getValue();

					if (getEnhancedH() < 0)
					{
						enhancedH = 0.0;
					}
				}
			}
		}

			
		DryMass deficit = getGrowthBuildingBlock().getNextMassPredicted() - getGrowthBuildingBlock().getCurrentTotalDryMass();

		PreciseDouble voracityAfterApplyProportion;

		if (getLifeStage() == LifeStage::PUPA) {
			preFinalVoracity = DryMass(0.0);
			predVor = DryMass(0.0);
			voracityAfterApplyProportion = 0.0;
		}
		else {
			const PreciseDouble& voracityProportion = getGenetics().getBaseIndividualTraits(BaseTraitType::voracityProportion).getPhenotypicValue();
			
			if (deficit < 0.0) {
				voracityAfterApplyProportion = preFinalVoracity.getValue() * getEnhancedH() * voracityProportion;
			}
			else {
				const PreciseDouble& assim = getGenetics().getBaseIndividualTraits(BaseTraitType::assim).getPhenotypicValue();

				voracityAfterApplyProportion = (preFinalVoracity.getValue() + (deficit.getValue() / assim)) * voracityProportion;
			}

			voracityAfterApplyProportion = fmax(voracityAfterApplyProportion, 0.0);
		}


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
		PreciseDouble searchAfterPlasticity = applyPlasticityDueToConditionToTrait(searchAfterAllometric, getSpecies()->getPlasticityDueToConditionSearch(), getEnhancedH());
		

		PreciseDouble speedAfterAllometric = applyAllometricModel(
			getGenetics().getBaseIndividualTraits(BaseTraitType::coeffMassForSpeed).getPhenotypicValue(),
			getGenetics().getBaseIndividualTraits(BaseTraitType::scaleMassForSpeed).getPhenotypicValue()
		);
		
		PreciseDouble postTspeed = landscape->calculatePostTSpeed(speedAfterAllometric, getGrowthBuildingBlock().getCurrentTotalWetMass());
		PreciseDouble speedAfterPlasticity = applyPlasticityDueToConditionToTrait(postTspeed, getSpecies()->getPlasticityDueToConditionSpeed(), getEnhancedH());


		
		PreciseDouble searchAfterEncounters;
		PreciseDouble vorAfterEncounters;

		if(getPredationEncountersCurrentDay() > 0u)
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
		getSpecies()->getFemaleMaxReproductionEvents(), 
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
		fmt::format_to(fmt::appender(infoMassText), FMT_COMPILE("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n"),
			numberOfTimeSteps, getId(), getGrowthBuildingBlock().getCurrentTotalDryMass(), instarTargetMass, 
			getGrowthBuildingBlock().getMassForNextReproduction(), growthCurveMass, 
			getGrowthBuildingBlock().getNextMassPredicted(), getGrowthBuildingBlock().getCurrentAge(),
			instarTargetAge, getGrowthBuildingBlock().getAgeForNextReproduction()
		);
	}






	PreciseDouble& instarMaximumVoracity = maximumVoracity[getGrowthBuildingBlock().getInstar()];
	instarMaximumVoracity = fmax(instarMaximumVoracity, getVoracity());
	

	PreciseDouble& instarMaximumInteractionArea = maximumInteractionArea[getGrowthBuildingBlock().getInstar()];
	instarMaximumInteractionArea = fmax(instarMaximumInteractionArea, getInteractionAreaRadius());





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


const uint64_t& AnimalNonStatistical::getPredationEncountersCurrentDay() const
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
	return (getSearchAreaRadius() > 0.0) ? distanceTravelled/getSearchAreaRadius() : 0.0;
}

void AnimalNonStatistical::metabolize(const Landscape* const landscape, const TimeStep& actualTimeStep)
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
		LogManager::emit("The metabolic loss was 0 or positive:\n");
		LogManager::emit(fmt::format(" - Animal: {}({})\n", getId(), getSpecies()->getScientificName()));
		LogManager::emit(fmt::format(" - Last energy tank: {}\n", lastEnergyTank));
		LogManager::emit(fmt::format(" - After loss energy tank: {}\n", afterLossEnergyTank));
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


void AnimalNonStatistical::increasePredationEncounters(CustomIndexedVector<AnimalSpeciesID, uint64_t>& animalSpeciesMaximumPredationEncountersPerDay)
{
	totalPredationEncounters++;
	predationEncountersCurrentDay++;

	uint64_t& maximumPredationEncountersPerDay = animalSpeciesMaximumPredationEncountersPerDay[getSpecies()->getAnimalSpeciesId()];
	maximumPredationEncountersPerDay = max(maximumPredationEncountersPerDay, getPredationEncountersCurrentDay());
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

id_type AnimalNonStatistical::getIdFromFemaleParent() const
{
	return idFromFemaleParent;
}

id_type AnimalNonStatistical::getIdFromMaleParent() const
{
	return idFromMaleParent;
}

void AnimalNonStatistical::formatToBufferDirect(fmt::memory_buffer& out) const noexcept
{
	const AnimalNonStatisticalGrowth& growthBB = getGrowthBuildingBlock();
	const Genetics& geneticsBB = getGenetics();

	const IndividualTrait& eggDevTrait = geneticsBB.getBaseIndividualTraits(BaseTraitType::eggDevTime);
	const IndividualTrait& energyTankTrait = geneticsBB.getBaseIndividualTraits(BaseTraitType::energy_tank);

	const size_t estimatedAnimalLineSize = 2000u;
	// Reuse per-animal memory buffer to minimize per-call allocations and
	// to allow fmt to write contiguously into a buffer.
	fmt::memory_buffer& buf = formatBuffer;

	// Ensure initial reservation is large enough to hold a full line
	if (buf.capacity() < estimatedAnimalLineSize) {
		buf.reserve(estimatedAnimalLineSize);
	}

	// Clear previous content but keep reserved capacity
	buf.clear();

	// Single format_to writing the main fixed fields and the trailing newline
	fmt::format_to(fmt::appender(buf), FMT_COMPILE("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}"),
		getId(),
		getSpecies()->getScientificName(),
		getGender(),
		getPosition(),
		lifeStage,
		growthBB.getInstar(),
		growthBB.getCurrentAge(),
		eggDevTrait.getConstitutiveValue(),
		growthBB.getDateEgg(),
		getAgeOfFirstReproduction(),
		getReproCounter(),
		getFecundity(),
		getDateOfDeath(),
		getGenerationNumberFromFemaleParent(),
		getGenerationNumberFromMaleParent(),
		getIdFromFemaleParent(),
		getIdFromMaleParent(),
		getPredationEncountersCurrentDay(),
		getTotalPredationEncounters(),
		getVoracity(),
		getSearchAreaRadius(),
		getSpeed(),
		energyTankTrait.getConstitutiveValue(),
		eggDevTrait.getConstitutiveValue(),
		growthBB.getCurrentBodySize(),
		growthBB.getCurrentTotalDryMass(),
		growthBB.getCurrentEnergyTank()
	);

	// Append genetics directly into the same buffer
	geneticsBB.formatToBufferDirect(buf);

	buf.push_back('\n');

	out.append(buf.data(), buf.data() + buf.size());
}

void AnimalNonStatistical::formatToBufferDirect(std::string& out) const noexcept
{
	fmt::memory_buffer localOut;
	formatToBufferDirect(localOut);

	out.append(localOut.data(), localOut.size());
}

void AnimalNonStatistical::getHeader(std::string& header)
{
	header.append("id\tspecies\tgender");

	for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
	{
		fmt::format_to(std::back_inserter(header), "\t{}", magic_enum::enum_names<Axis>()[axis]);
	}

	header.append("\tstate\tinstar\tage\tpheno_ini\tdateEgg\tage_first_rep\treproCounter\tfecundity\tdate_death\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\tencounters_pred\tglobal_pred_encs\tvor_ini\tsearch_ini\tspeed_ini\ttank_ini\tpheno_ini\tcurrentBodySize\tcurrentDryMass\tcurrentEnergyTank");
			
	Trait::getHeader(header);
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

void AnimalNonStatistical::printVoracities(const Landscape* const landscape, fmt::memory_buffer& voracitiesText, const PreciseDouble& timeStepsPerDay)
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


	fmt::format_to(fmt::appender(voracitiesText), FMT_COMPILE("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n"),
		getId(), getSpecies()->getScientificName(), lifeStage, getGrowthBuildingBlock().getCurrentAge() - TimeStep(1),
		getGrowthBuildingBlock().getInstar(), getGrowthBuildingBlock().isMature(), getGrowthBuildingBlock().getCurrentBodySize(),
		getGrowthBuildingBlock().getCurrentEnergyTank(), getGrowthBuildingBlock().getCurrentTotalDryMass(),
		getGrowthBuildingBlock().getCurrentTotalWetMass(), getGrowthBuildingBlock().getTankAtGrowth(),
		getGrowthBuildingBlock().getNextMassPredicted(), maxVoracityTimeStep, minMassAtCurrentAge, getVoracity(),
		getSearchAreaRadius(), getSpeed(), getVoracity(), getSearchAreaRadius(), getSpeed(), 
		getGrowthBuildingBlock().getCurrentTotalDryMass() + getVoracity(), foodMassEatenCurrentTimeStep,
		getGrowthBuildingBlock().getCurrentTotalDryMass() + foodMassAssimilatedCurrentTimeStep,
		totalMetabolicDryMassLossAfterAssim, getSearchAreaRadius(), eatenToday, getEnhancedH(), distanceTravelled, stepsAttempted,
		getSearchAreaRadius(), sated, calculateProportionOfTimeWasMoving(), 
		getVoracity() / getGrowthBuildingBlock().getCurrentTotalDryMass().getValue(),
		getGender(), mated, getGrowthBuildingBlock().getEggDryMassAtBirth(), 
		getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue(),
		getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue(),
		getDateOfDeath(), getGrowthBuildingBlock().getAgeOfFirstMaturation(pupaPeriodTime), reproCounter
	);
}

PreciseDouble AnimalNonStatistical::applyPlasticityDueToConditionToTrait(const PreciseDouble& traitValue, const PreciseDouble& plasticityDueToCondition, const PreciseDouble& h) const
{
	return traitValue*(1-exp(-plasticityDueToCondition*h));
}


void AnimalNonStatistical::printTraits(fmt::memory_buffer& traitsText)
{
	fmt::format_to(fmt::appender(traitsText), FMT_COMPILE("{}\t{}"),
		getId(), getSpecies()->getScientificNameReplaced()
	);

	for(unsigned char axis = 0; axis < DIMENSIONS; axis++)
	{
		fmt::format_to(fmt::appender(traitsText), FMT_COMPILE("\t{}"),
			getPositionAxisValue(getPosition(), axis)
		);
	}

	fmt::format_to(fmt::appender(traitsText), FMT_COMPILE("\t{}\t{}\t{}\t{}\t{}"),
		generationNumberFromFemaleParent, generationNumberFromMaleParent, getIdFromFemaleParent(),
		getIdFromMaleParent(), getGrowthBuildingBlock().getDateEgg()
	);

	traitsText.push_back('\t');

	getGenetics().printTraits(traitsText);

	traitsText.push_back('\n');
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

	if(getLifeStage() == LifeStage::REPRODUCING && getLifeStage() != newLifeStage)
	{
		setInstarToEvaluateCells(getGrowthBuildingBlock().getInstar());
	}


	getMutableTerrainCell()->eraseAnimal(this);

	lifeStage = newLifeStage;

	updateSignature();

	getMutableTerrainCell()->insertAnimal(landscape, this);
}


void AnimalNonStatistical::setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay)
{
	setNewLifeStage(landscape, newLifeStage);

	switch (newLifeStage)
	{
	case LifeStage::STARVED: {
		setDateOfDeath(Day(numberOfTimeSteps, timeStepsPerDay));
		nextAction = Action::NONE;
		break;
	}
	case LifeStage::PREDATED: {
		setDateOfDeath(Day(numberOfTimeSteps, timeStepsPerDay));
		nextAction = Action::NONE;
		break;
	}
	case LifeStage::BACKGROUND: {
		setDateOfDeath(Day(numberOfTimeSteps, timeStepsPerDay));
		nextAction = Action::NONE;
		break;
	}
	case LifeStage::SENESCED: {
		setDateOfDeath(Day(numberOfTimeSteps, timeStepsPerDay));
		nextAction = Action::NONE;
		break;
	}
	case LifeStage::SHOCKED: {
		setDateOfDeath(Day(numberOfTimeSteps, timeStepsPerDay));
		nextAction = Action::NONE;
		break;
	}
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


void AnimalNonStatistical::setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage, const TimeStep numberOfTimeSteps, id_type predatorId, const PreciseDouble& timeStepsPerDay)
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

void AnimalNonStatistical::setPredatedByID(id_type predatorId) 
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

void AnimalNonStatistical::doInitialCellEvaluation(CustomIndexedVector<Instar, PreciseDouble>& maximumPatchEdibilityValueGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchPredationRiskGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchConspecificBiomassGlobal)
{
	searchTargetToTravelTo(getScopeAreaRadius(), maximumPatchEdibilityValueGlobal, maximumPatchPredationRiskGlobal, maximumPatchConspecificBiomassGlobal);
	setAtDestination(true);
}


bool AnimalNonStatistical::mustDoHabitatShift() const
{
	return getGrowthBuildingBlock().isInHabitatShift() || isInHabitatShiftBeforeBreeding() || isInHabitatShiftAfterBreeding();
}


AnimalNonStatistical::Action AnimalNonStatistical::getNextAction() const
{
	return nextAction;
}


void AnimalNonStatistical::actionPlanning(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay,
		bool saveEdibilitiesFile, fmt::memory_buffer& edibilitiesText, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchEdibilityValueGlobal,
		CustomIndexedVector<Instar, PreciseDouble>& maximumPatchPredationRiskGlobal, 
		CustomIndexedVector<Instar, PreciseDouble>& maximumPatchConspecificBiomassGlobal)
{
	if (!(getLifeStage() == LifeStage::ACTIVE || getLifeStage() == LifeStage::REPRODUCING))
	{
		nextAction = Action::NONE;
		return;
	}


	if (mustDoHabitatShift())
	{
		if (getGrowthBuildingBlock().isInHabitatShift() && (isInHabitatShiftBeforeBreeding() || isInHabitatShiftAfterBreeding()))
		{
			getMutableGrowthBuildingBlock().setInHabitatShift(false);
		}


		PreciseDouble currentScopeAreaRadius = getScopeAreaRadius();

		if (getGrowthBuildingBlock().isInHabitatShift())
		{
			setScopeAreaRadius(getScopeAreaRadius() * getSpecies()->getGrowthBuildingBlock().getHabitatShiftFactor());

			getMutableGrowthBuildingBlock().setInHabitatShift(false);
		}
		else if (isInHabitatShiftBeforeBreeding())
		{
			setScopeAreaRadius(getScopeAreaRadius() * getSpecies()->getHabitatShiftBeforeBreedingFactor());

			setInHabitatShiftBeforeBreeding(false);
		}
		else if (isInHabitatShiftAfterBreeding())
		{
			setScopeAreaRadius(getScopeAreaRadius() * getSpecies()->getHabitatShiftAfterBreedingFactor());

			setInHabitatShiftAfterBreeding(false);
		}


		searchTargetToTravelTo(getScopeAreaRadius(), maximumPatchEdibilityValueGlobal, maximumPatchPredationRiskGlobal, maximumPatchConspecificBiomassGlobal);


		setScopeAreaRadius(currentScopeAreaRadius);


		nextAction = Action::HABITAT_SHIFT;
		return;
	}

	
	if (isExhausted()) {
		nextAction = Action::NONE;
		return;
	}

	if (lifeStage == LifeStage::REPRODUCING && getSpecies()->occursHabitatShiftBeforeBreeding() && inBreedingZone)
	{
		nextAction = Action::NONE;
		return;
	}

	if (!(!sated || // If it is not satisfied, it will have to look for food
		(sated && getGrowthBuildingBlock().isMature() && getGender() == Gender::MALE) || // If it is sated, mature and a male, so it will look for a mate to breed with
		(sated && getGrowthBuildingBlock().isMature() && getGender() == Gender::FEMALE && lifeStage == LifeStage::REPRODUCING && getSpecies()->occursHabitatShiftBeforeBreeding())))
	{
		nextAction = Action::NONE;
		return;
	}

	
	vector<tuple<PreciseDouble, Edible*, DryMass, bool>> ediblesByEdibility;


	if(numberOfTimeSteps == TimeStep(0) && actionsCurrentTimeStep == 0u) {
		doInitialCellEvaluation(maximumPatchEdibilityValueGlobal, maximumPatchPredationRiskGlobal, maximumPatchConspecificBiomassGlobal);
	}

	if(actionsCurrentTimeStep == 0u && checkInitialMovementCellLeaving()) {
		bool withoutDestinations = searchTargetToTravelTo(getScopeAreaRadius(), maximumPatchEdibilityValueGlobal, maximumPatchPredationRiskGlobal, maximumPatchConspecificBiomassGlobal);
				
		if(!withoutDestinations) {
			nextAction = Action::MOVEMENT;
			return;
		}
	}

	if (!isSated()) {
		if (getCurrentPrey().isThereLeftoverFood()) {
			nextAction = Action::FEED;
			return;
		}
		else {
			searchAnimalsAndResourceToEat(landscape, ediblesByEdibility, numberOfTimeSteps, saveEdibilitiesFile, edibilitiesText);
		}
	}

	if(getGrowthBuildingBlock().isMature() && !isMated())
	{
		searchAnimalToBreed(landscape, numberOfTimeSteps, timeStepsPerDay);
	}

	if(!ediblesByEdibility.empty() && !isSated())
	{
		auto ediblesIt = ediblesByEdibility.begin();

		potencialPrey = make_tuple<>(get<1>(*ediblesIt), get<2>(*ediblesIt), get<3>(*ediblesIt));
		nextAction = Action::PREDATE;
		return;
	}
	else
	{
		bool withoutDestinations = false;

		if(isAtDestination())
		{
			withoutDestinations = searchTargetToTravelTo(getScopeAreaRadius(), maximumPatchEdibilityValueGlobal, maximumPatchPredationRiskGlobal, maximumPatchConspecificBiomassGlobal);
		}

		if(withoutDestinations)
		{
			nextAction = Action::NONE;
			return;
		}
		else
		{
			nextAction = Action::MOVEMENT;
			return;
		}
	}
}


void AnimalNonStatistical::actionExecution(Landscape* const landscape, const bool saveActivity, fmt::memory_buffer& activitiesText,
		const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay,
		const bool saveMovements, 
		fmt::memory_buffer& movementsText)
{
	switch (nextAction) {
	case Action::MOVEMENT:
		move(landscape, numberOfTimeSteps, timeStepsPerDay, saveMovements, movementsText, saveActivity, activitiesText);
		break;
	case Action::FEED:
		feed(saveActivity, activitiesText, numberOfTimeSteps, timeStepsPerDay);
		break;
	case Action::HABITAT_SHIFT:
		habitatShift(landscape);
		break;
	case Action::NONE:
		throwLineInfoException("Parallel execution is not supported for this action type");
		break;
	case Action::PREDATE:
		throwLineInfoException("Parallel execution is not supported for this action type");
		break;
	default:
		throwLineInfoException("Default case");
		break;
	}

	actionsCurrentTimeStep++;
}

void AnimalNonStatistical::habitatShift(Landscape* const landscape)
{
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

	if(getLifeStage() == LifeStage::REPRODUCING)
	{
		setInBreedingZone(true);
	}
}

void AnimalNonStatistical::feed(const bool saveActivity, fmt::memory_buffer& activitiesText, const TimeStep numberOfTimeSteps,
		const PreciseDouble& timeStepsPerDay)
{
	DryMass foodMass = computeHandlingFoodMass();
		

	getCurrentPrey().decreaseFoodMass(foodMass);


	if(getSpecies()->getActivatedHandling())
	{
		applyHandlingTime(foodMass, saveActivity, activitiesText, numberOfTimeSteps, timeStepsPerDay);
	}


	assimilateFoodMass(foodMass);


	if(!getSpecies()->getPreserveLeftovers() || !getCurrentPrey().isThereLeftoverFood()) 
	{
		removeCurrentPrey();
	}
}

DryMass AnimalNonStatistical::computeHandlingFoodMass() const
{
	if(getSpecies()->getActivatedHandling())
	{
		return DryMass(fmin((getDistanceTravelled() / getSearchAreaRadius()) * getCurrentPrey().getFoodDryMassPerTimeStep().getValue(), getRemainingVoracity()));
	}
	else
	{
		return DryMass(fmin(getCurrentPrey().getFoodDryMass().getValue(), getRemainingVoracity()));
	}
}

void AnimalNonStatistical::applyHandlingTime(const DryMass& foodMass, const bool saveActivity, fmt::memory_buffer& activitiesText, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
	Day initialDay = Day(actualTimeStep, timeStepsPerDay) + Day((getDistanceTravelled() / getSearchAreaRadius()) * timeStepsPerDay);

	PreciseDouble distanceToAdd = (foodMass.getValue() / getCurrentPrey().getFoodDryMassPerTimeStep().getValue()) * getSearchAreaRadius();

	increaseDistanceTravelled(distanceToAdd);

	Day finalDay = Day(actualTimeStep, timeStepsPerDay) + Day((getDistanceTravelled() / getSearchAreaRadius()) * timeStepsPerDay);

	if(saveActivity)
	{
		fmt::format_to(fmt::appender(activitiesText), "{}\t{}\t{}\t{}\t{}\t{}\n",
			getId(), getSpecies()->getScientificName(), EnumClass<ActivityType>::to_string(ActivityType::HANDLING),
			initialDay.getValue().getValue(), finalDay.getValue().getValue(), (finalDay - initialDay).getValue().getValue()
		);
	}
}

const std::tuple<Edible*, DryMass, bool>& AnimalNonStatistical::getPotencialPrey() const
{
	return potencialPrey;
}

std::tuple<Edible*, DryMass, bool>& AnimalNonStatistical::getPotencialPrey()
{
	return potencialPrey;
}

bool AnimalNonStatistical::predate(const bool retaliation, const bool saveAnimalsEachDayPredationProbabilities, 
		fmt::memory_buffer& predationProbabilitiesText, Landscape* const landscape, const TimeStep numberOfTimeSteps, 
		const PreciseDouble& timeStepsPerDay, const bool competitionAmongResourceSpecies, 
		CustomIndexedVector<Species::ID, unsigned int>& predationEventsOnOtherSpecies,
		CustomIndexedVector<AnimalSpeciesID, uint64_t>& animalSpeciesMaximumPredationEncountersPerDay)
{
	if (get<0>(getPotencialPrey())->getSpecies()->isMobile()) {
		auto potencialPreyLifeStage = static_cast<AnimalNonStatistical&>(*get<0>(getPotencialPrey())).getLifeStage();

		if (potencialPreyLifeStage != LifeStage::ACTIVE && potencialPreyLifeStage != LifeStage::REPRODUCING) {
			return false;
		}
	}


	PreciseDouble randomProbability = Random::randomUniform();

	PreciseDouble probabilityToCompare = getSpecies()->getDecisionsBuildingBlock()->getKillProbability();

	if(retaliation) {
		probabilityToCompare = getSpecies()->getDecisionsBuildingBlock()->getKillProbability();
	}
	else {
		probabilityToCompare = calculatePredationProbability(*get<0>(getPotencialPrey()), get<1>(getPotencialPrey()));
	}


	if(saveAnimalsEachDayPredationProbabilities)
	{
		fmt::format_to(fmt::appender(predationProbabilitiesText), "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}",
			randomProbability.getValue(),
			probabilityToCompare.getValue(),
			static_cast<unsigned int>(retaliation),
			getId(),
			get<0>(getPotencialPrey())->getId(),
			getSpecies()->getScientificName(),
			get<0>(getPotencialPrey())->getSpecies()->getScientificName(),
			static_cast<unsigned int>(get<0>(getPotencialPrey())->isHunting()),
			getGrowthBuildingBlock().getCurrentTotalDryMass().getValue().getValue(),
			get<1>(getPotencialPrey()).getValue().getValue()
		);
	}


	if (get<0>(getPotencialPrey())->getSpecies()->isMobile()) {
		static_cast<AnimalNonStatistical&>(*get<0>(getPotencialPrey())).increasePredationEncounters(animalSpeciesMaximumPredationEncountersPerDay);
	}


	if(probabilityToCompare >= randomProbability)
	{
		decisions.addToCumulativePredationProbability(probabilityToCompare);


		if(saveAnimalsEachDayPredationProbabilities)
		{
			fmt::format_to(fmt::appender(predationProbabilitiesText), "\t{}\n", static_cast<unsigned int>(true));
		}


		if (!retaliation || (retaliation && getCurrentPrey().getFoodDryMass() <= get<1>(getPotencialPrey())))
		{
			setCurrentPrey(getPotencialPrey(), landscape, numberOfTimeSteps, timeStepsPerDay, competitionAmongResourceSpecies, predationEventsOnOtherSpecies);
		}
	
		
		return true;
	}
	else
	{
		if(saveAnimalsEachDayPredationProbabilities)
		{
			fmt::format_to(fmt::appender(predationProbabilitiesText), "\t{}\n", static_cast<unsigned int>(false));
		}


		if(!retaliation && get<0>(getPotencialPrey())->getSpecies()->isMobile())
		{
			const PreciseDouble edibilityValue = static_cast<AnimalNonStatistical&>(*get<0>(getPotencialPrey())).calculateEdibilityValue(*this);

			if(edibilityValue > 0.0)
			{
				static_cast<AnimalNonStatistical&>(*get<0>(getPotencialPrey())).activateRetaliation(this, 
					getGrowthBuildingBlock().getCurrentTotalDryMass(), saveAnimalsEachDayPredationProbabilities, 
					predationProbabilitiesText, landscape, numberOfTimeSteps, timeStepsPerDay, 
					competitionAmongResourceSpecies, predationEventsOnOtherSpecies, animalSpeciesMaximumPredationEncountersPerDay);
			}
		}


		return false;
	}
}

void AnimalNonStatistical::activateRetaliation(Edible* prey, const DryMass &targetDryMass, const bool saveAnimalsEachDayPredationProbabilities,
		fmt::memory_buffer& predationProbabilitiesText, Landscape* const landscape, const TimeStep numberOfTimeSteps,
		const PreciseDouble& timeStepsPerDay, const bool competitionAmongResourceSpecies, 
		CustomIndexedVector<Species::ID, unsigned int>& predationEventsOnOtherSpecies,
		CustomIndexedVector<AnimalSpeciesID, uint64_t>& animalSpeciesMaximumPredationEncountersPerDay)
{
	auto currentPotencialPrey = potencialPrey;

	potencialPrey = make_tuple<>(prey, targetDryMass, true);

	bool success = predate(true, saveAnimalsEachDayPredationProbabilities, predationProbabilitiesText, landscape, numberOfTimeSteps,
		timeStepsPerDay, competitionAmongResourceSpecies, predationEventsOnOtherSpecies, animalSpeciesMaximumPredationEncountersPerDay);

	if (success) {
		nextAction = Action::NONE;
	}
	else {
		potencialPrey = currentPotencialPrey;
	}
}

void AnimalNonStatistical::updateTimeStepsWithoutFood()
{
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

	if (getInteractionAreaRadius() > 0.0) {
		getMutableTerrainCell()->applyFunctionToEdiblesInRadius(getPosition(), getInteractionAreaRadius(), animalFunctions, resourceFunctions);
	}
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

void AnimalNonStatistical::setInstarToEvaluateCells(const Instar& newInstarToEvaluateCells)
{
	instarToEvaluateCells = newInstarToEvaluateCells;
}

void AnimalNonStatistical::searchAnimalsAndResourceToEat(Landscape* const landscape, vector<tuple<PreciseDouble, Edible*, DryMass, bool>>& ediblesByEdibility, const TimeStep numberOfTimeSteps, bool saveEdibilitiesFile, fmt::memory_buffer& edibilitiesText)
{
	vector<pair<const AnimalSearchParams&, AnimalFunctions>> animalFunctions;

	animalFunctions.emplace_back(
		getSpecies()->getPreySearchParams(getGrowthBuildingBlock().getInstar()).getAnimalSearchParams(),
		AnimalFunctions{
			PreviousAnimalFunctions{},
			IndividualFunctions{
				[this, &ediblesByEdibility](Animal& animal) { 
					AnimalNonStatistical* animalCast = static_cast<AnimalNonStatistical*>(&animal);

					const PreciseDouble edibilityValue = calculateEdibilityValue(*animalCast);

					if(edibilityValue > 0.0)
					{
						ediblesByEdibility.push_back(tuple<PreciseDouble, Edible*, DryMass, bool>(edibilityValue, animalCast, animalCast->getGrowthBuildingBlock().getCurrentTotalDryMass(), true));
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
			[this, &landscape, &ediblesByEdibility](CellResourceInterface& resource, bool fullCoverage, const PointContinuous* const sourcePosition, const PreciseDouble &radius) {
				const DryMass dryMassAvailable = resource.calculateDryMassAvailable(fullCoverage, sourcePosition, radius);

				if (dryMassAvailable >= landscape->getMinExploitableResource())
				{
					const PreciseDouble edibilityValue = calculateEdibilityValue(resource, dryMassAvailable);

					if (edibilityValue > 0.0)
					{
						ediblesByEdibility.push_back(tuple<PreciseDouble, Edible*, DryMass, bool>(edibilityValue, &resource, dryMassAvailable, fullCoverage));
					}
				}
			}
		}
	);
		

	if (getInteractionAreaRadius() > 0.0) {
		getMutableTerrainCell()->applyFunctionToEdiblesInRadius(getPosition(), getInteractionAreaRadius(), animalFunctions, resourceFunctions);
	}


	// Sorting the elements by edibility
	std::sort(ediblesByEdibility.begin(), ediblesByEdibility.end(), compareByEdibilityValue);

	// Print interaction information
	if (saveEdibilitiesFile) {
		for (auto ediblesIt = ediblesByEdibility.begin(); ediblesIt != ediblesByEdibility.end(); ediblesIt++)
		{
			fmt::format_to(fmt::appender(edibilitiesText), "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n",
				numberOfTimeSteps.getValue(), getId(), getSpecies()->getScientificName(), 
				foodMassEatenCurrentTimeStep.getValue().getValue(), getId(), getSpecies()->getScientificName(), 
				getGrowthBuildingBlock().getCurrentTotalDryMass().getValue().getValue(), get<1>(*ediblesIt)->getId(),
				get<1>(*ediblesIt)->getSpecies()->getScientificName(), get<2>(*ediblesIt).getValue().getValue(), 
				decisions.calculatePredationProbability(*get<1>(*ediblesIt), get<2>(*ediblesIt)).getValue(),
				get<0>(*ediblesIt).getValue(), 
				decisions.getPreference(get<1>(*ediblesIt)->getSpecies()->getId(), get<1>(*ediblesIt)->getGrowthBuildingBlock().getInstar()).getValue(), 
				decisions.getMeanExperience(get<1>(*ediblesIt)->getSpecies()->getId(), get<1>(*ediblesIt)->getGrowthBuildingBlock().getInstar()).getValue()
			);
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

void AnimalNonStatistical::doDefinitive(Landscape* const landscape, const bool saveGenetics, std::vector<fmt::memory_buffer>& geneticsText) {
	id = landscape->generateEdibleId();

	landscape->generateAnimalId();

	if(saveGenetics)
	{
		std::string animalInfo;

		fmt::format_to(std::back_inserter(animalInfo), "{}\t{}\t{}\t{}\t{}\t{}",
			getId(), getSpecies()->getScientificName(), generationNumberFromFemaleParent,
			generationNumberFromMaleParent, getIdFromFemaleParent(), getIdFromMaleParent()
			);

		getMutableSpecies()->getMutableGenetics().printGenetics(animalInfo, getGenetics().getGenome(), geneticsText);
	}

	getMutableGenetics().deleteHomologousCorrelosomes();


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

unsigned int AnimalNonStatistical::breedAsexually(Landscape* const landscape, unsigned int objectiveEggsNumber, const TimeStep numberOfTimeSteps, const bool saveGenetics, std::vector<fmt::memory_buffer>& geneticsText, const PreciseDouble& timeStepsPerDay, bool saveAnimalConstitutiveTraits, fmt::memory_buffer& traitsText)
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

			id_type idFemale = getId();
			id_type idMale = 0u;

			AnimalNonStatistical* newOffspring = createOffspring(femaleGameteSelected, maleGameteSelected, getMutableTerrainCell(), getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue(), generationFemale, generationMale, idFemale, idMale, getMutableSpecies(), offSpringGender, numberOfTimeSteps, timeStepsPerDay);

			newOffspring->getMutableGrowthBuildingBlock().setDryMass(
				newOffspring->getGrowthBuildingBlock().getEggDryMassAtBirth(), newOffspring->getGrowthBuildingBlock().getEggDryMassAtBirth(), 
				newOffspring->getGenetics().getBaseIndividualTraits(BaseTraitType::energy_tank).getPhenotypicValue(), false, newOffspring->getGender(), false, numberOfTimeSteps
			);

			newOffspring->doDefinitive(landscape, saveGenetics, geneticsText);

			totalOffspringDryMass = totalOffspringDryMass + newOffspring->getGrowthBuildingBlock().getEggDryMassAtBirth();

			newOffspring->setPosition(getPosition());
			getMutableTerrainCell()->insertAnimal(landscape, newOffspring);

			if (saveAnimalConstitutiveTraits)
			{
				newOffspring->printTraits(traitsText);
			}

			newOffspring->initControlVariables(landscape->getExistingSpecies());

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
		LogManager::emit(fmt::format(" - Animal: {}: Partial breed {}/{}.\n", getId(), createdEggsNumber, objectiveEggsNumber));
	}

	
		
	fecundity += createdEggsNumber;

	getMutableGrowthBuildingBlock().modifyEnergyTank(getGrowthBuildingBlock().getCurrentEnergyTank() - totalOffspringDryMass, numberOfTimeSteps);

	if(postBreeding(timeStepsPerDay))
	{
		getMutableTerrainCell()->changeAnimalToSenesced(landscape, this, numberOfTimeSteps);
	}
	else
	{
		setNewLifeStage(landscape, LifeStage::ACTIVE);
	}


	return createdEggsNumber;
}


unsigned int AnimalNonStatistical::breedSexually(Landscape* const landscape, unsigned int objectiveEggsNumber, const TimeStep numberOfTimeSteps, const bool saveGenetics, std::vector<fmt::memory_buffer>& geneticsText, const PreciseDouble& timeStepsPerDay, bool saveAnimalConstitutiveTraits, fmt::memory_buffer& traitsText)
{
	DryMass offspringDryMass = getSpecies()->getGrowthBuildingBlock().getEggDryMass() + getSpecies()->getGrowthBuildingBlock().getEggDryMass().getValue()*getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue();

	DryMass totalOffspringDryMass(0.0);

	Gender offSpringGender = Gender::FEMALE;
	unsigned int createdEggsNumber = 0;

	Generation generationFemale;
	Generation generationMale;

	id_type idFemale = 0u;
	id_type idMale = 0u;

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
			
			newOffspring->doDefinitive(landscape, saveGenetics, geneticsText);

			totalOffspringDryMass = totalOffspringDryMass + newOffspring->getGrowthBuildingBlock().getEggDryMassAtBirth();

			newOffspring->setPosition(getPosition());
			getMutableTerrainCell()->insertAnimal(landscape, newOffspring);

			if (saveAnimalConstitutiveTraits)
			{
				newOffspring->printTraits(traitsText);
			}

			newOffspring->initControlVariables(landscape->getExistingSpecies());

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
		LogManager::emit(fmt::format(" - Animal: {}: Partial breed {}/{}.\n", getId(), createdEggsNumber, objectiveEggsNumber));
	}



	fecundity += createdEggsNumber;

	getMutableGrowthBuildingBlock().modifyEnergyTank(getGrowthBuildingBlock().getCurrentEnergyTank() - totalOffspringDryMass, numberOfTimeSteps);

	if(postBreeding(timeStepsPerDay))
	{
		getMutableTerrainCell()->changeAnimalToSenesced(landscape, this, numberOfTimeSteps);
	}
	else
	{
		setNewLifeStage(landscape, LifeStage::ACTIVE);
	}


	return createdEggsNumber;
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

unsigned int AnimalNonStatistical::breed(Landscape* const landscape, const TimeStep numberOfTimeSteps, const bool saveGenetics, std::vector<fmt::memory_buffer>& geneticsText, const PreciseDouble& timeStepsPerDay, bool saveAnimalConstitutiveTraits, fmt::memory_buffer& traitsText)
{
	unsigned int currentEggBatchNumber = getGrowthBuildingBlock().computeEggBatchNumber(getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue());

	unsigned int createdEggsNumber = 0u;

	switch (getSpecies()->getSexualType())
	{
		case SexualType::asexual:
			createdEggsNumber = breedAsexually(landscape, currentEggBatchNumber, numberOfTimeSteps, saveGenetics, geneticsText, timeStepsPerDay, saveAnimalConstitutiveTraits, traitsText);
			break;
		case SexualType::diploid:
			createdEggsNumber = breedSexually(landscape, currentEggBatchNumber, numberOfTimeSteps, saveGenetics, geneticsText, timeStepsPerDay, saveAnimalConstitutiveTraits, traitsText);
			break;
		case SexualType::haplodiploid:
			createdEggsNumber = breedSexually(landscape, currentEggBatchNumber, numberOfTimeSteps, saveGenetics, geneticsText, timeStepsPerDay, saveAnimalConstitutiveTraits, traitsText);
			break;
		default:
			throwLineInfoException("Default case");
			break;
	}

	if(getSpecies()->occursHabitatShiftAfterBreeding())
	{
		setInHabitatShiftAfterBreeding(true);
	}

	return createdEggsNumber;
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

	foodMassAssimilatedCurrentTimeStepPerSpecies[getCurrentPrey().getId()][getCurrentPrey().getInstar()] += assimilatedMass;
	foodMassAssimilatedCurrentTimeStep += assimilatedMass;

	foodMassEatenCurrentTimeStepPerSpecies[getCurrentPrey().getId()][getCurrentPrey().getInstar()] += foodMass;
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


const uint64_t& AnimalNonStatistical::getTotalPredationEncounters() const
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

void AnimalNonStatistical::setCurrentPrey(const std::tuple<Edible*, DryMass, bool>& prey, Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, const bool competitionAmongResourceSpecies, CustomIndexedVector<Species::ID, unsigned int>& predationEventsOnOtherSpecies)
{	
	DryMass preyDryMass = get<0>(prey)->turnIntoDryMass(get<1>(prey), getRemainingVoracity());


	if(get<0>(prey)->getSpecies()->isMobile())
	{
        static_cast<Animal&>(*get<0>(prey)).setNewLifeStage(landscape, LifeStage::PREDATED, numberOfTimeSteps, getId(), timeStepsPerDay);
	}
	else
	{
        static_cast<CellResourceInterface&>(*get<0>(prey)).substractBiomass(preyDryMass, get<2>(prey), getPosition(), getInteractionAreaRadius(), competitionAmongResourceSpecies);
	}


	currentPrey = Prey(get<0>(prey)->getSpecies()->getId(), get<0>(prey)->getGrowthBuildingBlock().getInstar(), preyDryMass, computeHandlingTime(preyDryMass, timeStepsPerDay), timeStepsPerDay);


	++predationEventsOnOtherSpecies[get<0>(prey)->getSpecies()->getId()];
	++eatenToday;
}

void AnimalNonStatistical::setInitialPreferences(const PreciseDouble& timeStepsPerDay)
{
	decisions.setInitialPreferences(timeStepsPerDay);
}

void AnimalNonStatistical::initControlVariables(const vector<Species*>& existingSpecies)
{
	foodMassAssimilatedCurrentTimeStepPerSpecies.resize(existingSpecies.size());
	foodMassEatenCurrentTimeStepPerSpecies.resize(existingSpecies.size());

	for (const Species* const species : existingSpecies) {
		foodMassAssimilatedCurrentTimeStepPerSpecies[species->getId()].resize(species->getGrowthBuildingBlock().getNumberOfInstars(), DryMass(0.0));
		foodMassEatenCurrentTimeStepPerSpecies[species->getId()].resize(species->getGrowthBuildingBlock().getNumberOfInstars(), DryMass(0.0));
	}


	foodMassAssimilatedCurrentTimeStep = DryMass(0.0);
	foodMassEatenCurrentTimeStep = DryMass(0.0);
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
		for(size_t preyInstar = 0; preyInstar < foodMassAssimilatedCurrentTimeStepPerSpecies[speciesId].size(); preyInstar++)
		{
			foodMassAssimilatedCurrentTimeStepPerSpecies[speciesId][preyInstar] = DryMass(0.0);
			foodMassEatenCurrentTimeStepPerSpecies[speciesId][preyInstar] = DryMass(0.0);
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

	actionsCurrentTimeStep = 0u;

	eatenToday = 0;
	distanceTravelled = 0.0;
	stepsAttempted = 0;
	exhausted = false;


	voracity = 0.0;
	sated = false;
}

void AnimalNonStatistical::increaseDistanceTravelled(const PreciseDouble& distanceToAdd)
{
	distanceTravelled += distanceToAdd;

	exhausted = (distanceTravelled >= getSearchAreaRadius());
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

PreciseDouble AnimalNonStatistical::calculatePredationProbability(const Edible& prey, const DryMass& preyDryMass)
{
	if(getId() != prey.getId())
	{
		if(!isSated() && !isExhausted())
		{
			return decisions.calculatePredationProbability(prey, preyDryMass);
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

PreciseDouble AnimalNonStatistical::calculateCellQuality(const AnimalNonStatistical& prey)
{
	if(getId() != prey.getId())
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

PreciseDouble AnimalNonStatistical::calculateEdibilityValue(const AnimalNonStatistical& prey)
{
	if(getId() != prey.getId())
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

	ar & distanceTravelled;

	ar & stepsAttempted;

	ar & totalPredationEncounters;
	ar & predatedByID;

	ar & predationEncountersCurrentDay;

	ar & nextAction;

	ar & actionsCurrentTimeStep;

	ar & timeStepsWithoutFood;
	ar & sated;
	ar & ageOfLastMoultOrReproduction;
	ar & dateOfDeath;

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
