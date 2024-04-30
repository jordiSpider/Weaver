#ifndef ANIMAL_INTERFACE_GROWTH_H_
#define ANIMAL_INTERFACE_GROWTH_H_


#include <string>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>
#include <memory>

#include "IBM/Physics/Temperature.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"




class AnimalInterfaceGrowth
{
protected:
    
public:
    AnimalInterfaceGrowth();

    virtual ~AnimalInterfaceGrowth();

    virtual const double& getCurrentBodySize() const=0;
};

#endif // ANIMAL_INTERFACE_GROWTH_H_
