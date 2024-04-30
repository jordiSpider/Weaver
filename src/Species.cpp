/*
 * species.cpp
 *
 *  Created on: May 24, 2013
 *      Author: jrbcast
 */

#include "Species.h"


using namespace std;



unsigned int Species::speciesCounter = 0;

Species::Species(const string& scientificName) : id(speciesCounter++), scientificName(trim(scientificName)) {}

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

