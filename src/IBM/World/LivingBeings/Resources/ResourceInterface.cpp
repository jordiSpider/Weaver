#include "IBM/World/LivingBeings/Resources/ResourceInterface.h"
#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"
#include "IBM/World/Map/SpatialTree/SpatialTree.h"
#include "IBM/World/WorldInterface.h"

using namespace std;



ResourceInterface::ResourceInterface(Species* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar, const double &biomass)
    : Edible(mySpecies, terrainCellInterface, instar, false), biomass(biomass), fullCapacity(false)
{
	
}

ResourceInterface::~ResourceInterface()
{

}

ResourceSpecies* const ResourceInterface::getSpecies() const 
{ 
	return static_cast<ResourceSpecies* const>(Edible::getSpecies()); 
}

const double ResourceInterface::getSpeed() const 
{ 
	return 0.0; 
}

const double ResourceInterface::getVoracity() const 
{ 
	return 0.0; 
}

const double ResourceInterface::getCurrentBodySize() const 
{ 
	return 0.0; 
}

int ResourceInterface::getPredatedByID() const 
{ 
	return -1; 
}

void ResourceInterface::incrementEncountersWithPredator(const int &predatorId) 
{

}

const double& ResourceInterface::getBiomass() const
{
    return biomass;
}

void ResourceInterface::setBiomass(const double &newBiomass)
{
    biomass = newBiomass;
}

double ResourceInterface::substractBiomass(double dryMassToBeSubstracted) 
{ 
	return substractBiomass(dryMassToBeSubstracted, 0.0);
}

double ResourceInterface::substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea)
{
	auto intersection = Geometry::calculateIntersection(*perceptionArea, getTerrainCellInterface()->getEffectiveArea());

	double percent = Geometry::calculateCoveragePercent(intersection.get(), getTerrainCellInterface()->getEffectiveArea());

	if(Geometry::checkCoverage(percent) == Coverage::Full)
	{
		return substractBiomass(dryMassToBeSubstracted);
	}
	else
	{
		return substractBiomass(dryMassToBeSubstracted, perceptionArea, calculateDryMass() * (1.0 - percent));
	}
}

double ResourceInterface::substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea, const double &minimumDryMass) 
{ 
	return substractBiomass(dryMassToBeSubstracted, minimumDryMass);
}

void ResourceInterface::updateFullCapacityStatus()
{
	setFullCapacity(false);

	if(getTerrainCellInterface()->getMapInterface().getWorldInterface()->getCompetitionAmongResourceSpecies())
	{
		for(auto &resource : getMutableTerrainCellInterface()->getMutableResources())
		{
			if(resource != nullptr && resource != this)
			{
				if(resource->getBiomass() < resource->getResourceMaximumCapacity())
				{
					resource->setFullCapacity(false);
				}
			}
		}
	}

	auto neighbours = getTerrainCellInterface()->getMutableMapInterface().getResourceNeighbours(getMutableTerrainCellInterface(), getSpecies()->getResourceSpeciesId(), 1);

	for(auto &neighbour : *neighbours)
	{
		neighbour->getMutableResource(getSpecies()->getResourceSpeciesId())->setFullCapacity(false);
	}
}

double ResourceInterface::substractBiomass(double dryMassToBeSubstracted, const double &minimumDryMass) 
{ 
	updateFullCapacityStatus();

	double totalDryMass = calculateDryMass() - minimumDryMass;

	double remainingDryMass = dryMassToBeSubstracted - totalDryMass;

	if(remainingDryMass <= numeric_limits<double>::epsilon())
	{
		biomass -= getSpecies()->getConversionToWetMass() * dryMassToBeSubstracted;
		return 0.0;
	}
	else
	{
		biomass -= getSpecies()->getConversionToWetMass() * totalDryMass;

		if(biomass <= numeric_limits<double>::epsilon())
		{
			biomass = 0.0;
		}

		return remainingDryMass;
	}
}

const bool ResourceInterface::isFullCapacity() const
{
	return fullCapacity;
}

