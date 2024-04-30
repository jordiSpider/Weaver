
#include "IBM/Maths/Functions.h"


const double VonBertalanffy(const double& Linf, const double& k, const double& t, const double& t0)
{
    return Linf*(1-exp(-k*(t-t0)));
}

const double Logistic(const double& Linf, const double& xmid, const double& k, const double& t)
{
    return Linf/(1+exp(k*(xmid-t)));
}

const double Exponential(const double& a, const double& b, const double& k, const double& t)
{
    return exp(k*t + a) + b;
}
