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

Instar operator+(const Instar& lhs, const unsigned int& rhs)
{
    return Instar(lhs.value + rhs);
}

Instar operator+(const unsigned int& lhs, const Instar& rhs)
{
    return Instar(lhs + rhs.value);
}

bool operator<(const Instar& lhs, const Instar& rhs)
{
    return lhs.value < rhs.value;
}

bool operator<=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value <= rhs.value;
}

bool operator>(const Instar& lhs, const Instar& rhs)
{
    return lhs.value > rhs.value;
}

bool operator>=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value >= rhs.value;
}

bool operator==(const Instar& lhs, const Instar& rhs)
{
    return lhs.value == rhs.value;
}

bool operator!=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value != rhs.value;
}

ostream& operator<<(ostream& os, const Instar& instar)
{
    os << (instar.value + 1);
    return os;
}

Instar::operator size_t() const {
    return static_cast<size_t>(value);
}

size_t hash<Instar>::operator()(const Instar& instar) const
{
    return hash<unsigned int>{}(instar.getValue());
}

template <class Archive>
void Instar::serialize(Archive &ar, const unsigned int version) {
    ar & value;
}

// Specialisation
template void Instar::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Instar::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Instar::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Instar::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
