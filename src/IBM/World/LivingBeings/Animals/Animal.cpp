#include "IBM/World/LivingBeings/Animals/Animal.h"

#include "IBM/World/World.h"


using namespace std;


Animal::Animal(AnimalSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar, 
		const LifeStage& lifeStage)
	: AnimalInterface(mySpecies, terrainCellInterface, instar), lifeStage(lifeStage), gender(getSpecies()->getRandomGender()),
	  genome(getSpecies()->getLoci(), getSpecies()->getRandomlyCreatedPositionsForChromosomes(), getSpecies()->getNumberOfChromosomes(), 
	  getSpecies()->getNumberOfLociPerChromosome(), getSpecies()->getNumberOfChiasmasPerChromosome()), 
	  baseTraitsElements(EnumClass<Trait::Type>::size()), finalTraitsValue(EnumClass<Trait::Type>::size(), 0.0), 
	  animalGrowth(nullptr), currentAge(0)
{
	setOtherAttributes();
}

Animal::Animal(AnimalSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar, 
		const LifeStage& lifeStage, Gamete* const firstParentGamete, Gamete* const secondParentGamete, 
		const AnimalSpecies::Gender::GenderValue& gender)
	: AnimalInterface(mySpecies, terrainCellInterface, instar), lifeStage(lifeStage), gender(gender),
	  genome(firstParentGamete, secondParentGamete, getSpecies()->getRandomlyCreatedPositionsForChromosomes(), 
	  getSpecies()->getNumberOfLociPerChromosome(), getSpecies()->getNumberOfChiasmasPerChromosome()), 
	  baseTraitsElements(EnumClass<Trait::Type>::size()), finalTraitsValue(EnumClass<Trait::Type>::size(), 0.0), 
	  animalGrowth(nullptr), currentAge(0)
{
	setOtherAttributes();
}

void Animal::setOtherAttributes()
{
	#ifdef DEBUG
		baseTraitsElementsInitialised.resize(EnumClass<Trait::Type>::size(), false);

		finalTraitsValueInitialised.resize(EnumClass<Trait::Type>::size(), false);
		finalTraitsValueLastTimeStep.resize(EnumClass<Trait::Type>::size(), UINT_MAX);

		currentEnergyTankInitialised = false;
		currentEnergyTankLastTimeStep = 0;
	#endif

	setMatureValue(false);

	initTraits();
}

Animal::~Animal()
{
	delete animalGrowth;
}

const AnimalSpecies* const Animal::getSpecies() const 
{ 
	return static_cast<const AnimalSpecies* const>(Edible::getSpecies()); 
}

AnimalSpecies* const Animal::getMutableSpecies() 
{ 
	return static_cast<AnimalSpecies* const>(Edible::getMutableSpecies()); 
}

const unsigned int& Animal::getCellDepthOnActualInstar() const
{
	return getSpecies()->getCellDepthPerInstar()[getInstar()];
}

const PointContinuous& Animal::getPosition() const
{
	return position;
}

PointContinuous& Animal::getMutablePosition()
{
	return position;
}

void Animal::setPosition(const PointContinuous &newPosition)
{
	position = newPosition;
}

void Animal::setPosition(const vector<double> &newPosition)
{
	for(unsigned int i = 0; i < DIMENSIONS; i++)
	{
		setPositionAxisValue(position, i, newPosition[i]);
	}
}

const LifeStage& Animal::getLifeStage() const 
{ 
	return lifeStage; 
}

void Animal::setCurrentAge(const double &newCurrentAge)
{
	currentAge = newCurrentAge;
}

const double& Animal::getCurrentAge() const
{
	return currentAge;
}

AnimalSpecies::Gender::GenderValue Animal::getGender() const 
{ 
	return gender; 
}

const double Animal::getSpeed() const 
{ 
	return getFinalTraitValue(Trait::Type::speed); 
}

const double Animal::getVoracity() const 
{ 
	return getFinalTraitValue(Trait::Type::voracity); 
}

const AnimalGrowth* const Animal::getAnimalGrowth() const
{
	return animalGrowth;
}

AnimalGrowth* const Animal::getMutableAnimalGrowth()
{
	return animalGrowth;
}

const CustomIndexedVector<Trait::Type, CustomIndexedVector<TraitDefinitionSection::Elements, double>>& Animal::getBaseTraitElementVector() const
{
	return baseTraitsElements;
}

const double& Animal::getBaseTraitElement(const Trait::Type name, const TraitDefinitionSection::Elements elementType) const
{
	#ifdef DEBUG
		testGetBaseTraitElement(name, elementType);
	#endif

	return baseTraitsElements[name][elementType];
}

