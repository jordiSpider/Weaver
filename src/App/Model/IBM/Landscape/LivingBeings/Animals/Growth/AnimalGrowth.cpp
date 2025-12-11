#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalGrowth.h"


using namespace std;


AnimalGrowth::AnimalGrowth()
    : EdibleGrowth()
{

}

AnimalGrowth::AnimalGrowth(AnimalSpeciesGrowth* const speciesGrowth, const Instar &instar, const bool temperatureDependent, Gender gender, const std::vector<IndividualTrait>& baseIndividualTraits, const PreciseDouble& factorEggMassValue, const unsigned int femaleMaxReproductionEvents, const Temperature& actualTemperature, const Temperature& tempFromLab, const PreciseDouble& timeStepsPerDay)
    : EdibleGrowth(speciesGrowth, instar), individualInstarAgeVector(getSpeciesGrowth()->getInstarAgeVector().size(), TimeStep(0)),
      instarLengthsVector(getSpeciesGrowth()->getInstarAgeVector().size()), 
      instarMassesVector(getSpeciesGrowth()->getInstarAgeVector().size()),
      baseDevTimeProportion(Day(baseIndividualTraits[static_cast<size_t>(BaseTraitType::devTime)].getConstitutiveValue()).getValue() / getSpeciesGrowth()->getInstarAgeVector().back().getValue()),
	  temperatureDependent(temperatureDependent), currentAge(0)
{
	setMatureValue(false, gender);

	#ifdef DEBUG
		currentBodySizeInitialised = false;
		currentBodySizeLastTimeStep = TimeStep(UINT_MAX);

		currentEnergyTankInitialised = false;
		currentEnergyTankLastTimeStep = TimeStep(UINT_MAX);
	#endif

    calculateValuesAtBirth(factorEggMassValue, gender);

	individualGrowthModel = nullptr;
	calculateGrowthModel(baseIndividualTraits, actualTemperature, tempFromLab, timeStepsPerDay);

	calculateGrowthCurves(baseIndividualTraits, femaleMaxReproductionEvents, timeStepsPerDay);
}

AnimalGrowth::~AnimalGrowth()
{
	delete individualGrowthModel;
}

const TimeStep& AnimalGrowth::getInstarAge(const Instar& instarValue) const
{
    return individualInstarAgeVector[instarValue-1];
}

void AnimalGrowth::setCurrentAge(const TimeStep &newCurrentAge)
{
	currentAge = newCurrentAge;
}

const TimeStep& AnimalGrowth::getCurrentAge() const
{
	return currentAge;
}

const CustomIndexedVector<Instar, TimeStep>& AnimalGrowth::getIndividualInstarAgeVector() const
{
    return individualInstarAgeVector;
}

void AnimalGrowth::calculateValuesAtBirth(const PreciseDouble& factorEggMass, Gender gender)
{
    eggDryMassAtBirth = getSpeciesGrowth()->getEggDryMass() + getSpeciesGrowth()->getEggDryMass().getValue()*factorEggMass;

	if(gender == Gender::FEMALE)
	{
		DryMass maxMassAtCurrentAge = eggDryMassAtBirth + eggDryMassAtBirth.getValue()*getSpeciesGrowth()->getMaxPlasticityKVonBertalanffy();
		DryMass minMassAtCurrentAge = eggDryMassAtBirth - eggDryMassAtBirth.getValue()*getSpeciesGrowth()->getMinPlasticityKVonBertalanffy();

		eggDryMassAtBirth = DryMass(fmin(eggDryMassAtBirth.getValue(), maxMassAtCurrentAge.getValue()));
		eggDryMassAtBirth = DryMass(fmax(eggDryMassAtBirth.getValue(), minMassAtCurrentAge.getValue()));
	}

    lengthAtBirth = getSpeciesGrowth()->calculateDryLength(eggDryMassAtBirth, false);
}

const DryMass& AnimalGrowth::getEggDryMassAtBirth() const
{
	return eggDryMassAtBirth;
}

const Length& AnimalGrowth::getLengthAtBirth() const
{
    return lengthAtBirth;
}

