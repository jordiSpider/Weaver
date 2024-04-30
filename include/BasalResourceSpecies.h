#ifndef WEAVER_BASALRESOURCESPECIES_H_
#define WEAVER_BASALRESOURCESPECIES_H_

#include "ResourceSpecies.h"


class BasalResourceSpecies: public ResourceSpecies
{
public:
	BasalResourceSpecies(const std::string& scientificName, const double& ACTIVATION_ENERGY, const double& NORMALIZATION_B, const std::string& patchesFolderName);

private:
	
};

#endif /* WEAVER_BASALRESOURCESPECIES_H_ */
