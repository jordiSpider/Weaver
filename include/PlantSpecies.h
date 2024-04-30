/*
 * PlantSpecies.h
 *
 *  Created on: Jul 6, 2013
 *      Author: jrbcast
 */

#ifndef WEAVER_PLANTSPECIES_H_
#define WEAVER_PLANTSPECIES_H_

#include "Species.h"

class PlantSpecies : public Species
{
public:
	friend class Plant;

	PlantSpecies(std::string newScientificName);
	virtual ~PlantSpecies();
};

#endif /* WEAVER_PLANTSPECIES_H_ */
