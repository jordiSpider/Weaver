/*
 * Edible.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#include "Edible.h"

using namespace std;


LifeStage::LifeStage(const LifeStageValue value) : value(value) {}

LifeStage::LifeStage(const LifeStage &other) : value(other.value) {}

ostream& operator<<(ostream& os, const LifeStage lifeStage)
{
	os << magic_enum::enum_name(lifeStage.value);
	
	return os;
}

id_type Edible::edibleId = 0;

Edible::Edible(Species* const mySpecies, const bool temporary) : mySpecies(mySpecies), temporary(temporary)
{
	if(!temporary){
		id = edibleId++;
		generateIdStr();
	}
}

Edible::~Edible()
{
	// TODO Auto-generated destructor stub
}

ostream& operator<<(ostream& os, const Edible& edible)
{
	os << edible.getIdStr() << "\t";

	return os;
}

const Instar& Edible::getInstar() const 
{ 
	return instar; 
}
