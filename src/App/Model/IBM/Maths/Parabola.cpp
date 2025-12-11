/**
 * @file Parabola.cpp
 * @brief Contains the implementation of the Parabola class.
 * @author jrbcast
 */

#include "App/Model/IBM/Maths/Parabola.h"

Parabola::Parabola(const PreciseDouble& zero1, const PreciseDouble& zero2, const PreciseDouble& max)
{
	const PreciseDouble middlePointX = (zero1 + zero2) / 2.0;
	const PreciseDouble middlePointY = max;
	const PreciseDouble denom = 1.0 / ((zero1 - zero2) * (zero1 - middlePointX) * (zero2 - middlePointX));

	a = (zero2 * (-middlePointY) + zero1 * (middlePointY)) * denom;
	b = (zero2 * zero2 * middlePointY + zero1 * zero1 * (-middlePointY)) * denom;
	c = (zero1 * zero2 * (zero1 - zero2) * middlePointY) * denom;
}

void Parabola::redefine(const PreciseDouble& zero1, const PreciseDouble& zero2, const PreciseDouble& max)
{
	PreciseDouble middlePointX = (zero1 + zero2) / 2.0;
	PreciseDouble middlePointY = max;
	PreciseDouble denom = 1.0 / ((zero1 - zero2) * (zero1 - middlePointX) * (zero2 - middlePointX));

	a = (zero2 * (-middlePointY) + zero1 * (middlePointY)) * denom;
	b = (zero2 * zero2 * middlePointY + zero1 * zero1 * (-middlePointY)) * denom;
	c = (zero1 * zero2 * (zero1 - zero2) * middlePointY) * denom;
}

const PreciseDouble& Parabola::getA() const
{
	return a;
}

const PreciseDouble& Parabola::getB() const
{
	return b;
}

const PreciseDouble& Parabola::getC() const
{
	return c;
}

void Parabola::setA(const PreciseDouble& value)
{
	a = value;
}

void Parabola::setB(const PreciseDouble& value)
{
	b = value;
}

void Parabola::setC(const PreciseDouble& value)
{
	c = value;
}

PreciseDouble Parabola::getValue(const PreciseDouble& x) const
{
	return (x * (a * x + b) + c);
}

Parabola::~Parabola()
{
}
