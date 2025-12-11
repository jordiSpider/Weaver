#include "App/Model/IBM/Maths/OptimisedOperators.h"

size_t displacementPower(const size_t displacement, const size_t exponent)
{
    return displacement << exponent;
}

size_t displacementDivision(const size_t displacement, const size_t exponent)
{
    return exponent >> displacement;
}

