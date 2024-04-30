#ifndef ANIMAL_GROWTH_H_
#define ANIMAL_GROWTH_H_


#include <string>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>
#include <memory>

#include "IBM/World/LivingBeings/Animals/AnimalInterfaceGrowth.h"
#include "IBM/Physics/Temperature.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"
#include "IBM/World/LivingBeings/Instar.h"
#include "IBM/World/LivingBeings/Animals/GrowthModule/IndividualGrowthModel.h"
#include "Misc/CustomIndexedVector.h"



class Animal;


class AnimalGrowth : public AnimalInterfaceGrowth
{
protected:
    Animal* animal;

    CustomIndexedVector<Instar, double> individualInstarDevTimeVector;
    CustomIndexedVector<Instar, double> instarLengthsVector;
    CustomIndexedVector<Instar, double> instarMassesVector;

    IndividualGrowthModel* individualGrowthModel;

    double eggDryMassAtBirth;
    double lengthAtBirth;

    double baseDevTimeProportion;

    bool currentBodySizeInitialised;
    unsigned int currentBodySizeLastTimeStep;

    double currentBodySize;

    bool growthModuleTemperatureDependent;

    void calculateValuesAtBirth(const double& factorEggMass);

    virtual void calculateGrowthCurves();

    void setDryMass(const double &newDryMass, const double& investment);
    void setDryMass(const double &newDryMass, const double &newCurrentBodySize, const double& newEnergyTank);

    #ifdef DEBUG
		void testSetCurrentBodySize();
        void testGetCurrentBodySize() const;
	#endif

    void setCurrentBodySize(const double& newCurrentBodySize);

    const bool isGrowthModuleTemperatureDependent() const;

public:
    AnimalGrowth(Animal* const animal, const double& factorEggMass, const bool growthModuleTemperatureDependent);

    virtual ~AnimalGrowth();

    const Animal* const getAnimal() const;
    Animal* const getMutableAnimal() const;

    const double& getCurrentBodySize() const;

    const double& getInstarAge(const Instar& instar) const;
    const CustomIndexedVector<Instar, double>& getIndividualInstarDevTimeVector() const;

    const IndividualGrowthModel* const getIndividualGrowthModel() const;

    const double& getEggDryMassAtBirth() const;
    const double& getLengthAtBirth() const;

    void setDryMass(const double &newDryMass);

    virtual void forceMolting()=0;
};

#endif // ANIMAL_GROWTH_H_
