#include "Maths/OptimisedOperators.h"

unsigned int displacementPower(const unsigned int displacement, const unsigned int exponent)
{
    return displacement << exponent;
}

unsigned int displacementDivision(const unsigned int displacement, const unsigned int exponent)
{
    return exponent >> displacement;
}

