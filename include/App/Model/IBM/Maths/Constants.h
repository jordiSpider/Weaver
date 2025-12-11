/**
 * @file Constants.h
 * @brief Mathematical and physical constants used throughout the project.
 *
 * This header defines commonly used constants such as PI and the Boltzmann constant,
 * along with its inverse for computational efficiency.
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

/**
 * @brief The mathematical constant PI.
 *
 * Represents the ratio of the circumference of a circle to its diameter.
 */
const double PI = 3.141592653589793;

/**
 * @brief Boltzmann constant (k) in double precision.
 *
 * Value in electronvolts per Kelvin (eV/K). Used in temperature-related calculations.
 */
const double BOLTZMANN = 8.62E-5;

/**
 * @brief Inverse of the Boltzmann constant for computational speed-up.
 *
 * Precomputed value of 1 / BOLTZMANN to avoid repeated division operations in performance-critical code.
 */
const double INV_BOLTZMANN = 11600.9280742;

#endif /* CONSTANTS_H_ */
