#include "World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "Maths/Constants.h"


using namespace std;
using json = nlohmann::json;


unsigned int ResourceSpecies::resourceSpeciesCounter = 0;

const id_type& ResourceSpecies::getResourceSpeciesCounter()
{
	return resourceSpeciesCounter;
}


ResourceSpecies::ResourceSpecies(const json &resourceSpeciesInfo) :
	Species(resourceSpeciesInfo["name"], 1), resourceSpeciesId(resourceSpeciesCounter++),
	cellMass(resourceSpeciesInfo["cellMass"]),
	ACTIVATION_ENERGY(resourceSpeciesInfo["ACTIVATION_ENERGY"]),
	NORMALIZATION_B(resourceSpeciesInfo["NORMALIZATION_B"]),
	minHR(resourceSpeciesInfo["maxHR"]), maxHR(resourceSpeciesInfo["minHR"]),
	maxRScale(resourceSpeciesInfo["maxRScale"]), variableIntrinsicRateOfIncrease(resourceSpeciesInfo["variableIntrinsicRateOfIncrease"]),
	minimumEdibleBiomass(static_cast<unsigned int>(resourceSpeciesInfo["minimumNumberOfEdibleSpores"]) * static_cast<double>(resourceSpeciesInfo["cellMass"]))
{
	maxR = -1; // Will always have positive values

	growingParabola = new Parabola();

	this->setConversionToWetMass(resourceSpeciesInfo["conversionToWetMass"]);
}

ResourceSpecies::~ResourceSpecies()
{
	delete growingParabola;
}

const id_type& ResourceSpecies::getResourceSpeciesId() const
{
	return resourceSpeciesId;
}

bool ResourceSpecies::getVariableIntrinsicRateOfIncrease() const 
{ 
	return variableIntrinsicRateOfIncrease; 
}

void ResourceSpecies::updateK_Density(const double &resourceMaximumCapacityDensity)
{
	double newK_Density = convertToDryMass(resourceMaximumCapacityDensity);

	Instar speciesInstar(getNumberOfInstars());
	setInstarK_Density(speciesInstar, (getInstarK_Density(speciesInstar) < newK_Density) ? newK_Density : getInstarK_Density(speciesInstar));
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

double ResourceSpecies::getRateOfGrowth(Temperature temperature, double moisture, double massRatio)
{
	double oldMaxR = maxR;

	maxR = maxRScale * exp(NORMALIZATION_B - ACTIVATION_ENERGY * ( INV_BOLZMANN / (temperature.getTemperatureKelvin()))) * pow(cellMass*massRatio, -0.25);
	
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

double ResourceSpecies::calculateInstarK_Density(const Instar &instar, 
		vector<InstarVector<bool>> &checkedAnimalSpecies, vector<InstarVector<bool>> &cannibalismAnimalSpecies)
{
	return getInstarK_Density(Instar(getNumberOfInstars())) * 0.1;
}

void ResourceSpecies::generateInstarInvolvedResourceSpecies(const Instar &instar, vector<ResourceSpecies*> &instarInvolvedResourceSpecies, vector<InstarVector<bool>> &alreadyCheckedSpecies)
{
	instarInvolvedResourceSpecies.push_back(this);
}
