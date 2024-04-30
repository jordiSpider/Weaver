/*
 * Edible.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#include "IBM/World/LivingBeings/Edible.h"

using namespace std;




id_type Edible::edibleId = 1;


Edible::Edible(const id_type id, Species* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar) 
	: EdibleInterface(),
	  id(id), mySpecies(mySpecies), instar(instar), terrainCellInterface(terrainCellInterface)
{
	generateIdStr();
}

Edible::Edible(Species* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar) 
	: EdibleInterface(),
	  id(0), mySpecies(mySpecies), instar(instar), terrainCellInterface(terrainCellInterface)
{
	generateIdStr();
}


Edible::~Edible()
{
	// TODO Auto-generated destructor stub
}


const id_type Edible::generateId()
{
	return Edible::edibleId++;
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


const Species* const Edible::getSpecies() const 
{ 
	return mySpecies; 
}

Species* const Edible::getMutableSpecies() 
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
	ar & instar;
	ar & terrainCellInterface;
	ar & edibleId;
}