const IndividualGrowthModel* AnimalGrowth::getIndividualGrowthModel() const
{
    return individualGrowthModel;
}

bool AnimalGrowth::isTemperatureDependent() const
{
    return temperatureDependent;
}

void AnimalGrowth::calculateGrowthModel(const std::vector<IndividualTrait>& baseIndividualTraits, const Temperature& actualTemperature, const Temperature& tempFromLab, const PreciseDouble& timeStepsPerDay)
{
	IndividualGrowthModel::Type actualGrowthModel;
	PreciseDouble actualA;

	tie(actualGrowthModel, actualA) = getSpeciesGrowth()->getActualGrowthModel(actualTemperature, tempFromLab);

	bool updateIndividualGrowthModel = false;

	if(individualGrowthModel != nullptr)
	{
		if(individualGrowthModel->getType() != actualGrowthModel)
		{
			delete individualGrowthModel;

			updateIndividualGrowthModel = true;
		}
	}
	else
	{
		updateIndividualGrowthModel = true;
	}

	if(updateIndividualGrowthModel)
	{
		individualGrowthModel = IndividualGrowthModel::createInstance(actualGrowthModel, actualA);


		PreciseDouble finalDevTimeProportion;

		if(isTemperatureDependent())
		{
			finalDevTimeProportion = Day(baseIndividualTraits[static_cast<size_t>(BaseTraitType::devTime)].getPhenotypicValue()).getValue() / (getSpeciesGrowth()->getInstarAgeVector().back().getValue() * baseDevTimeProportion);
		}
		else
		{
			finalDevTimeProportion = 1.0;
		}


		TimeStep tMax(getSpeciesGrowth()->getInstarDevTime(Instar(static_cast<unsigned int>(getSpeciesGrowth()->getInstarAgeVector().size()))) * baseDevTimeProportion * finalDevTimeProportion, timeStepsPerDay);
		

		individualGrowthModel->adjust(
			baseIndividualTraits[static_cast<size_t>(BaseTraitType::growth)].getPhenotypicValue(), Length(baseIndividualTraits[static_cast<size_t>(BaseTraitType::lengthAtMaturation)].getPhenotypicValue()), 
			Day(tMax, timeStepsPerDay), getLengthAtBirth()
		);
	}
}

void AnimalGrowth::calculateGrowthCurves(const std::vector<IndividualTrait>& baseIndividualTraits, const unsigned int femaleMaxReproductionEvents, const PreciseDouble& timeStepsPerDay)
{
    PreciseDouble finalDevTimeProportion;

    if(isTemperatureDependent())
    {
        finalDevTimeProportion = Day(baseIndividualTraits[static_cast<size_t>(BaseTraitType::devTime)].getPhenotypicValue()).getValue() / (getSpeciesGrowth()->getInstarAgeVector().back().getValue() * baseDevTimeProportion);
    }
    else
    {
        finalDevTimeProportion = 1.0;
    }


	for(unsigned int i = 1; i <= getSpeciesGrowth()->getInstarAgeVector().size(); i++)
	{
		Instar instarValue(i);

		individualInstarAgeVector[instarValue] = TimeStep(getSpeciesGrowth()->getInstarDevTime(instarValue) * baseDevTimeProportion * finalDevTimeProportion, timeStepsPerDay);
	}


	if(isTemperatureDependent())
    {
        individualGrowthModel->adjust(
			baseIndividualTraits[static_cast<size_t>(BaseTraitType::growth)].getPhenotypicValue(), Length(baseIndividualTraits[static_cast<size_t>(BaseTraitType::lengthAtMaturation)].getPhenotypicValue()), 
			Day(individualInstarAgeVector.back(), timeStepsPerDay), getLengthAtBirth()
		);
	}


	for(unsigned int i = 1; i <= getSpeciesGrowth()->getInstarAgeVector().size(); i++)
	{
		Instar instarValue(i);

		instarLengthsVector[instarValue] = getIndividualGrowthModel()->calculateLength(Day(individualInstarAgeVector[instarValue], timeStepsPerDay), baseIndividualTraits[static_cast<size_t>(BaseTraitType::growth)].getPhenotypicValue());
        
        instarMassesVector[instarValue] = getSpeciesGrowth()->calculateDryMass(
			instarLengthsVector[instarValue], 
			instarValue >= getSpeciesGrowth()->getInstarFirstMaturation()
		);
	}


	if(getSpeciesGrowth()->hasIndeterminateGrowth())
	{
		if(getInstarAge(getSpeciesGrowth()->getInstarFirstReproduction()-1) * baseIndividualTraits[static_cast<size_t>(BaseTraitType::longevitySinceMaturation)].getPhenotypicValue() < getIndividualInstarAgeVector().back())
		{
			throwLineInfoException("Item 'longevitySinceMaturation' must be rounded up to a higher value.");
		}

		if((getInstarAge(getSpeciesGrowth()->getInstarFirstReproduction()-1) * baseIndividualTraits[static_cast<size_t>(BaseTraitType::longevitySinceMaturation)].getPhenotypicValue() - getInstarAge(getSpeciesGrowth()->getInstarFirstReproduction()-1)) / femaleMaxReproductionEvents < TimeStep(1))
		{
			throwLineInfoException("Item 'longevitySinceMaturation' must be rounded up to a higher value due to female reproductive events.");
		}
	}
}

