#include "IBM/World/LivingBeings/Animals/AnimalInterface.h"

#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"

using namespace std;



AnimalInterface::AnimalInterface(Species* const mySpecies, const Instar &instar, const bool temporary, TerrainCellInterface* parentTerrainCellInterface) 
    : Edible(mySpecies, parentTerrainCellInterface, instar, temporary)
{
	
}

AnimalInterface::~AnimalInterface()
{

}

AnimalSpecies* const AnimalInterface::getSpecies() const 
{ 
	return static_cast<AnimalSpecies* const>(Edible::getSpecies()); 
}

template <class Archive>
void AnimalInterface::serialize(Archive &ar, const unsigned int version) 
{

}
