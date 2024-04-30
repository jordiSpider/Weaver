#ifndef VON_BERTALANFFY_INDIVIDUAL_GROWTH_MODEL_H_
#define VON_BERTALANFFY_INDIVIDUAL_GROWTH_MODEL_H_

#include <cfloat>
#include <random>
#include <vector>
#include <cmath>

#include "IBM/World/LivingBeings/Animals/GrowthModule/IndividualGrowthModel.h"
#include "IBM/Maths/Functions.h"
#include "IBM/Maths/MathFunctions.h"
#include "IBM/Maths/Conversions.h"
#include "IBM/Maths/Functions.h"



class VonBertalanffyIndividualGrowthModel: public IndividualGrowthModel {
public:
    VonBertalanffyIndividualGrowthModel();
    virtual ~VonBertalanffyIndividualGrowthModel();

    const double& getLinf() const;
    const double& getT0() const;

    double calculateLength(const double& age, const double& growthCoefficient) const;

    void adjust(const double& growthCoefficient, const double& Lmax, const double& Tmax, const double& lengthAtBirth);

protected:
    double Linf;
    double t0;
};

#endif /* VON_BERTALANFFY_INDIVIDUAL_GROWTH_MODEL_H_ */
