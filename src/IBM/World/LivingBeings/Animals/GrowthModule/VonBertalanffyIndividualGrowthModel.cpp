
#include "IBM/World/LivingBeings/Animals/GrowthModule/VonBertalanffyIndividualGrowthModel.h"

#include "IBM/World/LivingBeings/Animals/AnimalGrowth.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"

using namespace std;



VonBertalanffyIndividualGrowthModel::VonBertalanffyIndividualGrowthModel() 
    : IndividualGrowthModel() 
{

}

VonBertalanffyIndividualGrowthModel::~VonBertalanffyIndividualGrowthModel() 
{

}

const double& VonBertalanffyIndividualGrowthModel::getLinf() const
{
    return Linf;
}

const double& VonBertalanffyIndividualGrowthModel::getT0() const
{
    return t0;
}

void VonBertalanffyIndividualGrowthModel::adjust(const double& growthCoefficient, const double& Lmax, const double& Tmax, const double& lengthAtBirth)
{
    Linf = (Lmax*exp(growthCoefficient*Tmax) - lengthAtBirth) / (exp(growthCoefficient*Tmax) - 1);

    t0 = log(1-(lengthAtBirth/Linf))/growthCoefficient;
}

double VonBertalanffyIndividualGrowthModel::calculateLength(const double& age, const double& growthCoefficient) const
{
    return VonBertalanffy(getLinf(), growthCoefficient, age, getT0());
}
