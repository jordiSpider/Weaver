/*
 * Edible.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#include "IBM/World/LivingBeings/Edible.h"

#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"
#include "IBM/World/Map/MapInterface.h"
#include "IBM/World/WorldInterface.h"

using namespace std;





Edible::Edible(TerrainCellInterface* terrainCellInterface)
	: EdibleInterface(), terrainCellInterface(terrainCellInterface)
{

}

Edible::Edible(Species* const mySpecies, const Instar &instar, const bool temporary) 
	: EdibleInterface(),
	  mySpecies(mySpecies), temporary(temporary), instar(instar), terrainCellInterface(nullptr)
{
	if(!temporary){
		id = mySpecies->getMutableWorldInterface()->generateEdibleId();
		generateIdStr();
	}
}

Edible::Edible(Species* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar, const bool temporary) 
	: EdibleInterface(),
	  mySpecies(mySpecies), temporary(temporary), instar(instar), terrainCellInterface(terrainCellInterface)
{
	if(!temporary){
		id = mySpecies->getMutableWorldInterface()->generateEdibleId();
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

void Edible::doDefinitive() 
{ 
	id = getSpecies()->getMutableWorldInterface()->generateEdibleId();
	generateIdStr();
	temporary = false;
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
	ar & boost::serialization::base_object<EdibleInterface>(*this);
	
	ar & id;
	ar & idStr;

	Species::ID speciesID;
	if (Archive::is_loading::value)
	{
		ar & speciesID;
		mySpecies = terrainCellInterface->getMutableMapInterface().getMutableWorldInterface()->getMutableExistingSpecies().at(speciesID.getValue());
	}
	else
	{
		speciesID = mySpecies->getId();
		ar & speciesID;
	}

	ar & temporary;
	ar & instar;
}

// Specialisation
template void Edible::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Edible::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Edible::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Edible::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
