#ifndef MATH_FUNCTIONS_H_
#define MATH_FUNCTIONS_H_

#include <vector>
#include "Output.h"

class Math_Functions
{
public:
    static double dells(double temperature, double Topt, double ED, double activationEnergy);
	static double linearInterpolate(double x, double min_x, double max_x, double min_y, double max_y);
    static std::vector<std::vector<double> > cholesky(double LinfKcorr);
};



#endif /* MATH_FUNCTIONS_H_ */
