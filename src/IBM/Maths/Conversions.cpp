
#include "IBM/Maths/Conversions.h"

using namespace std;


const double convertLengthToDryMass(const double& length, const double& coefficientForMassA, const double& scaleForMassB)
{
    return coefficientForMassA * pow(length, scaleForMassB);
}

const double convertDryMassToLength(const double& dryMass, const double& coefficientForMassA, const double& scaleForMassB)
{
    return pow(dryMass/coefficientForMassA, 1/scaleForMassB);
}
