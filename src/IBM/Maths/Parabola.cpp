/**
 * @file Parabola.cpp
 * @brief Contains the implementation of the Parabola class.
 * @author jrbcast
 */

#include "IBM/Maths/Parabola.h"

Parabola::Parabola()
{
	a = 1;
	b = 1;
	c = 1;
}

Parabola::Parabola(double zero1, double zero2, double max)
{
	double middlePointX = (zero1 + zero2) / 2.0;
	double middlePointY = max;
	double denom = 1.0 / ((zero1 - zero2) * (zero1 - middlePointX) * (zero2 - middlePointX));

	a = (zero2 * (-middlePointY) + zero1 * (middlePointY)) * denom;
	b = (zero2 * zero2 * middlePointY + zero1 * zero1 * (-middlePointY)) * denom;
	c = (zero1 * zero2 * (zero1 - zero2) * middlePointY) * denom;
}

void Parabola::redefine(double zero1, double zero2, double max)
{
	double middlePointX = (zero1 + zero2) / 2.0;
	double middlePointY = max;
	double denom = 1.0 / ((zero1 - zero2) * (zero1 - middlePointX) * (zero2 - middlePointX));

	a = (zero2 * (-middlePointY) + zero1 * (middlePointY)) * denom;
	b = (zero2 * zero2 * middlePointY + zero1 * zero1 * (-middlePointY)) * denom;
	c = (zero1 * zero2 * (zero1 - zero2) * middlePointY) * denom;
}

double Parabola::getA() const
{
	return a;
}

double Parabola::getB() const
{
	return b;
}

double Parabola::getC() const
{
	return c;
}

void Parabola::setA(double a)
{
	this->a = a;
}

void Parabola::setB(double b)
{
	this->b = b;
}

void Parabola::setC(double c)
{
	this->c = c;
}

double Parabola::getValue(double x)
{
	return (x * (a * x + b) + c);
}

Parabola::~Parabola()
{
}

template <class Archive>
void Parabola::serialize(Archive &ar, const unsigned int version) {
	ar & a;
	ar & b;
	ar & c;
}

// Specialisation
template void Parabola::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Parabola::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Parabola::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Parabola::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
