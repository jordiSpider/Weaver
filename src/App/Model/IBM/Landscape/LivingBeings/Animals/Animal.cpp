#include "App/Model/IBM/Landscape/LivingBeings/Animals/Animal.h"

#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"


using namespace std;


Animal::Animal()
	: Edible()
{

}

Animal::Animal(const EdibleID id, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const Genome* const genome, 
		const LifeStage& lifeStage, const TimeStep actualTimeStep)
	: Edible(id, mySpecies, terrainCell), 
	  genetics(
		&getMutableSpecies()->getMutableGenetics(), genome, getTerrainCell()->getPatchApplicator().getCellMoisture().getTemperature(), actualTimeStep,
		getSpecies()->getGrowthBuildingBlock().getCoefficientForMassAforMature(), 
		getSpecies()->getGrowthBuildingBlock().getScaleForMassBforMature(),
		getSpecies()->getTempFromLab()
	  ), 
	  lifeStage(lifeStage), gender(getSpecies()->getRandomGender())
{
	
}

Animal::Animal(const EdibleID id, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, 
		const LifeStage& lifeStage, Gamete* const firstParentGamete, Gamete* const secondParentGamete, 
		const Gender& gender, const TimeStep actualTimeStep)
	: Edible(id, mySpecies, terrainCell), 
	  genetics(
		&getMutableSpecies()->getMutableGenetics(), firstParentGamete, secondParentGamete,
		getTerrainCell()->getPatchApplicator().getCellMoisture().getTemperature(), actualTimeStep,
		getSpecies()->getGrowthBuildingBlock().getCoefficientForMassAforMature(), 
		getSpecies()->getGrowthBuildingBlock().getScaleForMassBforMature(),
		getSpecies()->getTempFromLab()
	  ), 
	  lifeStage(lifeStage), gender(gender)
{
	
}

Animal::~Animal()
{
	
}

const AnimalSpecies* Animal::getSpecies() const 
{ 
	return static_cast<const AnimalSpecies*>(Edible::getSpecies()); 
}

AnimalSpecies* Animal::getMutableSpecies() 
{ 
	return static_cast<AnimalSpecies*>(Edible::getMutableSpecies()); 
}

unsigned int Animal::getCellDepthOnActualInstar() const
{
	return getSpecies()->getCellDepthPerInstar()[getGrowthBuildingBlock().getInstar()];
}

const PointContinuous& Animal::getPosition() const
{
	return position;
}

PointContinuous& Animal::getMutablePosition()
{
	return position;
}

const Genetics& Animal::getGenetics() const
{
	return genetics;
}

Genetics& Animal::getMutableGenetics()
{
	return genetics;
}

void Animal::setPosition(const PointContinuous &newPosition)
{
	position = newPosition;
}

void Animal::setPosition(const vector<PreciseDouble> &newPosition)
{
	for(unsigned char i = 0; i < DIMENSIONS; i++)
	{
		setPositionAxisValue(position, i, newPosition[i]);
	}
}

const LifeStage& Animal::getLifeStage() const 
{ 
	return lifeStage; 
}

PreciseDouble Animal::applyAllometricModel(const PreciseDouble& coefficient, const PreciseDouble& scale) const
{
	return coefficient * pow(getGrowthBuildingBlock().getCurrentTotalDryMass().getValue(), scale);
}

Gender Animal::getGender() const 
{ 
	return gender; 
}

#ifdef DEBUG
	void Animal::deserializeDebugVariables()
	{
		getMutableGenetics().deserializeDebugVariables();

		getMutableGrowthBuildingBlock().deserializeDebugVariables();
	}
#endif


void Animal::setSpecies(Species* newSpecies)
{
	Edible::setSpecies(newSpecies);

	genetics.setAnimalSpeciesGenetics(&getMutableSpecies()->getMutableGenetics());
}


void Animal::checkGrowthMinimumValue() const
{
	switch(getGrowthBuildingBlock().getIndividualGrowthModel()->getType()) {
		case IndividualGrowthModel::Type::Linear: {
			PreciseDouble growthMinimumValue = 0.0;

			if(getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue() <= growthMinimumValue)
			{
				throwLineInfoException("The value of 'k' is out of range for the Linear model.");
			}
			break;
		}
		case IndividualGrowthModel::Type::VonBertalanffy: {
			PreciseDouble growthMinimumValue = 0.0;

			if(getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue() <= growthMinimumValue)
			{
				throwLineInfoException("The value of 'k' is out of range for the VonBertalanffy model.");
			}
			break;
		}
		case IndividualGrowthModel::Type::Logistic: {
			PreciseDouble growthMinimumValue = log(getGenetics().getBaseIndividualTraits(BaseTraitType::lengthAtMaturation).getPhenotypicValue() / getGrowthBuildingBlock().getLengthAtBirth().getValue()) / getGenetics().getBaseIndividualTraits(BaseTraitType::devTime).getPhenotypicValue();

			if(getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue() <= growthMinimumValue)
			{
				throwLineInfoException("The value of 'k' is out of range for the Logistic model.");
			}
			break;
		}
		case IndividualGrowthModel::Type::Logistic4P: {
			PreciseDouble growthMinimumValue = 0.0;

			if(getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue() <= growthMinimumValue)
			{
				throwLineInfoException("The value of 'k' is out of range for the Logistic4P model.");
			}
			break;
		}
		case IndividualGrowthModel::Type::Exponential: {
			PreciseDouble growthMinimumValue = 0.0;

			if(getGenetics().getBaseIndividualTraits(BaseTraitType::growth).getPhenotypicValue() <= growthMinimumValue)
			{
				throwLineInfoException("The value of 'k' is out of range for the Exponential model.");
			}
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}







BOOST_SERIALIZATION_ASSUME_ABSTRACT(Animal)

template <class Archive>
void Animal::serialize(Archive &ar, const unsigned int) 
{
	ar & boost::serialization::base_object<Edible>(*this);

	ar & genetics;

	ar & lifeStage;

    ar & gender;

    ar & position;
}

// // Specialisation
template void Animal::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Animal::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Animal::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Animal::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
