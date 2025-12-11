
#include "App/Model/IBM/Maths/Conversions.h"

using namespace std;


PreciseDouble convertLengthToDryMass(const PreciseDouble& length, const PreciseDouble& coefficientForMassA, const PreciseDouble& scaleForMassB)
{
    return coefficientForMassA * pow(length, scaleForMassB);
}

PreciseDouble convertDryMassToLength(const PreciseDouble& dryMass, const PreciseDouble& coefficientForMassA, const PreciseDouble& scaleForMassB)
{
    return pow(dryMass/coefficientForMassA, 1/scaleForMassB);
}
