#include "IBM/World/LivingBeings/Animals/AnimalInterface.h"


using namespace std;


AnimalInterface::AnimalInterface(Species* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar)
	: Edible(mySpecies, terrainCellInterface, instar)
{

}

AnimalInterface::~AnimalInterface()
{

}
