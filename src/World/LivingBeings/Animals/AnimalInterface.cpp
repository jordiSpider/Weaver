#include "World/LivingBeings/Animals/AnimalInterface.h"

using namespace std;



AnimalInterface::AnimalInterface(Species* const mySpecies, const Instar &instar, const bool temporary) 
    : Edible(mySpecies, nullptr, instar, temporary)
{
	
}

AnimalInterface::~AnimalInterface()
{

}

AnimalSpecies* const AnimalInterface::getSpecies() const 
{ 
	return static_cast<AnimalSpecies* const>(Edible::getSpecies()); 
}
