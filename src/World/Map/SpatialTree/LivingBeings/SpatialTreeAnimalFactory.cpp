#include "World/Map/SpatialTree/LivingBeings/SpatialTreeAnimalFactory.h"


using namespace std;



AnimalInterface* SpatialTreeAnimalFactory::createInstance(double factorEggMassFromMom, double timeStep, const Instar &instar, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender,
		bool temporary)
{
    switch(AnimalFactory::Type::Default) {
		case AnimalFactory::Type::Default: {
			return new SpatialTreeAnimal(factorEggMassFromMom, timeStep, instar, g_numb_prt1, g_numb_prt2, ID_prt1, ID_prt2, mySpecies, gender, temporary);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}

AnimalInterface* SpatialTreeAnimalFactory::createInstance(Gamete* const firstParentGamete, Gamete* const secondParentGamete, double factorEggMassFromMom, double timeStep, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender,
		bool temporary)
{
    switch(AnimalFactory::Type::Default) {
		case AnimalFactory::Type::Default: {
			return new SpatialTreeAnimal(firstParentGamete, secondParentGamete, factorEggMassFromMom, timeStep, g_numb_prt1, g_numb_prt2, ID_prt1, ID_prt2, mySpecies, gender, temporary);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}
