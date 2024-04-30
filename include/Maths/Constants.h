/*
 * constants.h
 *
 *  Created on: Jul 31, 2012
 *      Author: jrbcast
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

/**
 * @file constants.h
 * @brief Contains declarations of mathematical constants.
 */

// Math constants

/**
 * @brief The mathematical constant PI.
 */
const double PI = 3.141592653589793;

/**
 * @brief Boltzmann constant in double precision.
 *
 * This constant represents the Boltzmann constant in double precision.
 */
const double BOLZMANN = 8.62E-5;

/**
 * @brief Inverse of the Boltzmann constant for speed-up.
 *
 * This constant represents the inverse of the Boltzmann constant,
 * intended for speed-up calculations.
 */
const double INV_BOLZMANN = 11600.9280742;

#endif /* CONSTANTS_H_ */