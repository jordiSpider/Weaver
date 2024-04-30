#ifndef WEAVER_RESOURCESPECIES_H_
#define WEAVER_RESOURCESPECIES_H_

#include "Maths/Parabola.h"
#include "Species.h"
#include "LineInfoException.h"
#include <math.h>
#include <unordered_map>
#include <string>

#include <magic_enum.hpp>

#define FMT_HEADER_ONLY
#include <fmt/core.h>



class ResourceSpecies: public Species
{
public:
	static const id_type& getResourceSpeciesCounter();


	ResourceSpecies(const std::string& scientificName, const double& ACTIVATION_ENERGY, const double& NORMALIZATION_B, const std::string& patchesFolderName);
	virtual ~ResourceSpecies();

	const id_type& getResourceSpeciesId() const;

	double getCellMass() const;
	bool getVariableIntrinsicRateOfIncrease() const;
	double getMaxRScale() const;
	void setCellMass(const double& cellMass);
	void setGrowingRateParams(const double& minHR, const double& maxHR, const double& maxRscale, const bool& variableIntrinsicRateOfIncrease);
	void setMinimumEdibleBiomass(const double& minimumNumberOfEdibleSpores, const double& cellMass);
	virtual std::string getPatchesFolderName() const;
	virtual double getMinHR() const;
	virtual double getMaxHR() const;
	virtual double getRateOfGrowth(double temperature, double moisture, double massRatio);
	virtual double getMinimumEdibleBiomass() const;
	virtual bool isMobile() const;
	virtual float getAttackProbability() const;
	virtual float getExposedAttackProbability() const;
	virtual float getKillProbability() const;
	virtual float getAttackAvoidingProbability() const;
	virtual float getKillAvoidingProbability() const;
	void updateK_Value(const double &resourceMaximumCapacity);

private:
	static id_type resourceSpeciesCounter;

	const id_type resourceSpeciesId;
	std::string patchesFolderName;
	double cellMass;

	double ACTIVATION_ENERGY;
	double NORMALIZATION_B;

	// Moisture limits for resource grow in % percent
	double minHR;
	double maxHR;

	// Maximum growing rate
	double maxR;
	double maxRScale;
	bool variableIntrinsicRateOfIncrease;
	double minimumEdibleBiomass;
	Parabola * growingParabola;
};

#endif /* WEAVER_RESOURCESPECIES_H_ */
