/*
 * species.cpp
 *
 *  Created on: May 24, 2013
 *      Author: jrbcast
 */

#include "Species.h"
#include <cfloat>


using namespace std;



unsigned int Species::speciesCounter = 0;

const id_type& Species::getSpeciesCounter()
{
	return speciesCounter;
}


Species::Species(const string& scientificName) : id(speciesCounter++), scientificName(trim(scientificName)), K_Value(NEG_DBL_MAX) {}

Species::~Species()
{
}

bool Species::operator<(const Species& other) const
{
	return this->id < other.id;
}

bool Species::operator==(const Species& other) const {
	return (this->scientificName == other.scientificName);
}

const double& Species::getK_Value() const
{
	return K_Value;
}

void Species::setK_Value(const double &newK_Value)
{
	K_Value = newK_Value;
}

double Species::convertToWetMass(const double &dryMass) const
{
	return getConversionToWetMass() * dryMass;
}

double Species::convertToDryMass(const double &wetMass) const
{
	return wetMass / getConversionToWetMass();
}

const unsigned int& Species::getNumberOfInstars() const 
{ 
	return numberOfInstars; 
}

void Species::setNumberOfInstars(const unsigned int& numberOfInstars) 
{ 
	this->numberOfInstars = numberOfInstars; 
}
