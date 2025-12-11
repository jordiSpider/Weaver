
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalNonStatisticalGrowth.h"

#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"

using namespace std;
using json = nlohmann::json;


AnimalNonStatisticalGrowth::AnimalNonStatisticalGrowth()
    : AnimalGrowth()
{

}

AnimalNonStatisticalGrowth::AnimalNonStatisticalGrowth(AnimalSpeciesGrowth* const speciesGrowth, const Instar &instar, const bool temperatureDependent, const TimeStep& actualTimeSteps, Gender gender, const std::vector<IndividualTrait>& baseIndividualTraits, const PreciseDouble& factorEggMassValue, const unsigned int femaleMaxReproductionEvents, const Temperature& actualTemperature, const Temperature& tempFromLab, const PreciseDouble& timeStepsPerDay)
    : AnimalGrowth(speciesGrowth, instar, temperatureDependent, gender, baseIndividualTraits, factorEggMassValue, femaleMaxReproductionEvents, actualTemperature, tempFromLab, timeStepsPerDay), inHabitatShift(false)
{   
    pupaTimer = TimeStep(0);

    diapauseTimeSteps = TimeStep(0);

	tankAtGrowth = DryMass(0.0);

	calculatedFirstReproductionTargets = false;

	calculatedMassAtMaturation = false;

	actualMoltingTimeVector.reserve(getSpeciesGrowth()->getNumberOfInstars()-1);
	actualMoltingMassVector.reserve(getSpeciesGrowth()->getNumberOfInstars()-1);

	setDateEgg(Day(actualTimeSteps, timeStepsPerDay));

	capitalBreeding = false;

	massForNextReproduction = DryMass(0.0);

	ageForNextReproduction = TimeStep(0);
}

AnimalNonStatisticalGrowth::~AnimalNonStatisticalGrowth()
{
    
}

void AnimalNonStatisticalGrowth::calculateTotalReproMass()
{
	totalReproMass = DryMass(0.0);
	for(Instar instarValue = getSpeciesGrowth()->getInstarFirstReproduction(); instarValue <= getSpeciesGrowth()->getLastInstar(); ++instarValue)
	{
		totalReproMass = totalReproMass + getInstarMass(instarValue);
	}
}

const DryMass& AnimalNonStatisticalGrowth::getPreviousReproMassTarget() const
{
	return previousReproMassTarget;
}

const DryMass& AnimalNonStatisticalGrowth::getPreviousMoultingMassTarget() const
{
	return previousMoultingMassTarget;
}

void AnimalNonStatisticalGrowth::tune(const std::vector<IndividualTrait>& baseIndividualTraits, const TimeStep actualTimeStep, const unsigned int femaleMaxReproductionEvents, const Temperature& actualTemperature, const Temperature& tempFromLab, const PreciseDouble &timeStepsPerDay)
{
	calculateGrowthModel(baseIndividualTraits, actualTemperature, tempFromLab, timeStepsPerDay);

    calculateGrowthCurves(baseIndividualTraits, femaleMaxReproductionEvents, timeStepsPerDay);
}

const CustomIndexedVector<Instar, Length>& AnimalNonStatisticalGrowth::getInstarLengthsVector() const
{
    return instarLengthsVector;
}

const Length& AnimalNonStatisticalGrowth::getInstarLength(const Instar& instarValue) const
{
    return instarLengthsVector[instarValue-1];
}

const CustomIndexedVector<Instar, DryMass>& AnimalNonStatisticalGrowth::getInstarMassesVector() const
{
    return instarMassesVector;
}

const DryMass& AnimalNonStatisticalGrowth::getInstarMass(const Instar& instarValue) const
{
    return instarMassesVector[instarValue-1];
}

const TimeStep AnimalNonStatisticalGrowth::getAgeOfFirstMaturation(const TimeStep& pupaPeriodTime) const
{
    return getInstarAge(getSpeciesGrowth()->getInstarFirstMaturation()) + pupaPeriodTime;
}

void AnimalNonStatisticalGrowth::calculateLongevity(const PreciseDouble& longevitySinceMaturationValue, const TimeStep& pupaPeriodTime)
{
    longevity = longevitySinceMaturationValue * getAgeOfFirstMaturation(pupaPeriodTime);
}

void AnimalNonStatisticalGrowth::calculateClutchDryMass(const DryMass& individualDryMass)
{
	clutchDryMass = DryMass(getSpeciesGrowth()->calculateEggsPerBatch(individualDryMass)*getEggDryMassAtBirth().getValue());
}

void AnimalNonStatisticalGrowth::calculateClutchDryMass(const PreciseDouble& currentEggs)
{
	clutchDryMass = DryMass(currentEggs*getEggDryMassAtBirth().getValue());
}

const DryMass& AnimalNonStatisticalGrowth::getClutchDryMass() const
{
	return clutchDryMass;
}

bool AnimalNonStatisticalGrowth::hasCapitalBreeding() const
{
	return capitalBreeding;
}

void AnimalNonStatisticalGrowth::disableCapitalBreeding()
{
	capitalBreeding = false;
}

bool AnimalNonStatisticalGrowth::isInHabitatShift() const
{
	return inHabitatShift;
}

void AnimalNonStatisticalGrowth::moveOnNextInstar(Landscape* const landscape, AnimalNonStatistical* animal, const PreciseDouble& timeStepsPerDay)
{
	Instar actualInstar = getInstar();
	actualInstar.moveOnNextInstar();
	setInstar(landscape, actualInstar, animal, timeStepsPerDay);
}

void AnimalNonStatisticalGrowth::setAgeForNextReproduction(const TimeStep& newAgeForNextReproduction)
{
	ageForNextReproduction = newAgeForNextReproduction;
}