void ResourceInterface::setFullCapacity(const bool newFullCapacityValue)
{
	fullCapacity = newFullCapacityValue;
}

const bool ResourceInterface::isHunting() const 
{ 
	return false; 
}

double ResourceInterface::anyLeft(const double &dryMass) const
{
    double leftovers = 0;
	
	if(dryMass - getSpecies()->getMinimumEdibleBiomass()/getSpecies()->getConversionToWetMass() > 0)
	{
		leftovers = dryMass - getSpecies()->getMinimumEdibleBiomass()/getSpecies()->getConversionToWetMass();
	}

	return leftovers;	
}

const double ResourceInterface::getInterpolatedDryMass(const unsigned int evaluationDepth, const double &dryMass) const
{
    double biomassToEvaluate;

	if(static_cast<const PointSpatialTree &>(getTerrainCellInterface()->getPosition()).getDepth() == evaluationDepth)
	{
		biomassToEvaluate = dryMass;
	}
	else
	{
		unsigned int depthDifference = static_cast<const PointSpatialTree &>(getTerrainCellInterface()->getPosition()).getDepth() - evaluationDepth;
		biomassToEvaluate = dryMass / static_cast<double>(displacementPower(1, DIMENSIONS * depthDifference));
	}

	return MathFunctions::linearInterpolate01(biomassToEvaluate, getSpecies()->getInstarK_Value(Instar(getSpecies()->getNumberOfInstars()), static_cast<const SpatialTreeInterface &>(getTerrainCellInterface()->getMapInterface()).getCellSize(evaluationDepth)));
}

const double ResourceInterface::turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const 
{ 
    return min(predatorVoracity / profitability, leftovers);
}

void ResourceInterface::setNewLifeStage(const LifeStage newLifeStage) 
{
	throwLineInfoException("No implementation");
}


void ResourceInterface::setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath) 
{
	throwLineInfoException("No implementation");
}


void ResourceInterface::setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath, int predatorId) 
{
	throwLineInfoException("No implementation");
}

const double ResourceInterface::calculateDryMass() const 
{ 
    return getBiomass() / getSpecies()->getConversionToWetMass();
}

const double ResourceInterface::calculateWetMass() const
{
    if(getBiomass() < getSpecies()->getCellMass())
	{
		return 0.0;  //this is because no less than one biological cell can possibly grow
	}
	else
	{
		return getBiomass();	
	}
}

const double ResourceInterface::calculateWetMass(const double &dryMass) const
{
    return getSpecies()->convertToWetMass(dryMass);
}

bool ResourceInterface::canEatEdible(const EdibleInterface* const &edible, const list<const EdibleInterface*> &ediblesHasTriedToPredate, const double &dryMass) const
{ 
	return false; 
}

bool ResourceInterface::predateEdible(EdibleInterface &edibleToBePredated, const double &targetDryMass, const Ring* const perceptionArea, int day, bool retaliation, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea)
{ 
	return false; 
}

bool ResourceInterface::isDepleted(double foodDemand, const double &dryMass) const
{ 
	//if(this->position->getX() == 1 && this->position->getY() == 1){
	//if((foodDemand > 25) && (biomass/getSpecies()->getConversionToWetMass() - foodDemand < 1)){
	/* cout << "here:  " << endl;
	cout << biomass/getSpecies()->getConversionToWetMass() << endl;
	cout << foodDemand << endl;
	cout << ((biomass / getSpecies()->getConversionToWetMass()) - foodDemand) << endl;
	cout << getSpecies()->getMinimumEdibleBiomass()/getSpecies()->getConversionToWetMass() << endl;  */
	//exit(-1);
	//}
	//  
	bool out = false;
	if((round((dryMass)*pow(10,7)) - round(foodDemand*pow(10,7))) < round((getSpecies()->getMinimumEdibleBiomass()/getSpecies()->getConversionToWetMass())*pow(10,7))){
		out = true;
		//biomass = getSpecies()->getMinimumEdibleBiomass(); this makes resource to stop growing¿¿??
	}

	return out;  
}

void ResourceInterface::removeBiomass()
{
	setBiomass(0.0);
}
