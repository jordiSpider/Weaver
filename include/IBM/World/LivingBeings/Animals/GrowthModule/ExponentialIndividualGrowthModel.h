#ifndef EXPONENTIAL_INDIVIDUAL_GROWTH_MODEL_H_
#define EXPONENTIAL_INDIVIDUAL_GROWTH_MODEL_H_

#include <cfloat>
#include <random>
#include <vector>
#include <cmath>

#include "IBM/World/LivingBeings/Animals/GrowthModule/IndividualGrowthModel.h"
#include "IBM/Maths/Functions.h"
#include "IBM/Maths/MathFunctions.h"
#include "IBM/Maths/Conversions.h"
#include "IBM/Maths/Functions.h"



class ExponentialIndividualGrowthModel: public IndividualGrowthModel {
public:
    ExponentialIndividualGrowthModel();
    virtual ~ExponentialIndividualGrowthModel();

    const double& getA() const;
    const double& getB() const;

    double calculateLength(const double& age, const double& growthCoefficient) const;

    void adjust(const double& growthCoefficient, const double& Lmax, const double& Tmax, const double& lengthAtBirth);

protected:
    double a;
    double b;
};

#endif /* EXPONENTIAL_INDIVIDUAL_GROWTH_MODEL_H_ */
