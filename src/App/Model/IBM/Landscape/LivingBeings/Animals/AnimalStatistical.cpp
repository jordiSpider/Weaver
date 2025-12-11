#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalStatistical.h"


#include "App/Model/IBM/Landscape/Landscape.h"


using namespace std;


AnimalStatistical::AnimalStatistical(Landscape* const landscape, const Instar &instar, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const PreciseDouble& timeStepsPerDay)
	: Animal(landscape->generateEdibleId(), mySpecies, terrainCell, LifeStage::ACTIVE, TimeStep(0)),
	  growthBuildingBlock(
		&getMutableSpecies()->getMutableGrowthBuildingBlock(), instar, 
		getSpecies()->getGenetics().isGrowthTraitsThermallyDependent(), 
		getGender(), getGenetics().getBaseIndividualTraits(), getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue(),
		getSpecies()->getFemaleMaxReproductionEvents(), terrainCell->getPatchApplicator().getCellMoisture().getTemperature(), 
		mySpecies->getTempFromLab(), timeStepsPerDay
	  )
{
	getMutableGrowthBuildingBlock().forceMolting(getGenetics().getBaseIndividualTraits(), getGender(), timeStepsPerDay);

	getMutableGenetics().deleteHomologousCorrelosomes();
}

AnimalStatistical::~AnimalStatistical()
{

}

const AnimalStatisticalGrowth& AnimalStatistical::getGrowthBuildingBlock() const
{
	return growthBuildingBlock;
}

AnimalStatisticalGrowth& AnimalStatistical::getMutableGrowthBuildingBlock()
{
	return growthBuildingBlock;
}

void AnimalStatistical::increasePredationEncounters()
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

bool AnimalStatistical::isHunting() const
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::addAnimalSpecies(const unsigned int, const PreciseDouble&)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::addResourceSpecies(const unsigned int, const PreciseDouble&)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::setNewLifeStage(Landscape* const, const LifeStage)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::resetControlVariables()
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

PreciseDouble AnimalStatistical::getRemainingVoracity() const
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::setNewLifeStage(Landscape* const, const LifeStage, const TimeStep, const PreciseDouble&)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::setNewLifeStage(Landscape* const, const LifeStage, const TimeStep, EdibleID, const PreciseDouble&)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

const Instar& AnimalStatistical::getInstarToEvaluateCells() const
{
	return getGrowthBuildingBlock().getInstar();
}

pair<bool, DryMass> AnimalStatistical::predateEdible(Landscape* const, Edible &, const DryMass &, const PointContinuous &, const PreciseDouble &, const TimeStep, const PreciseDouble&, bool, std::list<const Edible*> &, std::ostringstream&, std::ostringstream&, const PreciseDouble&, const PreciseDouble&, const PreciseDouble&, const PreciseDouble&, const PreciseDouble&, const PreciseDouble&, const bool)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

DryMass AnimalStatistical::turnIntoDryMass(const DryMass&, const PreciseDouble&) const
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth)
{
	growthBuildingBlock.setSpeciesGrowth(newSpeciesGrowth);
}
