#include "ResourceSpecies.h"
#include "Maths/Constants.h"


using namespace std;


unsigned int ResourceSpecies::resourceSpeciesCounter = 0;

const id_type& ResourceSpecies::getResourceSpeciesCounter()
{
	return resourceSpeciesCounter;
}


ResourceSpecies::ResourceSpecies(const std::string& newScientificName, const double& ACTIVATION_ENERGY, const double& NORMALIZATION_B, const string& patchesFolderName) :
	Species(newScientificName), resourceSpeciesId(resourceSpeciesCounter++)
{
	this->patchesFolderName = patchesFolderName;
	maxR = -1; // Will always have positive values
	this->ACTIVATION_ENERGY = ACTIVATION_ENERGY;
	this->NORMALIZATION_B = NORMALIZATION_B;

	setNumberOfInstars(1);

	growingParabola = new Parabola();
}

ResourceSpecies::~ResourceSpecies()
{
	delete growingParabola;
}

bool ResourceSpecies::getVariableIntrinsicRateOfIncrease() const 
{ 
	return variableIntrinsicRateOfIncrease; 
}

void ResourceSpecies::updateK_Value(const double &resourceMaximumCapacity)
{
	double newK_Value = convertToDryMass(resourceMaximumCapacity);

	setK_Value((K_Value < newK_Value) ? newK_Value : K_Value);
}

double ResourceSpecies::getMaxRScale() const 
{ 
	return maxRScale; 
}

void ResourceSpecies::setCellMass(const double& cellMass) 
{ 
	this->cellMass = cellMass; 
}

void ResourceSpecies::setMinimumEdibleBiomass(const double& minimumNumberOfEdibleSpores, const double& cellMass) 
{ 
	minimumEdibleBiomass = minimumNumberOfEdibleSpores * cellMass; 
}

std::string ResourceSpecies::getPatchesFolderName() const 
{ 
	return patchesFolderName; 
}

double ResourceSpecies::getMinHR() const 
{ 
	return minHR; 
}

double ResourceSpecies::getMaxHR() const 
{ 
	return maxHR; 
}

double ResourceSpecies::getMinimumEdibleBiomass() const 
{ 
	return minimumEdibleBiomass; 
}

bool ResourceSpecies::isMobile() const 
{ 
	return false; 
}

float ResourceSpecies::getAttackProbability() const 
{ 
	return 0.0; 
}

float ResourceSpecies::getExposedAttackProbability() const 
{ 
	return 0.0; 
}

float ResourceSpecies::getKillProbability() const 
{ 
	return 0.0; 
}

float ResourceSpecies::getAttackAvoidingProbability() const 
{ 
	return -1.0; 
}

float ResourceSpecies::getKillAvoidingProbability() const 
{ 
	return -1.0; 
}

void ResourceSpecies::setGrowingRateParams(const double& minHR, const double& maxHR, const double& maxRScale, const bool& variableIntrinsicRateOfIncrease)
{
	this->maxHR = maxHR;
	this->minHR = minHR;
	this->maxRScale = maxRScale;
	this->variableIntrinsicRateOfIncrease = variableIntrinsicRateOfIncrease;
}

double ResourceSpecies::getRateOfGrowth(double temperature, double moisture, double massRatio)
{
	double oldMaxR = maxR;

	maxR = maxRScale * exp(NORMALIZATION_B - ACTIVATION_ENERGY * ( INV_BOLZMANN / (temperature+273.15))) * pow(cellMass*massRatio, -0.25);
	
	if (oldMaxR != maxR)
	{
		growingParabola->redefine(minHR, maxHR, maxR);
	}

	// An equation of the form aX^2+bX+c=Y (simplified to x(ax+b)+c=Y
	return growingParabola->getValue(moisture);
}

double ResourceSpecies::getCellMass() const
{
	return cellMass;
}