const double Animal::calculateDryMass() const 
{ 
	return getSpecies()->calculateDryMass(getAnimalGrowth()->getCurrentBodySize(), isMature());
}

const double Animal::calculateWetMass() const
{ 
	return calculateWetMass(calculateDryMass());
}

const double Animal::calculateWetMass(const double &dryMass) const
{ 
	return getSpecies()->convertToWetMass(dryMass);
}

#ifdef DEBUG
	void Animal::testSetBaseTraitElement(const Trait::Type name, const TraitDefinitionSection::Elements elementType)
	{
		if(baseTraitsElementsInitialised[name][elementType])
		{
			if(name != Trait::Type::eggDevTime)
			{
				throwLineInfoException("Variable already initialised.");
			}
		}
		else
		{
			baseTraitsElementsInitialised[name][elementType] = true;
		}
	}

	void Animal::testGetBaseTraitElement(const Trait::Type name, const TraitDefinitionSection::Elements elementType) const
	{
		if(!baseTraitsElementsInitialised[name][elementType])
		{
			throwLineInfoException("Trying to read an uninitialised variable.");
		}
	}

	void Animal::testSetFinalTraitValue(const Trait::Type name)
	{
		if(finalTraitsValueInitialised[name])
		{
			if(finalTraitsValueLastTimeStep[name] == getSpecies()->getWorld()->getActualTimeStep())
			{
				// throwLineInfoException("Variable modified twice in the same time step.");
			}

			finalTraitsValueLastTimeStep[name] = getSpecies()->getWorld()->getActualTimeStep();
		}
		else
		{
			finalTraitsValueInitialised[name] = true;
		}
	}

	void Animal::testGetFinalTraitValue(const Trait::Type name) const
	{
		if(!finalTraitsValueInitialised[name])
		{
			throwLineInfoException("Trying to read an uninitialised variable.");
		}
	}

	void Animal::testModifyEnergyTank()
	{
		if(currentEnergyTankInitialised)
		{
			if(currentEnergyTankLastTimeStep == getSpecies()->getWorld()->getActualTimeStep())
			{
				// throwLineInfoException("Variable modified twice in the same time step.");
			}
		}
		else
		{
			currentEnergyTankInitialised = true;
		}

		currentEnergyTankLastTimeStep = getSpecies()->getWorld()->getActualTimeStep();
	}

	void Animal::testGetCurrentEnergyTank() const
	{
		if(!currentEnergyTankInitialised)
		{
			throwLineInfoException("Trying to read an uninitialised variable.");
		}
	}
#endif

const double& Animal::getBaseTraitValue(const Trait::Type name) const
{
	return getBaseTraitElement(name, TraitDefinitionSection::Elements::TraitValue);
}

const double& Animal::getFinalTraitValue(const Trait::Type& name) const
{
	#ifdef DEBUG
		testGetFinalTraitValue(name);
	#endif

	return finalTraitsValue[name];
}

void Animal::setBaseTraitElement(const Trait::Type name, const TraitDefinitionSection::Elements elementType, const double& newValue)
{
	#ifdef DEBUG
		testSetBaseTraitElement(name, elementType);
	#endif

	if(elementType == TraitDefinitionSection::Elements::TraitValue && name != Trait::Type::factorEggMass && name != Trait::Type::eggDevTime)
	{
		if(newValue < 0.0)
		{
			throwLineInfoException("The trait '" + EnumClass<Trait::Type>::to_string(name) + "' will have a negative value.");
		}
	}

	baseTraitsElements[name][elementType] = newValue;
}

void Animal::setFinalTraitValue(const Trait::Type& name, const double& newValue)
{
	#ifdef DEBUG
		testSetFinalTraitValue(name);
	#endif

	if(name != Trait::Type::factorEggMass && name != Trait::Type::eggDevTime)
	{
		if(newValue < 0.0)
		{
			throwLineInfoException("The trait '" + EnumClass<Trait::Type>::to_string(name) + "' will have a negative value.");
		}
	}

	finalTraitsValue[name] = newValue;
}

void Animal::setMatureValue(const bool isMature)
{
	mature = isMature;
}

const bool Animal::isMature() const 
{ 
	return mature; 
}

const double& Animal::getCurrentEnergyTank() const
{
	#ifdef DEBUG
		testGetCurrentEnergyTank();
	#endif

	return currentEnergyTank;
}

void Animal::modifyEnergyTank(const double &newValue)
{
	#ifdef DEBUG
		testModifyEnergyTank();
	#endif

	currentEnergyTank = (newValue < 0.0) ? 0.0 : newValue;
}