void AnimalGrowth::setMatureValue(const bool isMature, const Gender&)
{
	mature = isMature;
}

bool AnimalGrowth::isMature() const 
{ 
	return mature; 
}

void AnimalGrowth::setDryMass(const DryMass &newDryMass, const DryMass& investment, const PreciseDouble& energyTankFinalTraitValue, const bool isMoulting, Gender gender, const bool checkPreviousValues, const TimeStep actualTimeStep)
{
	DryMass dryMassToUse, investmentToUse;

	if(isMoulting)
	{
		dryMassToUse = DryMass(newDryMass.getValue()*getSpeciesGrowth()->getAssignedForMolt());
		investmentToUse = DryMass(investment.getValue()*getSpeciesGrowth()->getAssignedForMolt());
	}
	else
	{
		dryMassToUse = newDryMass;
		investmentToUse = investment;
	}

	DryMass excessInvestment = investmentToUse-dryMassToUse;


	DryMass massToCapitalBreeding(0.0);

	if(gender == Gender::FEMALE && isMature() && getSpeciesGrowth()->hasCapitalBreeding()){
		massToCapitalBreeding = DryMass(static_cast<double>(getSpeciesGrowth()->getNumberOfCapitalBreeds()) * getSpeciesGrowth()->getEggsPerBatch().getValue() * getEggDryMassAtBirth().getValue());
	}


	DryMass newEnergyTank; 
	DryMass newCurrentBodySize;

	tie(newEnergyTank, newCurrentBodySize) = getSpeciesGrowth()->decomposeMassElements(dryMassToUse - massToCapitalBreeding, energyTankFinalTraitValue);

	newEnergyTank = newEnergyTank + massToCapitalBreeding;


	if(excessInvestment > 0.0)
    {
		newEnergyTank = newEnergyTank + DryMass(excessInvestment.getValue()*(1-getSpeciesGrowth()->getExcessInvestInSize()));

		newCurrentBodySize = newCurrentBodySize + DryMass(excessInvestment.getValue()*getSpeciesGrowth()->getExcessInvestInSize());
    }


	if(checkPreviousValues)
	{
		if(newCurrentBodySize < getCurrentBodySize())
		{
			if(dryMassToUse <= getCurrentBodySize())
			{
				newCurrentBodySize = getCurrentBodySize();
				newEnergyTank = DryMass(0.0);
			}
			else
			{
				newCurrentBodySize = getCurrentBodySize();
				newEnergyTank = DryMass(pow(dryMassToUse.getValue(), getSpeciesGrowth()->getBetaScaleTank())) - newCurrentBodySize;
			}
		}
	}
	

	setMassElements(
		newCurrentBodySize,
		newEnergyTank,
		actualTimeStep
	);
}

