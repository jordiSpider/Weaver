#ifndef LOGISTIC_INDIVIDUAL_GROWTH_MODEL_H_
#define LOGISTIC_INDIVIDUAL_GROWTH_MODEL_H_

#include <cfloat>
#include <random>
#include <vector>
#include <cmath>

#include "IBM/World/LivingBeings/Animals/GrowthModule/IndividualGrowthModel.h"
#include "IBM/Maths/Functions.h"
#include "IBM/Maths/MathFunctions.h"
#include "IBM/Maths/Conversions.h"
#include "IBM/Maths/Functions.h"



class LogisticIndividualGrowthModel: public IndividualGrowthModel {
public:
    LogisticIndividualGrowthModel();
    virtual ~LogisticIndividualGrowthModel();

    const double& getLinf() const;
    const double& getXmid() const;

    double calculateLength(const double& age, const double& growthCoefficient) const;

    void adjust(const double& growthCoefficient, const double& Lmax, const double& Tmax, const double& lengthAtBirth);

protected:
    double Linf;
    double xmid;
};

#endif /* LOGISTIC_INDIVIDUAL_GROWTH_MODEL_H_ */
