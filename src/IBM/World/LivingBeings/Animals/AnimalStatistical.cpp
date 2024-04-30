#include "IBM/World/LivingBeings/Animals/AnimalStatistical.h"

#include "IBM/World/World.h"


using namespace std;


AnimalStatistical::AnimalStatistical(const Instar &instar, AnimalSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface)
	: Animal(mySpecies, terrainCellInterface, instar, LifeStage::ACTIVE)
{
	animalGrowth = new AnimalStatisticalGrowth(this, getFinalTraitValue(Trait::Type::factorEggMass));

	getMutableAnimalGrowth()->forceMolting();

	getMutableSpecies()->getMutableWorld()->updateMaxSearchArea(getFinalTraitValue(Trait::Type::search_area));
}

AnimalStatistical::~AnimalStatistical()
{

}

const AnimalStatisticalGrowth* const AnimalStatistical::getAnimalGrowth() const
{
	return static_cast<const AnimalStatisticalGrowth* const>(animalGrowth);
}

AnimalStatisticalGrowth* const AnimalStatistical::getMutableAnimalGrowth()
{
	return static_cast<AnimalStatisticalGrowth* const>(animalGrowth);
}

int AnimalStatistical::getPredatedByID() const
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::incrementEncountersWithPredator(const int &predatorId)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

const bool AnimalStatistical::isHunting() const
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

bool AnimalStatistical::isDepleted(double foodDemand, const double &dryMass) const
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

double AnimalStatistical::anyLeft(const double &dryMass) const
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

const double AnimalStatistical::getInterpolatedDryMass(const unsigned int evaluationDepth, const double &dryMass) const
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

const double AnimalStatistical::turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::setNewLifeStage(const LifeStage newLifeStage)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

void AnimalStatistical::setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps, int predatorId)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

double AnimalStatistical::substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

bool AnimalStatistical::canEatEdible(const EdibleInterface* const &edibleToCheck, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, const double &dryMass) const
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}

bool AnimalStatistical::predateEdible(EdibleInterface &edibleToBePredated, const double &targetDryMass, const Ring* const perceptionArea, const unsigned int numberOfTimeSteps, bool retaliation, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea)
{
	throwLineInfoException("Error: Do not use this method with the class 'AnimalStatistical'.");
}
