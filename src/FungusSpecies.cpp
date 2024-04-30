/*
 * FungusSpecies.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: jrbcast
 */

#include "FungusSpecies.h"
#include "Maths/Constants.h"

FungusSpecies::FungusSpecies(const std::string& newScientificName, const double& ACTIVATION_ENERGY, const double& NORMALIZATION_B, const string& patchesDirectoryName) :
	Species(newScientificName)
{
	this->patchesDirectoryName = patchesDirectoryName;
	maxR = -1; // Will always have positive values
	this->ACTIVATION_ENERGY = ACTIVATION_ENERGY;
	this->NORMALIZATION_B = NORMALIZATION_B;

	growingParabola = new Parabola();
}

FungusSpecies::~FungusSpecies()
{
	delete growingParabola;
}

void FungusSpecies::setGrowingRateParams(const double& minHR, const double& maxHR, const double& maxRScale)
{
	this->maxHR = maxHR;
	this->minHR = minHR;
	this->maxRScale = maxRScale;
}

double FungusSpecies::getRateOfGrowth(double temperature, double moisture)
{
	double oldMaxR = maxR;
	maxR = maxRScale * exp(NORMALIZATION_B - ACTIVATION_ENERGY * ( INV_BOLZMANN / (temperature+273.15))) * pow(cellMass*0.001, -0.25); //in kg/1000 = g

	if (oldMaxR != maxR)
	{
		growingParabola->redefine(minHR, maxHR, maxR);
	}

	// An equation of the form aX^2+bX+c=Y (simplified to x(ax+b)+c=Y
	return growingParabola->getValue(moisture);
}

double FungusSpecies::getCellMass()
{
	return cellMass;
}

