#ifndef SPATIAL_TREE_ANIMAL_FACTORY_H_
#define SPATIAL_TREE_ANIMAL_FACTORY_H_


#include "Exceptions/LineInfoException.h"
#include "World/LivingBeings/Animals/AnimalFactory.h"
#include "World/LivingBeings/Animals/AnimalInterface.h"
#include "World/Map/SpatialTree/LivingBeings/SpatialTreeAnimal.h"



class SpatialTreeAnimalFactory : public AnimalFactory
{
public:
	static AnimalInterface* createInstance(double eggMassAtBirth, double timeStep, const Instar &instar, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender,
			bool temporary=false);
	static AnimalInterface* createInstance(Gamete* const firstParentGamete, Gamete* const secondParentGamete, double eggMassAtBirth, double timeStep, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender,
			bool temporary=false);
};

#endif // SPATIAL_TREE_ANIMAL_FACTORY_H_
