/*
 * parabola.h
 *
 *  Created on: May 24, 2013
 *      Author: jrbcast
 */

#ifndef PARABOLA_H_
#define PARABOLA_H_

/**
 * This class stores a parabola of the form y=ax2+bx+c
 */
class Parabola
{
	double a;
	double b;
	double c;
public:
	Parabola();
	Parabola(double zero1, double zero2, double max);
	void redefine(double zero1, double zero2, double max);
	virtual ~Parabola();

	double getValue(double x);
	double getA() const;
	double getB() const;
	double getC() const;
	void setA(double a);
	void setB(double b);
	void setC(double c);
};

#endif /* PARABOLA_H_ */
