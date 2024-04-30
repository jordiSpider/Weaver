/*
 * FungusSpecies.h
 *
 *  Created on: Jun 11, 2013
 *      Author: jrbcast
 */

#ifndef WEAVER_FUNGUSSPECIES_H_
#define WEAVER_FUNGUSSPECIES_H_

#include "Maths/Parabola.h"
#include "Species.h"
#include <math.h>

class FungusSpecies: public Species
{
	string patchesDirectoryName;
	double cellMass;

	double ACTIVATION_ENERGY;
	double NORMALIZATION_B;

	// Moisture limits for fungus grow in % percent
	double minHR;
	double maxHR;

	// Maximum growing rate
	double maxR;
	double maxRScale;
	double minimumEdibleBiomass;
	Parabola * growingParabola;

public:

	friend class Fungus;

	FungusSpecies(const std::string& scientificName, const double& ACTIVATION_ENERGY, const double& NORMALIZATION_B, const string& patchesDirectoryName);
	virtual ~FungusSpecies();


	double getCellMass();
	inline void setCellMass(const double& cellMass) { this->cellMass = cellMass; }
	void setGrowingRateParams(const double& minHR, const double& maxHR, const double& maxRscale);
	inline void setMinimumEdibleBiomass(const double& minimumNumberOfEdibleSpores, const double& cellMass) { minimumEdibleBiomass = minimumNumberOfEdibleSpores * cellMass; }
	virtual string getPatchesDirectoryName() { return patchesDirectoryName; };
	virtual double getMinHR() { return minHR; };
	virtual double getMaxHR() { return maxHR; };
	virtual double getRateOfGrowth(double temperature, double moisture);
	virtual double getMinimumEdibleBiomass() { return minimumEdibleBiomass; };
	virtual bool isMobile() { return false; };
	virtual float getAttackProbability() { return 0.0; };
	virtual float getExposedAttackProbability() { return 0.0; };
	virtual float getKillProbability() { return 0.0; };
	virtual float getAttackAvoidingProbability() { return -1.0; };
	virtual float getKillAvoidingProbability() { return -1.0; };
};

#endif /* WEAVER_FUNGUSSPECIES_H_ */
