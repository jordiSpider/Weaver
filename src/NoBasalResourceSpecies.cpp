#include "NoBasalResourceSpecies.h"


using namespace std;

NoBasalResourceSpecies::NoBasalResourceSpecies(const string& scientificName, const double& ACTIVATION_ENERGY, const double& NORMALIZATION_B, 
    const string& patchesFolderName)
    : ResourceSpecies(scientificName, ACTIVATION_ENERGY, NORMALIZATION_B, patchesFolderName)
{

}