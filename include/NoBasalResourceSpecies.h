#ifndef WEAVER_NOBASALRESOURCESPECIES_H_
#define WEAVER_NOBASALRESOURCESPECIES_H_

#include "ResourceSpecies.h"


class NoBasalResourceSpecies: public ResourceSpecies
{
public:
	NoBasalResourceSpecies(const std::string& scientificName, const double& ACTIVATION_ENERGY, const double& NORMALIZATION_B, const std::string& patchesFolderName);

private:
	
};

#endif /* WEAVER_NOBASALRESOURCESPECIES_H_ */
