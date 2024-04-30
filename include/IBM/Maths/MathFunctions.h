#ifndef MATH_FUNCTIONS_H_
#define MATH_FUNCTIONS_H_

#include <vector>
#include "IBM/Physics/Temperature.h"

class MathFunctions
{
private:
    static double johnsonLewis(Temperature temperature, Temperature Topt, double ED, double activationEnergy);

public:
    static double useJohnsonLewis(Temperature newT, double activationEnergy, Temperature lowerThreshold, Temperature upperThreshold, Temperature Topt, double Ed, double trait);
    static double linearInterpolate(double x, double min_x, double max_x, double min_y, double max_y);
    static double linearInterpolate01(double x, double min_x, double max_x);
    static double linearInterpolate01(double x, double max_x);
    static std::vector<std::vector<double>> cholesky(double LinfKcorr);
};



#endif /* MATH_FUNCTIONS_H_ */
