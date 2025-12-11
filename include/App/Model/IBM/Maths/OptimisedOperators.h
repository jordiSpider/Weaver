/**
 * @file OptimisedOperators.h
 * @brief Provides optimized functions for integer power-of-two operations.
 *
 * These functions perform efficient calculations for powers and divisions
 * using bitwise operations, which are faster than standard multiplication
 * or division when the exponent corresponds to powers of 2.
 */

#ifndef OPTIMISED_OPERATORS_H_
#define OPTIMISED_OPERATORS_H_

#include <cstddef>

/**
 * @brief Computes displacement raised to a power of two.
 *
 * This function calculates `displacement * 2^exponent` efficiently using
 * a left bit shift operation.
 *
 * @param displacement The base value.
 * @param exponent The exponent (as a power of 2).
 * @return The result of `displacement * 2^exponent`.
 */
size_t displacementPower(const size_t displacement, const size_t exponent);

/**
 * @brief Computes displacement divided by a power of two.
 *
 * This function calculates `displacement / 2^exponent` efficiently using
 * a right bit shift operation.
 *
 * @param displacement The numerator value.
 * @param exponent The exponent (as a power of 2).
 * @return The result of `displacement / 2^exponent`.
 */
size_t displacementDivision(const size_t displacement, const size_t exponent);

#endif /* OPTIMISED_OPERATORS_H_ */
