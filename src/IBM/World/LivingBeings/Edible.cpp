/*
 * Edible.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#include "IBM/World/LivingBeings/Edible.h"

using namespace std;




id_type Edible::edibleId = 0;


Edible::Edible(Species* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar, const bool temporary) 
	: EdibleInterface(),
	  mySpecies(mySpecies), temporary(temporary), instar(instar), terrainCellInterface(terrainCellInterface)
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


Species* const Edible::getSpecies() const 
{ 
	return mySpecies; 
}


const TerrainCellInterface* const Edible::getTerrainCellInterface() const
{ 
	return terrainCellInterface; 
}


TerrainCellInterface* const Edible::getMutableTerrainCellInterface() const
{
	return terrainCellInterface;
}


void Edible::setTerrainCellInterface(TerrainCellInterface* newTerrainCellInterface)
{ 
	terrainCellInterface = newTerrainCellInterface; 
}


void Edible::setInstar(const Instar& newInstar)
{
	instar = newInstar;
}


template <class Archive>
void Edible::serialize(Archive &ar, const unsigned int version) {
	ar & edibleId; 
	ar & id;
	ar & idStr;
	ar & mySpecies;
	ar & temporary;
	ar & instar;
	ar & terrainCellInterface;
	ar & edibleId;
}
