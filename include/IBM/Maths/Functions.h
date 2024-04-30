#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <cmath>


const double VonBertalanffy(const double& Linf, const double& k, const double& t, const double& t0);

const double Logistic(const double& Linf, const double& xmid, const double& k, const double& t);

const double Exponential(const double& a, const double& b, const double& k, const double& t);


#endif /* FUNCTIONS_H_ */
