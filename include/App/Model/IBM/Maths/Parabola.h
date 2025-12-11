/**
 * @file Parabola.h
 * @brief Defines the Parabola class representing a quadratic function y = ax^2 + bx + c.
 *
 * This class allows defining a parabola using its two zeros and maximum point,
 * and provides methods to evaluate the function and access or modify its coefficients.
 */

#ifndef PARABOLA_H_
#define PARABOLA_H_


#include "App/Model/IBM/Maths/PreciseDouble.h"


/**
 * @class Parabola
 * @brief Represents a parabola of the form y = ax^2 + bx + c.
 *
 * The parabola can be defined by specifying its two zeros and the value of its
 * maximum (or minimum) point. Provides getters and setters for coefficients,
 * as well as a method to evaluate the function at a given x.
 */
class Parabola
{
private:
	PreciseDouble a; /**< Coefficient for x^2. */
    PreciseDouble b; /**< Coefficient for x. */
    PreciseDouble c; /**< Constant term. */

public:
	/**
     * @brief Constructor for the Parabola class with specified zeros and maximum point.
     * @param zero1 First zero of the parabola.
     * @param zero2 Second zero of the parabola.
     * @param max Maximum (or minimum) value of the parabola.
     */
	Parabola(const PreciseDouble& zero1, const PreciseDouble& zero2, const PreciseDouble& max);
	
	/**
     * @brief Redefines the parabola with new specified zeros and maximum point.
     * @param zero1 First zero of the parabola.
     * @param zero2 Second zero of the parabola.
     * @param max Maximum (or minimum) value of the parabola.
     */
	void redefine(const PreciseDouble& zero1, const PreciseDouble& zero2, const PreciseDouble& max);
	
	/**
     * @brief Virtual destructor for the Parabola class.
     */
	virtual ~Parabola();

	/**
     * @brief Calculates the value of the parabola for a given x.
     * @param x The input value for the parabola.
     * @return The corresponding y value of the parabola.
     */
	PreciseDouble getValue(const PreciseDouble& x) const;

	/**
     * @brief Getter for coefficient a.
     * @return The coefficient a of the parabola.
     */
	const PreciseDouble& getA() const;

	/**
     * @brief Getter for coefficient b.
     * @return The coefficient b of the parabola.
     */
	const PreciseDouble& getB() const;

	/**
     * @brief Getter for coefficient c.
     * @return The coefficient c of the parabola.
     */
	const PreciseDouble& getC() const;

	/**
     * @brief Setter for coefficient a.
     * @param value The new value for the coefficient a.
     */
	void setA(const PreciseDouble& value);

	/**
     * @brief Setter for coefficient b.
     * @param value The new value for the coefficient b.
     */
	void setB(const PreciseDouble& value);

	/**
     * @brief Setter for coefficient c.
     * @param value The new value for the coefficient c.
     */
	void setC(const PreciseDouble& value);
};

#endif /* PARABOLA_H_ */
