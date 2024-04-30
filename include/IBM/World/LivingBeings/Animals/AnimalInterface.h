#ifndef ANIMAL_INTERFACE_H_
#define ANIMAL_INTERFACE_H_


#include <string>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>
#include <memory>

#include "IBM/World/LivingBeings/Edible.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"
#include "IBM/World/LivingBeings/Animals/AnimalInterfaceGrowth.h"



class AnimalInterface: public Edible
{
protected:
    
public:
    AnimalInterface(Species* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar);

    virtual ~AnimalInterface();

    virtual const AnimalInterfaceGrowth* const getAnimalGrowth() const=0;
	virtual AnimalInterfaceGrowth* const getMutableAnimalGrowth()=0;
};

#endif // ANIMAL_INTERFACE_H_
