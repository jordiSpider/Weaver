#include "BasalResourceSpecies.h"


using namespace std;

BasalResourceSpecies::BasalResourceSpecies(const string& scientificName, const double& ACTIVATION_ENERGY, const double& NORMALIZATION_B, 
    const string& patchesFolderName)
    : ResourceSpecies(scientificName, ACTIVATION_ENERGY, NORMALIZATION_B, patchesFolderName)
{

}