#ifdef DEBUG
	void AnimalGrowth::testGetCurrentBodySize() const
	{
		if(!currentBodySizeInitialised)
		{
			throwLineInfoException("Trying to read an uninitialised variable.");
		}
	}

	void AnimalGrowth::testSetCurrentBodySize(const TimeStep actualTimeStep)
	{
		if(currentBodySizeInitialised)
		{
			if(currentBodySizeLastTimeStep == actualTimeStep)
			{
				// throwLineInfoException("Variable modified twice in the same time step.");
			}
		}
		else
		{
			currentBodySizeInitialised = true;
		}

		currentBodySizeLastTimeStep = actualTimeStep;
	}

	void AnimalGrowth::testModifyEnergyTank(const TimeStep actualTimeStep)
	{
		if(currentEnergyTankInitialised)
		{
			if(currentEnergyTankLastTimeStep == actualTimeStep)
			{
				// throwLineInfoException("Variable modified twice in the same time step.");
			}
		}
		else
		{
			currentEnergyTankInitialised = true;
		}

		currentEnergyTankLastTimeStep = actualTimeStep;
	}

	void AnimalGrowth::testGetCurrentEnergyTank() const
	{
		if(!currentEnergyTankInitialised)
		{
			throwLineInfoException("Trying to read an uninitialised variable.");
		}
	}

	void AnimalGrowth::deserializeDebugVariables()
	{
		currentBodySizeInitialised = true;
		currentBodySizeLastTimeStep = TimeStep(UINT_MAX);

		currentEnergyTankInitialised = true;
		currentEnergyTankLastTimeStep = TimeStep(UINT_MAX);
	}
#endif

const DryMass& AnimalGrowth::getCurrentEnergyTank() const
{
	#ifdef DEBUG
		testGetCurrentEnergyTank();
	#endif

	return currentEnergyTank;
}

void AnimalGrowth::modifyEnergyTank(const DryMass &newValue, const TimeStep actualTimeStep)
{
	#ifdef DEBUG
		testModifyEnergyTank(actualTimeStep);
	#endif

	currentEnergyTank = (newValue < 0.0) ? DryMass(0.0) : newValue;

	updateTotalMass();
}

void AnimalGrowth::setMassElements(const DryMass &newCurrentBodySize, const DryMass& newEnergyTank, const TimeStep actualTimeStep)
{
	setCurrentBodySize(newCurrentBodySize, actualTimeStep);
	modifyEnergyTank(newEnergyTank, actualTimeStep);
}

const DryMass& AnimalGrowth::getCurrentBodySize() const 
{ 
	#ifdef DEBUG
		testGetCurrentBodySize();
	#endif

	return currentBodySize; 
}

void AnimalGrowth::setCurrentBodySize(const DryMass& newCurrentBodySize, const TimeStep actualTimeStep)
{
	#ifdef DEBUG
		testSetCurrentBodySize(actualTimeStep);
	#endif

	currentBodySize = newCurrentBodySize;
}

void AnimalGrowth::updateTotalMass()
{
	setCurrentTotalDryMass(getCurrentBodySize() + getCurrentEnergyTank());
}

const AnimalSpeciesGrowth* AnimalGrowth::getSpeciesGrowth() const
{
	return static_cast<const AnimalSpeciesGrowth*>(EdibleGrowth::getSpeciesGrowth());
}

AnimalSpeciesGrowth* AnimalGrowth::getMutableSpeciesGrowth()
{
	return static_cast<AnimalSpeciesGrowth*>(EdibleGrowth::getMutableSpeciesGrowth());
}

void AnimalGrowth::setCurrentTotalWetMass(const WetMass&)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalGrowth'.");
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(AnimalGrowth)

template <class Archive>
void AnimalGrowth::serialize(Archive &ar, const unsigned int) 
{
	ar & boost::serialization::base_object<EdibleGrowth>(*this);

	ar & individualInstarAgeVector;
    ar & instarLengthsVector;
    ar & instarMassesVector;

	ar & individualGrowthModel;

    ar & eggDryMassAtBirth;
    ar & lengthAtBirth;

    ar & baseDevTimeProportion;

    ar & currentBodySize;

	ar & currentEnergyTank;

    ar & temperatureDependent;

	ar & currentAge;

	ar & mature;
}

// // Specialisation
template void AnimalGrowth::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalGrowth::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalGrowth::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalGrowth::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