const TimeStep& AnimalNonStatisticalGrowth::getAgeForNextReproduction() const
{
	return ageForNextReproduction;
}

const DryMass& AnimalNonStatisticalGrowth::getNextMassPredicted() const
{
	return nextMassPredicted;
}

const DryMass& AnimalNonStatisticalGrowth::getPreviousNextMassPredicted() const
{
	return previousNextMassPredicted;
}

const DryMass AnimalNonStatisticalGrowth::calculatePredVor(const DryMass& basalMetabolicDryMassLoss, const PreciseDouble& assim, const DryMass& previousNetGrowth, const TimeStep numberOfTimeSteps)
{
	DryMass predVor;

	DryMass deficit;

	if(numberOfTimeSteps > TimeStep(0))
	{
		deficit = getPreviousNextMassPredicted() - previousNetGrowth;

		if(deficit < 0.0)
		{
			deficit = DryMass(0.0);
		}
	}
	else
	{
		deficit = DryMass(0.0);
	}


	if(!hasCapitalBreeding() || !isMature())
	{
		predVor = DryMass((getNextMassPredicted() - getCurrentTotalDryMass() + basalMetabolicDryMassLoss + deficit).getValue() / assim);
	}
	else
	{
		predVor = DryMass(0.0);
	}

	predVor = DryMass(fmax(predVor.getValue(), 0.0));

	
	return predVor;
}

DryMass AnimalNonStatisticalGrowth::calculatePreviousTarget(const Gender gender, const unsigned int reproCounter) const
{
	if(isMature())
	{
		switch(gender) {
			case Gender::FEMALE: {
				if(reproCounter == 0)
				{
					return getPreviousMoultingMassTarget();
				}
				else
				{
					return getPreviousReproMassTarget();
				}
				break;
			}
			case Gender::MALE: {
				if(getSpeciesGrowth()->hasIndeterminateGrowth() && getInstar() < getSpeciesGrowth()->getLastInstar())
				{
					return getPreviousMoultingMassTarget();
				}
				else
				{
					return getPreviousMoultingMassTarget();
				}
				break;
			}
			case Gender::HERMAPHRODITE: {
				throwLineInfoException("Without implementation");
				break;
			}
			default: {
				throwLineInfoException("Default case");
				break;
			}
		}
	}
	else
	{
		if(getInstar() == 1)
		{
			return getEggDryMassAtBirth();
		}
		else
		{
			return getPreviousMoultingMassTarget();
		}
	}
}

DryMass AnimalNonStatisticalGrowth::calculateNextTarget(const Gender gender) const
{
	if(isMature())
	{
		switch(gender) {
			case Gender::FEMALE: {
				return getMassForNextReproduction();
				break;
			}
			case Gender::MALE: {
				if(getSpeciesGrowth()->hasIndeterminateGrowth() && getInstar() < getSpeciesGrowth()->getLastInstar())
				{
					return getInstarMass(getInstar()+1);
				}
				else
				{
					return getPreviousMoultingMassTarget();
				}
				break;
			}
			case Gender::HERMAPHRODITE: {
				throwLineInfoException("Without implementation");
				break;
			}
			default: {
				throwLineInfoException("Default case");
				break;
			}
		}
	}
	else
	{
		return getInstarMass(getInstar()+1);
	}
}

DryMass AnimalNonStatisticalGrowth::calculateMassLoad(const Gender gender, const unsigned int reproCounter) const
{
	DryMass previousTarget = calculatePreviousTarget(gender, reproCounter);
	DryMass nextTarget = calculateNextTarget(gender);

	if(nextTarget == previousTarget)
	{
		return DryMass(0.0);
	}
	else
	{
		return DryMass((getCurrentTotalDryMass() - previousTarget).getValue() / (nextTarget - previousTarget).getValue());
	}
}

void AnimalNonStatisticalGrowth::calculateNextMassPredicted(const unsigned int reproCounter, const unsigned int maxReproductionEvents, const PreciseDouble& growthValue, const Gender gender, const PreciseDouble& timeStepsPerDay)
{
	calculateNextMassPredicted(getCurrentTotalDryMass(), getCurrentAge(), reproCounter, maxReproductionEvents, growthValue, gender, timeStepsPerDay);
}

void AnimalNonStatisticalGrowth::calculateNextMassPredicted(const DryMass& dryMass, const TimeStep& age, const unsigned int reproCounter, const unsigned int maxReproductionEvents, const PreciseDouble& growthValue, const Gender gender, const PreciseDouble& timeStepsPerDay)
{
	previousNextMassPredicted = getNextMassPredicted();

	if(isMature())
	{
		if(gender == Gender::FEMALE)
		{
			if(reproCounter < maxReproductionEvents)
			{
				if(hasCapitalBreeding())
				{
					nextMassPredicted = dryMass;
				}
				else
				{
					if(getMassForNextReproduction() > dryMass && getAgeForNextReproduction() > age)
					{
						PreciseDouble slopeTarget = (getMassForNextReproduction() - dryMass).getValue()/static_cast<double>((getAgeForNextReproduction()-age).getValue());
						PreciseDouble interceptTarget = dryMass.getValue()-slopeTarget*static_cast<double>(age.getValue());
						nextMassPredicted = DryMass(interceptTarget + slopeTarget*static_cast<double>((age+TimeStep(1)).getValue()));
					}
					else
					{
						nextMassPredicted = dryMass;
					}
				}
			}
			else
			{
				nextMassPredicted = dryMass;
			}
		}
		else if(gender == Gender::MALE)
		{
			if(getSpeciesGrowth()->hasIndeterminateGrowth() && getInstar() < getSpeciesGrowth()->getLastInstar())
			{
				Length nextLengthPredicted = getIndividualGrowthModel()->calculateLength(Day(age + TimeStep(1), timeStepsPerDay), growthValue);
				nextMassPredicted = getSpeciesGrowth()->calculateDryMass(nextLengthPredicted, isMature());
			}
			else
			{
				nextMassPredicted = previousNextMassPredicted;
			}
		}
	}
	else
	{
		Length nextLengthPredicted = getIndividualGrowthModel()->calculateLength(Day(age + TimeStep(1), timeStepsPerDay), growthValue);
		nextMassPredicted = getSpeciesGrowth()->calculateDryMass(nextLengthPredicted, isMature());
	}
}

