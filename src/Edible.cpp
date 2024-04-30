/*
 * Edible.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#include "Edible.h"

int Edible::edibleId = 0;

Edible::Edible(Species* mySpecies, bool temporary)
{
	if(!temporary){
		id = edibleId++;
	}
	this->mySpecies = mySpecies;
}

Edible::~Edible()
{
	// TODO Auto-generated destructor stub
}
