#ifndef ANIMAL_STATISTICAL_GROWTH_H_
#define ANIMAL_STATISTICAL_GROWTH_H_


#include <string>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/LivingBeings/Animals/AnimalGrowth.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"


class AnimalStatistical;


class AnimalStatisticalGrowth : public AnimalGrowth
{
protected:
    AnimalStatistical* animal;

public:
    AnimalStatisticalGrowth(AnimalStatistical* const animal, const double& factorEggMass);   

    virtual ~AnimalStatisticalGrowth(); 

    const AnimalStatistical* const getAnimal() const;
    AnimalStatistical* const getMutableAnimal() const;

    void forceMolting();
};

#endif // ANIMAL_STATISTICAL_GROWTH_H_
