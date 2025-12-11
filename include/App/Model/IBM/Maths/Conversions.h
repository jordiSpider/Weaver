/**
 * @file Conversions.h
 * @brief Functions for converting between organism length and dry mass using allometric relationships.
 *
 * Provides functions to convert from length to dry mass and vice versa
 * based on a power-law relationship: dryMass = A * length^B.
 */

#ifndef CONVERSIONS_H_
#define CONVERSIONS_H_

#include <cmath>

#include "App/Model/IBM/Maths/PreciseDouble.h"


/**
 * @brief Converts a given length to dry mass using an allometric relationship.
 *
 * The conversion uses the formula: dryMass = coefficientForMassA * length^scaleForMassB
 *
 * @param length The length of the organism.
 * @param coefficientForMassA Coefficient A in the allometric equation.
 * @param scaleForMassB Exponent B in the allometric equation.
 * @return The corresponding dry mass of the organism.
 */
PreciseDouble convertLengthToDryMass(const PreciseDouble& length, const PreciseDouble& coefficientForMassA, const PreciseDouble& scaleForMassB);

/**
 * @brief Converts a given dry mass to organism length using the inverse allometric relationship.
 *
 * The conversion uses the formula: length = (dryMass / coefficientForMassA)^(1/scaleForMassB)
 *
 * @param dryMass The dry mass of the organism.
 * @param coefficientForMassA Coefficient A in the allometric equation.
 * @param scaleForMassB Exponent B in the allometric equation.
 * @return The corresponding length of the organism.
 */
PreciseDouble convertDryMassToLength(const PreciseDouble& dryMass, const PreciseDouble& coefficientForMassA, const PreciseDouble& scaleForMassB);


#endif /* CONVERSIONS_H_ */
