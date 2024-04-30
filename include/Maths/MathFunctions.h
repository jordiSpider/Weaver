#ifndef MATH_FUNCTIONS_H_
#define MATH_FUNCTIONS_H_

#include <vector>
#include "Physics/Temperature.h"

class MathFunctions
{
public:
    static double dells(Temperature temperature, Temperature Topt, double ED, double activationEnergy);
    static double linearInterpolate(double x, double min_x, double max_x, double min_y, double max_y);
    static double linearInterpolate01(double x, double min_x, double max_x);
    static double linearInterpolate01(double x, double max_x);
    static std::vector<std::vector<double>> cholesky(double LinfKcorr);
};



#endif /* MATH_FUNCTIONS_H_ */
