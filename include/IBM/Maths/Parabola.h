/**
 * @file Parabola.h
 * @brief Defines the Parabola class.
 * @author jrbcast
 */

#ifndef PARABOLA_H_
#define PARABOLA_H_


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>


/**
 * @class Parabola
 * @brief Represents a parabola of the form y = ax^2 + bx + c.
 */
class Parabola
{
	double a; /**< Coefficient for x^2. */
	double b; /**< Coefficient for x. */
	double c; /**< Constant term. */
public:
	/**
	 * @brief Default constructor for the Parabola class.
	 */
	Parabola();

	/**
	 * @brief Constructor for the Parabola class with specified zeros and maximum point.
	 * @param zero1 First zero of the parabola.
	 * @param zero2 Second zero of the parabola.
	 * @param max Maximum point of the parabola.
	 */
	Parabola(double zero1, double zero2, double max);
	
	/**
	* @brief Redefines the parabola with new specified zeros and maximum point.
	* @param zero1 First zero of the parabola.
	* @param zero2 Second zero of the parabola.
	* @param max Maximum point of the parabola.
	*/
	void redefine(double zero1, double zero2, double max);
	
	/**
	* @brief Virtual destructor for the Parabola class.
	*/
	virtual ~Parabola();

	/**
	* @brief Calculates the value of the parabola for the given x.
	* @param x The input value for the parabola.
	* @return The value of the parabola for the given x.
	*/
	double getValue(double x);

	/**
	* @brief Getter for coefficient a.
	* @return The coefficient a of the parabola.
	*/
	double getA() const;

	/**
	* @brief Getter for coefficient b.
	* @return The coefficient b of the parabola.
	*/
	double getB() const;

	/**
	* @brief Getter for coefficient c.
	* @return The coefficient c of the parabola.
	*/
	double getC() const;

	/**
	* @brief Setter for coefficient a.
	* @param a The new value for the coefficient a.
	*/
	void setA(double a);

	/**
	* @brief Setter for coefficient b.
	* @param b The new value for the coefficient b.
	*/
	void setB(double b);

	/**
	* @brief Setter for coefficient c.
	* @param c The new value for the coefficient c.
	*/
	void setC(double c);

	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* PARABOLA_H_ */
