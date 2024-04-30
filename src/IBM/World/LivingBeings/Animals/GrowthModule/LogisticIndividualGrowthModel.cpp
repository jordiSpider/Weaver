
#include "IBM/World/LivingBeings/Animals/GrowthModule/LogisticIndividualGrowthModel.h"

#include "IBM/World/LivingBeings/Animals/AnimalGrowth.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"

using namespace std;



LogisticIndividualGrowthModel::LogisticIndividualGrowthModel() 
    : IndividualGrowthModel() 
{

}

LogisticIndividualGrowthModel::~LogisticIndividualGrowthModel() 
{

}

const double& LogisticIndividualGrowthModel::getLinf() const
{
    return Linf;
}

const double& LogisticIndividualGrowthModel::getXmid() const
{
    return xmid;
}

void LogisticIndividualGrowthModel::adjust(const double& growthCoefficient, const double& Lmax, const double& Tmax, const double& lengthAtBirth)
{
    Linf = (Lmax*lengthAtBirth*(exp(growthCoefficient*Tmax)-1)) / (lengthAtBirth*exp(growthCoefficient*Tmax)-Lmax);

    xmid = log((Linf-lengthAtBirth) / lengthAtBirth) / growthCoefficient;
}

double LogisticIndividualGrowthModel::calculateLength(const double& age, const double& growthCoefficient) const
{
    return Logistic(getLinf(), getXmid(), growthCoefficient, age);
}
