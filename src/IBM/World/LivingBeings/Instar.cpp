/*
 * Edible.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#include "IBM/World/LivingBeings/Instar.h"

using namespace std;


Instar::Instar()
{
    this->value = 0;
}

Instar::Instar(unsigned int value)
{
    this->value = value - 1;
}

Instar::Instar(const Instar &other)
{
    this->value = other.getValue();
}

Instar::~Instar()
{

}

Instar operator+(const Instar& lhs, const int& rhs)
{
    return Instar(lhs.value + (unsigned int)rhs);
}

Instar operator+(const int& lhs, const Instar& rhs)
{
    return Instar((unsigned int)lhs + rhs.value);
}

bool operator<(const Instar& lhs, const Instar& rhs)
{
    return lhs.value < rhs.value;
}

bool operator<(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) < (unsigned int)rhs;
}

bool operator<(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs < (rhs.value+1);
}

bool operator<=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value <= rhs.value;
}

bool operator<=(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) <= (unsigned int)rhs;
}

bool operator<=(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs <= (rhs.value+1);
}

bool operator>(const Instar& lhs, const Instar& rhs)
{
    return lhs.value > rhs.value;
}

bool operator>(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) > (unsigned int)rhs;
}

bool operator>(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs > (rhs.value+1);
}

bool operator>=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value >= rhs.value;
}

bool operator>=(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) >= (unsigned int)rhs;
}

bool operator>=(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs >= (rhs.value+1);
}

bool operator==(const Instar& lhs, const Instar& rhs)
{
    return lhs.value == rhs.value;
}

bool operator==(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) == (unsigned int)rhs;
}

bool operator==(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs == (rhs.value+1);
}

bool operator!=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value != rhs.value;
}

bool operator!=(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) != (unsigned int)rhs;
}

bool operator!=(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs != (rhs.value+1);
}

ostream& operator<<(ostream& os, const Instar& instar)
{
    os << (instar.value + 1);
    return os;
}

size_t hash<Instar>::operator()(const Instar& instar) const
{
    return hash<unsigned int>{}(instar.getValue());
}

template <class Archive>
void Instar::serialize(Archive &ar, const unsigned int version) {
    ar & value;
}
