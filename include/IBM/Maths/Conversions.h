#ifndef CONVERSIONS_H_
#define CONVERSIONS_H_

#include <cmath>


const double convertLengthToDryMass(const double& length, const double& coefficientForMassA, const double& scaleForMassB);

const double convertDryMassToLength(const double& dryMass, const double& coefficientForMassA, const double& scaleForMassB);


#endif /* CONVERSIONS_H_ */