bool AnimalNonStatisticalGrowth::meetMoltingTargets() const
{
	TimeStep maxAgeTarget = getInstarAge(getInstar()+1) * (1.0+getSpeciesGrowth()->getMoltingAgeThreshold());

	if(getCurrentTotalDryMass() >= getInstarMass(getInstar()+1) && getCurrentAge() >= getInstarAge(getInstar()+1))
	{
		return true;
	}
	else
	{
		if(getCurrentAge() >= maxAgeTarget)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void AnimalNonStatisticalGrowth::applyMoulting(const TimeStep actualTimeStep, const PreciseDouble& energyTankFinalTraitValue, Gender gender)
{
	if(getCurrentTotalDryMass() <= getInstarMass(getInstar()+1))
	{
		setDryMass(getCurrentTotalDryMass(), getCurrentTotalDryMass(), energyTankFinalTraitValue, true, gender, true, actualTimeStep);
	}
	else
	{
		setDryMass(getInstarMass(getInstar()+1), getCurrentTotalDryMass(), energyTankFinalTraitValue, true, gender, true, actualTimeStep);
	}


	if(!isMature())
	{
		previousMoultingMassTarget = getCurrentTotalDryMass();
	}
}

//TODO parametro para que se ejecuten cada X timesteps
pair<pair<bool, TimeStep>, pair<bool, LifeStage>> AnimalNonStatisticalGrowth::grow(Landscape* const landscape, AnimalNonStatistical* animal, Gender gender, const TimeStep& pupaPeriodTime, const unsigned int reproCounter, const unsigned int femaleMaxReproductionEvents, const PreciseDouble& growthValue, const PreciseDouble& totFec, const SexualType sexualType, const bool isMated, const TimeStep actualTimeStep, const PreciseDouble& energyTankFinalTraitValue, const PreciseDouble& timeStepsPerDay)
{
	pair<pair<bool, TimeStep>, pair<bool, LifeStage>> result = make_pair<>(make_pair<>(false, TimeStep(0)), make_pair<>(false, LifeStage::ACTIVE));


	if(landscape->isDinosaurs() && getInstar() < getSpeciesGrowth()->getNumberOfInstars()){
		const Length lengthPredicted = getIndividualGrowthModel()->calculateLength(Day(getCurrentAge(), timeStepsPerDay), growthValue);

		const DryMass massPredicted = getSpeciesGrowth()->calculateDryMass(lengthPredicted, isMature());

		if(getCurrentTotalDryMass() > massPredicted){	
			setDryMass(getCurrentTotalDryMass(), getCurrentTotalDryMass(), energyTankFinalTraitValue, false, gender, true, actualTimeStep);
		}
	}


	if(isMature())
	{
		if(getSpeciesGrowth()->hasIndeterminateGrowth() && getInstar() < getSpeciesGrowth()->getLastInstar())
		{
			bool moltingResult = meetMoltingTargets();

			if(moltingResult)
			{
				applyMoulting(actualTimeStep, energyTankFinalTraitValue, gender);

				moveOnNextInstar(landscape, animal, timeStepsPerDay);
				actualMoltingTimeVector.push_back(getCurrentAge() + TimeStep(1));
				result.first = make_pair<>(true, getCurrentAge());

				if(gender == Gender::FEMALE)
				{
					calculateMatureNextReproductionTargets(pupaPeriodTime, femaleMaxReproductionEvents, reproCounter, growthValue, totFec, false, timeStepsPerDay);
				}
			}
		}

		auto reproResult = checkReproductionTargets(gender, sexualType, isMated);
		
		if(reproResult.first.first)
		{
			result = reproResult;
		}
	}
	else
	{
		bool moltingResult = meetMoltingTargets();

		if(moltingResult)
		{
			applyMoulting(actualTimeStep, energyTankFinalTraitValue, gender);

			setLastMassBeforeMaturationOrOviposition(getCurrentTotalDryMass());
		
			tankAtGrowth = getCurrentEnergyTank();
			
			moveOnNextInstar(landscape, animal, timeStepsPerDay);
			actualMoltingTimeVector.push_back(getCurrentAge() + TimeStep(1));
			result.first = make_pair<>(true, getCurrentAge());

			setMatureValue(getInstar() >= getSpeciesGrowth()->getInstarFirstMaturation(), gender);


			if(isMature())
			{
                //TODO TIMESTEP
				//This gets decremented in activateAnimals at the beginning of each day.
				pupaTimer = pupaPeriodTime;
				result.second = make_pair<>(true, LifeStage::PUPA);

				if(gender == Gender::FEMALE)
				{
					calculateMatureNextReproductionTargets(pupaPeriodTime, femaleMaxReproductionEvents, reproCounter, growthValue, totFec, true, timeStepsPerDay);
				}
			}
		}
	}

	return result;
}

bool AnimalNonStatisticalGrowth::isReadyToResumeFromPupaOrDecreasePupaTimer()
{
	pupaTimer = pupaTimer - TimeStep(1);

	return pupaTimer == TimeStep(0);
}

pair<bool, bool> AnimalNonStatisticalGrowth::isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(const PreciseDouble& relativeHumidity, const PreciseDouble& minRelativeHumidityThreshold)
{
	if(relativeHumidity >= minRelativeHumidityThreshold)
	{
		if(pupaTimer > TimeStep(0))
		{
			return make_pair<>(true, true);
		}
		else
		{
			return make_pair<>(true, false);
		}
	}
	else
	{
		diapauseTimeSteps = diapauseTimeSteps + TimeStep(1);
		return make_pair<>(false, false);
	}
}

const TimeStep AnimalNonStatisticalGrowth::getDiapauseTimeSteps() const
{
    return diapauseTimeSteps;
}

const TimeStep& AnimalNonStatisticalGrowth::getLongevity() const
{
    return longevity;
}

void AnimalNonStatisticalGrowth::setInHabitatShift(const bool newInHabitatShiftValue)
{
	inHabitatShift = newInHabitatShiftValue;
}

void AnimalNonStatisticalGrowth::calculateGrowthCurves(const std::vector<IndividualTrait>& baseIndividualTraits, const unsigned int femaleMaxReproductionEvents, const PreciseDouble& timeStepsPerDay)
{
	AnimalGrowth::calculateGrowthCurves(baseIndividualTraits, femaleMaxReproductionEvents, timeStepsPerDay);

	TimeStep pupaPeriodTime(Day(baseIndividualTraits[static_cast<size_t>(BaseTraitType::pupaPeriodTime)].getPhenotypicValue()), timeStepsPerDay);

	calculateLongevity(baseIndividualTraits[static_cast<size_t>(BaseTraitType::longevitySinceMaturation)].getPhenotypicValue(), pupaPeriodTime);

	calculateTotalReproMass();
}

void AnimalNonStatisticalGrowth::setInstar(Landscape* const landscape, const Instar& newInstar, AnimalNonStatistical* animal, const PreciseDouble& timeStepsPerDay)
{
	animal->getMutableTerrainCell()->eraseAnimal(animal);

	EdibleGrowth::setInstar(newInstar);

	animal->setInstarToEvaluateCells(newInstar);

	animal->updateDepth(timeStepsPerDay);

	animal->updateVariablesAssociatedWithInstar();

	setInHabitatShift(getSpeciesGrowth()->habitatShiftOccurs(getInstar()));

	animal->getMutableTerrainCell()->insertAnimal(landscape, animal);
}

pair<pair<bool, TimeStep>, pair<bool, LifeStage>> AnimalNonStatisticalGrowth::checkReproductionTargets(Gender gender, const SexualType sexualType, const bool isMated)
{
	pair<pair<bool, TimeStep>, pair<bool, LifeStage>> result = make_pair<>(make_pair<>(false, TimeStep(0)), make_pair<>(false, LifeStage::ACTIVE));

	if(gender == Gender::FEMALE)
	{
		bool reproResult = meetReproductionTargets();

		if(reproResult)
		{
			if(sexualType != SexualType::diploid || 
				(sexualType == SexualType::diploid && isMated))
			{
				result.first = make_pair<>(true, getCurrentAge());
				result.second = make_pair<>(true, LifeStage::REPRODUCING);
			}
		}
	}

	return result;
}

bool AnimalNonStatisticalGrowth::meetReproductionTargets() const
{
	TimeStep maxAgeTarget = getAgeForNextReproduction() * (1.0+getSpeciesGrowth()->getMoltingAgeThreshold());

	if(hasCapitalBreeding())
	{
		if(getCurrentAge() >= getAgeForNextReproduction())
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
		if(getCurrentTotalDryMass() >= getMassForNextReproduction() && getCurrentAge() >= getAgeForNextReproduction())
		{
			return true;
		}
		else
		{
			if(getCurrentAge() >= maxAgeTarget)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}

void AnimalNonStatisticalGrowth::calculateMatureNextReproductionTargets(const TimeStep& pupaPeriodTime, const unsigned int femaleMaxReproductionEvents, const unsigned int reproCounter, const PreciseDouble& growthValue, const PreciseDouble& totFec, const bool updatePreviousReproMassTarget, const PreciseDouble& timeStepsPerDay)
{
	if(reproCounter > 0 && updatePreviousReproMassTarget)
	{
		previousReproMassTarget = getCurrentTotalDryMass();
	}


	if(hasCapitalBreeding() && 
		reproCounter < femaleMaxReproductionEvents && 
		reproCounter >= getSpeciesGrowth()->getNumberOfCapitalBreeds())
	{
		disableCapitalBreeding();
	}


	TimeStep timeOfReproEvent;

	if(getSpeciesGrowth()->hasCapitalBreeding())
	{
		if(femaleMaxReproductionEvents == getSpeciesGrowth()->getNumberOfCapitalBreeds()) {
			timeOfReproEvent = TimeStep(0);
		}
		else {
			// because the first one is on the pupaPerdiodLength
			TimeStep totalTimeBreedingCapitally = getSpeciesGrowth()->getTimeOfReproEventDuringCapitalBreeding()*(getSpeciesGrowth()->getNumberOfCapitalBreeds()-1);
			timeOfReproEvent = (getLongevity() - totalTimeBreedingCapitally - getAgeOfFirstMaturation(pupaPeriodTime) - TimeStep(2*femaleMaxReproductionEvents)) / (femaleMaxReproductionEvents - getSpeciesGrowth()->getNumberOfCapitalBreeds());
		}
	}
	else
	{
		timeOfReproEvent = (getLongevity() - getAgeOfFirstMaturation(pupaPeriodTime) - TimeStep(2*femaleMaxReproductionEvents)) / femaleMaxReproductionEvents;
	}

	if(timeOfReproEvent < TimeStep(0)){
		throwLineInfoException("'timeOfReproEvent' must have a positive value.");
	}


	DryMass currentMass = getCurrentTotalDryMass();


	if(!hasCapitalBreeding() && !getSpeciesGrowth()->hasIndeterminateGrowth() && !calculatedMassAtMaturation)
	{
		massAtMaturation = currentMass;

		calculateClutchDryMass(massAtMaturation);

		calculatedMassAtMaturation = true;
	}


	if(hasCapitalBreeding())
	{
		if(reproCounter == 0 &&
			!calculatedFirstReproductionTargets)
		{
			calculateClutchDryMass(currentMass);

			setAgeForNextReproduction(getCurrentAge());
			setMassForNextReproduction(currentMass);

			calculatedFirstReproductionTargets = true;
		}

		if(getAgeForNextReproduction() < getCurrentAge())
		{
			setAgeForNextReproduction(getCurrentAge() + getSpeciesGrowth()->getTimeOfReproEventDuringCapitalBreeding());
			setMassForNextReproduction(currentMass);
		}
	}
	else
	{
		if(getSpeciesGrowth()->hasIndeterminateGrowth())
		{
			if(reproCounter == 0 &&
				!calculatedFirstReproductionTargets)
			{
				setAgeForNextReproduction(getAgeOfFirstMaturation(pupaPeriodTime) + timeOfReproEvent);

				calculatedFirstReproductionTargets = true;
			}
			else
			{
				setAgeForNextReproduction(getCurrentAge() + timeOfReproEvent);
			}


			if(getInstarAge(getSpeciesGrowth()->getLastInstar()) <= getAgeForNextReproduction())
			{
				massAtMaturation = getInstarMass(getSpeciesGrowth()->getLastInstar());

				calculateClutchDryMass(massAtMaturation);

				calculatedMassAtMaturation = true;
			}
			


			DryMass nextMass;

			if(calculatedMassAtMaturation)
			{
				nextMass = massAtMaturation;
			}
			else
			{
				Length nextLength = getIndividualGrowthModel()->calculateLength(Day(getAgeForNextReproduction(), timeStepsPerDay), growthValue);
				nextMass = getSpeciesGrowth()->calculateDryMass(nextLength, isMature());

				PreciseDouble fractFirstRepro = getInstarMass(getSpeciesGrowth()->getInstarFirstReproduction()+reproCounter).getValue()/getTotalReproMass().getValue();

				PreciseDouble currentEggs = fractFirstRepro*totFec;

				calculateClutchDryMass(currentEggs);
			}

			setMassForNextReproduction(nextMass + getClutchDryMass());
		}
		else
		{
			if(reproCounter == 0 && !calculatedFirstReproductionTargets)
			{
				calculateClutchDryMass(massAtMaturation);

				setAgeForNextReproduction(getAgeOfFirstMaturation(pupaPeriodTime) + timeOfReproEvent);
				setMassForNextReproduction(massAtMaturation + getClutchDryMass());

				calculatedFirstReproductionTargets = true;
			}
			else
			{
				// No se si hay que volver a calcular el ClutchDryMass

				setAgeForNextReproduction(getCurrentAge() + timeOfReproEvent);
				setMassForNextReproduction(massAtMaturation + getClutchDryMass());
			}
		}
	}
}

void AnimalNonStatisticalGrowth::setMatureValue(const bool isMature, const Gender& gender)
{
	if(isMature)
	{
		switch(gender) {
			case Gender::FEMALE: {
				capitalBreeding = getSpeciesGrowth()->hasCapitalBreeding();
				break;
			}
			case Gender::MALE: {
				capitalBreeding = false;
				break;
			}
			case Gender::HERMAPHRODITE: {
				throwLineInfoException("Without implementation");
				break;
			}
			default: {
				throwLineInfoException("Default case");
				break;
			}
		}
	}

	AnimalGrowth::setMatureValue(isMature, gender);
}

pair<TimeStep, pair<bool, LifeStage>> AnimalNonStatisticalGrowth::forceMolting(const std::vector<IndividualTrait>& baseIndividualTraits, const TimeStep actualTimeStep, Gender gender, const PreciseDouble& timeStepsPerDay)
{
	pair<TimeStep, pair<bool, LifeStage>> result;
	result.second = make_pair<>(false, LifeStage::ACTIVE);

	ofstream noStream;

	TimeStep ageAtInitialization;

	if(getInstar() == 1)
	{
		ageAtInitialization = Random::randomTimeStepInRange(TimeStep(0), getInstarAge(Instar(2)));
	}
	else if((getInstar() > 1) && (getInstar() < getSpeciesGrowth()->getNumberOfInstars()))
	{
		ageAtInitialization = Random::randomTimeStepInRange(getInstarAge(getInstar()), getInstarAge(getInstar()+1));
	}
	else
	{
		ageAtInitialization = Random::randomTimeStepInRange(getInstarAge(getInstar()), getLongevity());
	}


	if(getInstar() > 1)
	{
		for(Instar actualInstar = Instar(2); actualInstar <= getInstar(); ++actualInstar)
		{
			actualMoltingTimeVector.push_back(getInstarAge(actualInstar));
			actualMoltingMassVector.push_back(getInstarMass(actualInstar));
		}
	}


	setCurrentAge(ageAtInitialization);


	//La edad del animal en negativo para que los calculos de tuneTraits sean correctos
	setDateEgg(Day(ageAtInitialization + TimeStep(Day(baseIndividualTraits[static_cast<size_t>(BaseTraitType::eggDevTime)].getPhenotypicValue()), timeStepsPerDay), timeStepsPerDay) * -1);


	result.first = (getInstar() > 1) ? getInstarAge(getInstar()) : TimeStep(0);

	if(getInstar() >= getSpeciesGrowth()->getInstarFirstMaturation())
	{
		setMatureValue(true, gender);

		setLastMassBeforeMaturationOrOviposition(getInstarMass(getSpeciesGrowth()->getInstarFirstMaturation()));
	}




	Length expectedLengthAtCurrentAge = getIndividualGrowthModel()->calculateLength(Day(ageAtInitialization, timeStepsPerDay), baseIndividualTraits[static_cast<size_t>(BaseTraitType::growth)].getPhenotypicValue());
	DryMass expectedMassAtCurrentAge = getSpeciesGrowth()->calculateDryMass(expectedLengthAtCurrentAge, isMature());

	if(getInstar() == 1)
	{
		setDryMass(getEggDryMassAtBirth(), getEggDryMassAtBirth(), baseIndividualTraits[static_cast<size_t>(BaseTraitType::energy_tank)].getPhenotypicValue(), false, gender, false, actualTimeStep);
	}
	else
	{
		setDryMass(getInstarMass(getInstar()), getInstarMass(getInstar()), baseIndividualTraits[static_cast<size_t>(BaseTraitType::energy_tank)].getPhenotypicValue(), true, gender, false, actualTimeStep);
	}

	if(getInstar() < getSpeciesGrowth()->getLastInstar())
	{
		modifyEnergyTank(expectedMassAtCurrentAge - getCurrentBodySize(), actualTimeStep);
	}


	if(getInstar() > 1)
	{
		previousMoultingMassTarget = getInstarMass(getInstar());
	}
	



	if(isMature())
	{
		TimeStep pupaPeriodTime(Day(baseIndividualTraits[static_cast<size_t>(BaseTraitType::pupaPeriodTime)].getPhenotypicValue()), timeStepsPerDay);

		if(getCurrentAge() < getAgeOfFirstMaturation(pupaPeriodTime))
		{
			pupaTimer = getAgeOfFirstMaturation(pupaPeriodTime) - getCurrentAge();
			result.second = make_pair<>(true, LifeStage::PUPA);
		}


		result.first = ageAtInitialization;
	}

	return result;
}

void AnimalNonStatisticalGrowth::forceReproduction(const unsigned int reproCounter, const unsigned int femaleMaxReproductionEvents, const TimeStep& timeOfReproEvent, const TimeStep ageOfLastMoultOrReproduction, const TimeStep& pupaPeriodTime, const PreciseDouble& factorEggMassValue, const TimeStep actualTimeStep, const PreciseDouble& growthValue, const PreciseDouble& totFec, const bool lastEventIsMoult, const PreciseDouble& timeStepsPerDay)
{
	// setAgeForNextReproduction
	if(hasCapitalBreeding())
	{
		if(reproCounter < getSpeciesGrowth()->getNumberOfCapitalBreeds())
		{
			if(reproCounter == 0)
			{
				setAgeForNextReproduction(getCurrentAge());
			}
			else
			{
				setAgeForNextReproduction(ageOfLastMoultOrReproduction + getSpeciesGrowth()->getTimeOfReproEventDuringCapitalBreeding());
			}
		}
		else
		{
			setAgeForNextReproduction(ageOfLastMoultOrReproduction + timeOfReproEvent);
		}
	}
	else
	{
		if(reproCounter == 0)
		{
			setAgeForNextReproduction(getAgeOfFirstMaturation(pupaPeriodTime) + timeOfReproEvent);
		}
		else if(reproCounter < femaleMaxReproductionEvents)
		{
			setAgeForNextReproduction(ageOfLastMoultOrReproduction + timeOfReproEvent);
		}
	}



	// previousReproMassTarget
	if(reproCounter > 0)
	{
		previousReproMassTarget = getCurrentTotalDryMass();
	}



	// setMassForNextReproduction
	if(hasCapitalBreeding())
	{
		DryMass massAfterCapitalBreeding = getInstarMass(getSpeciesGrowth()->getInstarFirstMaturation());

		calculateClutchDryMass(massAfterCapitalBreeding);

		unsigned int currentEggBatchNumber = computeEggBatchNumber(factorEggMassValue);

		DryMass totalOffspringDryMass(currentEggBatchNumber*(getSpeciesGrowth()->getEggDryMass() + getSpeciesGrowth()->getEggDryMass().getValue()*factorEggMassValue).getValue());

		unsigned int reproToForce = min(reproCounter, getSpeciesGrowth()->getNumberOfCapitalBreeds());

		for(unsigned int i = 0; i < reproToForce; i++)
		{
			massAfterCapitalBreeding = massAfterCapitalBreeding - totalOffspringDryMass;
		}

		if(reproCounter < getSpeciesGrowth()->getNumberOfCapitalBreeds())
		{
			modifyEnergyTank(massAfterCapitalBreeding - getCurrentBodySize(), actualTimeStep);

			setMassForNextReproduction(getCurrentTotalDryMass());

			if(reproCounter > 0)
			{
				previousReproMassTarget = massAfterCapitalBreeding + totalOffspringDryMass;
			}
		}
		else
		{
			if(getSpeciesGrowth()->hasIndeterminateGrowth())
			{
				if(getInstarAge(getSpeciesGrowth()->getLastInstar()) <= getAgeForNextReproduction())
				{
					TimeStep totalTimeBreedingCapitally = getSpeciesGrowth()->getTimeOfReproEventDuringCapitalBreeding() * (getSpeciesGrowth()->getNumberOfCapitalBreeds()-1);

					if((getAgeOfFirstMaturation(pupaPeriodTime) + totalTimeBreedingCapitally) >= getInstarAge(getSpeciesGrowth()->getLastInstar()))
					{
						massAtMaturation = massAfterCapitalBreeding;
					}
					else
					{
						massAtMaturation = getInstarMass(getSpeciesGrowth()->getLastInstar());
					}

					calculateClutchDryMass(massAtMaturation);

					calculatedMassAtMaturation = true;
				}




				DryMass nextMass;

				if(calculatedMassAtMaturation)
				{
					nextMass = massAtMaturation;
				}
				else
				{
					Length nextLength = getIndividualGrowthModel()->calculateLength(Day(getAgeForNextReproduction(), timeStepsPerDay), growthValue);
					nextMass = getSpeciesGrowth()->calculateDryMass(nextLength, isMature());

					PreciseDouble fractFirstRepro = getInstarMass(getSpeciesGrowth()->getInstarFirstReproduction()+reproCounter).getValue()/getTotalReproMass().getValue();

					PreciseDouble currentEggs = fractFirstRepro*totFec;

					calculateClutchDryMass(currentEggs);
				}

				setMassForNextReproduction(nextMass + getClutchDryMass());

				if(reproCounter == getSpeciesGrowth()->getNumberOfCapitalBreeds())
				{
					previousReproMassTarget = massAfterCapitalBreeding;
				}
				else
				{
					if(getInstarAge(getSpeciesGrowth()->getLastInstar()) <= (getAgeForNextReproduction() - timeOfReproEvent))
					{
						previousReproMassTarget = massAtMaturation;
					}
					else
					{
						Length previousReproLength = getIndividualGrowthModel()->calculateLength(Day(getAgeForNextReproduction() - timeOfReproEvent, timeStepsPerDay), growthValue);
						previousReproMassTarget = getSpeciesGrowth()->calculateDryMass(previousReproLength, isMature());
					}
				}



				TimeStep initialAge = ageOfLastMoultOrReproduction;
				DryMass initialDryMass;

				if(lastEventIsMoult)
				{
					initialDryMass = getCurrentTotalDryMass();
				}
				else
				{
					if(reproCounter == getSpeciesGrowth()->getNumberOfCapitalBreeds())
					{
						initialDryMass = massAfterCapitalBreeding;
					}
					else
					{
						Length initialLength = getIndividualGrowthModel()->calculateLength(Day(getAgeForNextReproduction() - timeOfReproEvent, timeStepsPerDay), growthValue);
						initialDryMass = getSpeciesGrowth()->calculateDryMass(initialLength, isMature());
					}
				}



				DryMass finalDryMass = initialDryMass;

				for(TimeStep age = initialAge; age < getCurrentAge(); age = age + TimeStep(1))
				{
					calculateNextMassPredicted(finalDryMass, age, reproCounter, femaleMaxReproductionEvents, growthValue, Gender::FEMALE, timeStepsPerDay);

					finalDryMass = getNextMassPredicted();
				}

				modifyEnergyTank(getCurrentEnergyTank() + (finalDryMass - getCurrentTotalDryMass()), actualTimeStep);
			}
			else
			{
				modifyEnergyTank(massAfterCapitalBreeding - getCurrentBodySize(), actualTimeStep);

				massAtMaturation = getCurrentTotalDryMass();

				calculatedMassAtMaturation = true;

				calculateClutchDryMass(massAtMaturation);

				setMassForNextReproduction(massAtMaturation + getClutchDryMass());

				previousReproMassTarget = massAtMaturation;


				TimeStep initialAge = ageOfLastMoultOrReproduction;


				for(TimeStep age = initialAge; age < getCurrentAge(); age = age + TimeStep(1))
				{
					calculateNextMassPredicted(getCurrentTotalDryMass(), age, reproCounter, femaleMaxReproductionEvents, growthValue, Gender::FEMALE, timeStepsPerDay);

					modifyEnergyTank(getCurrentEnergyTank() + (getNextMassPredicted() - getCurrentTotalDryMass()), actualTimeStep);
				}
			}
		}
	}
	else
	{
		if(getSpeciesGrowth()->hasIndeterminateGrowth())
		{
			if(getInstarAge(getSpeciesGrowth()->getLastInstar()) <= getAgeForNextReproduction())
			{
				massAtMaturation = getInstarMass(getSpeciesGrowth()->getLastInstar());

				calculateClutchDryMass(massAtMaturation);

				calculatedMassAtMaturation = true;
			}




			DryMass nextMass;

			if(calculatedMassAtMaturation)
			{
				nextMass = massAtMaturation;
			}
			else
			{
				Length nextLength = getIndividualGrowthModel()->calculateLength(Day(getAgeForNextReproduction(), timeStepsPerDay), growthValue);
				nextMass = getSpeciesGrowth()->calculateDryMass(nextLength, isMature());

				PreciseDouble fractFirstRepro = getInstarMass(getSpeciesGrowth()->getInstarFirstReproduction()+reproCounter).getValue()/getTotalReproMass().getValue();

				PreciseDouble currentEggs = fractFirstRepro*totFec;

				calculateClutchDryMass(currentEggs);
			}

			setMassForNextReproduction(nextMass + getClutchDryMass());

			if(reproCounter > 0)
			{
				if(getInstarAge(getSpeciesGrowth()->getLastInstar()) <= (getAgeForNextReproduction() - timeOfReproEvent))
				{
					previousReproMassTarget = massAtMaturation;
				}
				else
				{
					Length previousReproLength = getIndividualGrowthModel()->calculateLength(Day(getAgeForNextReproduction() - timeOfReproEvent, timeStepsPerDay), growthValue);
					previousReproMassTarget = getSpeciesGrowth()->calculateDryMass(previousReproLength, isMature());
				}
			}



			TimeStep initialAge;
			DryMass initialDryMass;

			if(lastEventIsMoult)
			{
				initialDryMass = getCurrentTotalDryMass();
			}
			else
			{
				Length initialLength = getIndividualGrowthModel()->calculateLength(Day(getAgeForNextReproduction() - timeOfReproEvent, timeStepsPerDay), growthValue);
				initialDryMass = getSpeciesGrowth()->calculateDryMass(initialLength, isMature());
			}



			if(reproCounter == 0)
			{
				initialAge = getAgeOfFirstMaturation(pupaPeriodTime);
			}
			else
			{
				initialAge = ageOfLastMoultOrReproduction;
			}


			DryMass finalDryMass = initialDryMass;

			for(TimeStep age = initialAge; age < getCurrentAge(); age = age + TimeStep(1))
			{
				calculateNextMassPredicted(finalDryMass, age, reproCounter, femaleMaxReproductionEvents, growthValue, Gender::FEMALE, timeStepsPerDay);

				finalDryMass = getNextMassPredicted();
			}

			modifyEnergyTank(getCurrentEnergyTank() + (finalDryMass - getCurrentTotalDryMass()), actualTimeStep);
		}
		else
		{
			massAtMaturation = getCurrentTotalDryMass();

			calculatedMassAtMaturation = true;

			calculateClutchDryMass(massAtMaturation);

			setMassForNextReproduction(massAtMaturation + getClutchDryMass());

			if(reproCounter > 0)
			{
				previousReproMassTarget = massAtMaturation;
			}



			TimeStep initialAge;

			if(reproCounter == 0)
			{
				initialAge = getAgeOfFirstMaturation(pupaPeriodTime);
			}
			else
			{
				initialAge = ageOfLastMoultOrReproduction;
			}


			for(TimeStep age = initialAge; age < getCurrentAge(); age = age + TimeStep(1))
			{
				calculateNextMassPredicted(getCurrentTotalDryMass(), age, reproCounter, femaleMaxReproductionEvents, growthValue, Gender::FEMALE, timeStepsPerDay);

				modifyEnergyTank(getCurrentEnergyTank() + (getNextMassPredicted() - getCurrentTotalDryMass()), actualTimeStep);
			}
		}
	}




	// calculatedFirstReproductionTargets
	calculatedFirstReproductionTargets = true;


	// disableCapitalBreeding
	if(hasCapitalBreeding())
	{
		if(reproCounter >= getSpeciesGrowth()->getNumberOfCapitalBreeds())
		{
			disableCapitalBreeding();
		}
	}
}

const DryMass& AnimalNonStatisticalGrowth::getMassForNextReproduction() const
{
	return massForNextReproduction;
}

void AnimalNonStatisticalGrowth::setMassForNextReproduction(const DryMass& newMassForNextReproduction)
{
	massForNextReproduction = newMassForNextReproduction;
}

const DryMass& AnimalNonStatisticalGrowth::getLastMassBeforeMaturationOrOviposition() const
{
	return lastMassBeforeMaturationOrOviposition;
}

void AnimalNonStatisticalGrowth::setLastMassBeforeMaturationOrOviposition(const DryMass& newLastMassBeforeMaturationOrOviposition)
{
	lastMassBeforeMaturationOrOviposition = newLastMassBeforeMaturationOrOviposition;
}

unsigned int AnimalNonStatisticalGrowth::computeEggBatchNumber(const PreciseDouble& factorEggMassValue) const
{
	return static_cast<unsigned int>(floor(getClutchDryMass().getValue() / (getSpeciesGrowth()->getEggDryMass().getValue() + getSpeciesGrowth()->getEggDryMass().getValue()*factorEggMassValue)).getValue());
}

const DryMass& AnimalNonStatisticalGrowth::getTankAtGrowth() const
{
	return tankAtGrowth;
}

void AnimalNonStatisticalGrowth::setDateEgg(Day newDateEgg) 
{
	dateEgg = newDateEgg;
}

const Day& AnimalNonStatisticalGrowth::getDateEgg() const
{
	return dateEgg;
}

const DryMass& AnimalNonStatisticalGrowth::getTotalReproMass() const
{
	return totalReproMass;
}



BOOST_CLASS_EXPORT(AnimalNonStatisticalGrowth)

template <class Archive>
void AnimalNonStatisticalGrowth::serialize(Archive &ar, const unsigned int) 
{
	ar & boost::serialization::base_object<AnimalGrowth>(*this);

	ar & longevity;

    ar & pupaTimer;

    ar & diapauseTimeSteps;

    ar & massForNextReproduction;

    ar & lastMassBeforeMaturationOrOviposition;

    ar & clutchDryMass;

    ar & tankAtGrowth;

	ar & nextMassPredicted;

	ar & previousNextMassPredicted;

	ar & calculatedFirstReproductionTargets;

	ar & calculatedMassAtMaturation;
	ar & massAtMaturation;

    ar & actualMoltingTimeVector;
	ar & actualMoltingMassVector;

    ar & dateEgg;

    ar & totalReproMass;

	ar & capitalBreeding;

	ar & ageForNextReproduction;

	ar & inHabitatShift;

	ar & previousReproMassTarget;

	ar & previousMoultingMassTarget;
}

// Specialisation
template void AnimalNonStatisticalGrowth::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalNonStatisticalGrowth::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalNonStatisticalGrowth::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalNonStatisticalGrowth::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
