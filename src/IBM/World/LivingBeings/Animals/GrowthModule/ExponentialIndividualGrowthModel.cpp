
#include "IBM/World/LivingBeings/Animals/GrowthModule/ExponentialIndividualGrowthModel.h"

#include "IBM/World/LivingBeings/Animals/AnimalGrowth.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"

using namespace std;



ExponentialIndividualGrowthModel::ExponentialIndividualGrowthModel() 
    : IndividualGrowthModel() 
{

}

ExponentialIndividualGrowthModel::~ExponentialIndividualGrowthModel() 
{

}

const double& ExponentialIndividualGrowthModel::getA() const
{
    return a;
}

const double& ExponentialIndividualGrowthModel::getB() const
{
    return b;
}

void ExponentialIndividualGrowthModel::adjust(const double& growthCoefficient, const double& Lmax, const double& Tmax, const double& lengthAtBirth)
{
    a = log((Lmax - lengthAtBirth) / (exp(growthCoefficient*Tmax) - 1));

    b = lengthAtBirth - exp(a);
}

double ExponentialIndividualGrowthModel::calculateLength(const double& age, const double& growthCoefficient) const
{
    return Exponential(getA(), getB(), growthCoefficient, age);
}