void Animal::initTraits()
{
	for(const Trait::Type& traitType : EnumClass<Trait::Type>::getEnumValues())
	{
		#ifdef DEBUG
			baseTraitsElementsInitialised[traitType].resize(getSpecies()->getTrait(traitType).getElements().size(), false);
	  	#endif

		baseTraitsElements[traitType].resize(getSpecies()->getTrait(traitType).getElements().size(), 0.0);

		for(unsigned int element = static_cast<unsigned int>(TraitDefinitionSection::Elements::TraitValue); element < baseTraitsElements[traitType].size(); element++)
		{
			setBaseTraitElement(
				traitType, static_cast<TraitDefinitionSection::Elements>(element),
				getMutableSpecies()->getMutableTrait(traitType).getMutableElements().at(static_cast<TraitDefinitionSection::Elements>(element))->getValue(getGenome(), getSpecies()->getTraitsPerModule(), 
					getSpecies()->getNumberOfLociPerTrait(), getSpecies()->getRhoPerModuleVector(), getSpecies()->getRhoRangePerModuleVector()
				)
			);
		}

		if(!getSpecies()->getTrait(traitType).isThermallyDependent())
		{
			setFinalTraitValue(traitType, getBaseTraitValue(traitType));
		}
	}

	if(getSpecies()->getTrait(Trait::Type::lengthAtMaturation).isThermallyDependent())
	{
		auto result = getSpecies()->getTrait(Trait::Type::lengthAtMaturation).getTemperatureSection().calculateTempSizeRuleConstantAccumulationVector(getBaseTraitValue(Trait::Type::lengthAtMaturation), getSpecies());

		setLowerTempSizeRuleConstantAccumulationVector(get<0>(*result));
		setLastLowerTempSizeRuleConstant(get<1>(*result));
		setUpperTempSizeRuleConstantAccumulationVector(get<2>(*result));
		setLastUpperTempSizeRuleConstant(get<3>(*result));
	}

	deleteHomologousCorrelosomes();

	tuneTraits(getTerrainCellInterface()->getMoistureInfo()->getTemperature());
}

const map<Temperature, pair<double, double>>& Animal::getLowerTempSizeRuleConstantAccumulationVector() const
{
	return lowerTempSizeRuleConstantAccumulationVector;
}

const double& Animal::getLastLowerTempSizeRuleConstant() const
{
	return lastLowerTempSizeRuleConstant;
}

const map<Temperature, pair<double, double>>& Animal::getUpperTempSizeRuleConstantAccumulationVector() const
{
	return upperTempSizeRuleConstantAccumulationVector;
}

const double& Animal::getLastUpperTempSizeRuleConstant() const
{
	return lastUpperTempSizeRuleConstant;
}

void Animal::setLowerTempSizeRuleConstantAccumulationVector(const map<Temperature, pair<double, double>>& newLowerTempSizeRuleConstantAccumulationVector)
{
	lowerTempSizeRuleConstantAccumulationVector = newLowerTempSizeRuleConstantAccumulationVector;
}

void Animal::setLastLowerTempSizeRuleConstant(const double& newLastLowerTempSizeRuleConstant)
{
	lastLowerTempSizeRuleConstant = newLastLowerTempSizeRuleConstant;
}

void Animal::setUpperTempSizeRuleConstantAccumulationVector(const map<Temperature, pair<double, double>>& newUpperTempSizeRuleConstantAccumulationVector)
{
	upperTempSizeRuleConstantAccumulationVector = newUpperTempSizeRuleConstantAccumulationVector;
}

void Animal::setLastUpperTempSizeRuleConstant(const double& newLastUpperTempSizeRuleConstant)
{
	lastUpperTempSizeRuleConstant = newLastUpperTempSizeRuleConstant;
}

void Animal::tuneTraits(const Temperature& temperature)
{
	for(const Trait::Type& traitType : getSpecies()->getTemperatureDependentTraits())
	{
		double finalTraitValue = getSpecies()->getTrait(traitType).applyTemperatureDependency(
			temperature, baseTraitsElements[traitType], getLowerTempSizeRuleConstantAccumulationVector(),
			getLastLowerTempSizeRuleConstant(), getUpperTempSizeRuleConstantAccumulationVector(), getLastUpperTempSizeRuleConstant(), getSpecies()
		);

		setFinalTraitValue(traitType, finalTraitValue);
	}
}

const Genome& Animal::getGenome() const 
{ 
	return genome; 
}

void Animal::deleteHomologousCorrelosomes() 
{ 
	genome.deleteHomologousCorrelosomes(); 
}
