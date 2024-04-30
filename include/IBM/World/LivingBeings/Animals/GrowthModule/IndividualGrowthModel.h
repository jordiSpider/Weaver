#ifndef INDIVIDUAL_GROWTH_MODEL_H_
#define INDIVIDUAL_GROWTH_MODEL_H_

#include <utility>

#include "IBM/Maths/MathFunctions.h"
#include "Exceptions/LineInfoException.h"


class AnimalGrowth;


class IndividualGrowthModel {
public:
    enum class Type {
        VonBertalanffy,
        Logistic,
        Exponential
    };

    static std::unique_ptr<IndividualGrowthModel> createInstance(const Type& type);


    IndividualGrowthModel();
    virtual ~IndividualGrowthModel();

    virtual void adjust(const double& growthCoefficient, const double& Lmax, const double& Tmax, const double& lengthAtBirth)=0;

    virtual double calculateLength(const double& age, const double& growthCoefficient) const=0;
};

#endif /* INDIVIDUAL_GROWTH_MODEL_H_ */
