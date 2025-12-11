/**
 * @file Types.h
 * @brief Defines common type aliases and constants used throughout the Weaver project.
 *
 * This file provides type definitions for 3D coordinates, vectors, IDs, dates,
 * and constants for numerical limits. It simplifies the usage of common geometric
 * and numeric types in the simulation.
 */

#ifndef TYPES_H_
#define TYPES_H_

#include "Coordinate3D.h"
#include <cfloat>
#include <limits>

#include "App/Model/IBM/Maths/PreciseDouble.h"



// -----------------------------------------------------------------------------
// Geometric types
// -----------------------------------------------------------------------------

/**
 * @typedef iVector3D
 * @brief 3D vector with integer components.
 */
typedef Coordinate3D<int> iVector3D;

/**
 * @typedef iPoint3D
 * @brief 3D point with integer coordinates.
 */
typedef Coordinate3D<int> iPoint3D;

/**
 * @typedef uiVector3D
 * @brief 3D vector with unsigned integer components.
 */
typedef Coordinate3D<unsigned int> uiVector3D;

/**
 * @typedef uiPoint3D
 * @brief 3D point with unsigned integer coordinates.
 */
typedef Coordinate3D<unsigned int> uiPoint3D;

/**
 * @typedef dVector3D
 * @brief 3D vector with double precision components (PreciseDouble).
 */
typedef Coordinate3D<PreciseDouble> dVector3D;

/**
 * @typedef dPoint3D
 * @brief 3D point with double precision coordinates (PreciseDouble).
 */
typedef Coordinate3D<PreciseDouble> dPoint3D;


// -----------------------------------------------------------------------------
// Time and ID types
// -----------------------------------------------------------------------------

/**
 * @typedef date_type
 * @brief Type used to represent a date (unsigned integer).
 */
typedef unsigned int date_type;

/**
 * @def MAX_NUM_DIGITS_DAY
 * @brief Maximum number of decimal digits in a date_type value.
 *
 * Adds 1 because numeric_limits::digits10 ignores the sign bit.
 */
#define MAX_NUM_DIGITS_DAY (std::numeric_limits<unsigned int>::digits10 + 1)

/**
 * @typedef id_type
 * @brief Type used to represent unique IDs (size_t).
 */
typedef size_t id_type;

/**
 * @def MAX_NUM_DIGITS_ID
 * @brief Maximum number of decimal digits in an id_type value.
 *
 * Adds 1 because numeric_limits::digits10 ignores the sign bit.
 */
#define MAX_NUM_DIGITS_ID (std::numeric_limits<id_type>::digits10 + 1)


// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

/**
 * @def NEG_DBL_MAX
 * @brief Maximum negative value for double (DBL_MAX with negative sign).
 */
#define NEG_DBL_MAX (-1 * DBL_MAX)


#endif /* TYPES_H_ */
