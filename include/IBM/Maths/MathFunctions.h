#ifndef MATH_FUNCTIONS_H_
#define MATH_FUNCTIONS_H_

#include <vector>
#include "IBM/Physics/Temperature.h"

class MathFunctions
{
private:
    static double johnsonLewis(Temperature temperature, Temperature Topt, double ED, double activationEnergy);

public:
    static double useJohnsonLewis(const Temperature& newT, const double& activationEnergy, const Temperature& lowerThreshold, const Temperature& upperThreshold, const Temperature& Topt, const double& Ed, const double& traitAtTopt);
    static double linearInterpolate(double x, double min_x, double max_x, double min_y, double max_y);
    static double linearInterpolate01(double x, double min_x, double max_x);
    static double linearInterpolate01(double x, double max_x);
    static double linearExtrapolation(double x, double min_x, double max_x, double interpolationResult);
    static std::vector<std::vector<double>> cholesky(const std::vector<double>& sigmaValues);
};



#endif /* MATH_FUNCTIONS_H_ */
