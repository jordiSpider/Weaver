#ifndef WEAVER_RESOURCESPECIES_H_
#define WEAVER_RESOURCESPECIES_H_

#include <math.h>
#include <unordered_map>
#include <string>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include "IBM/Maths/Parabola.h"
#include "IBM/World/LivingBeings/Species.h"
#include "Exceptions/LineInfoException.h"
#include "IBM/Physics/Temperature.h"
#include "Misc/Macros.h"


class ResourceSpecies: public Species
{
public:
	static const id_type& getResourceSpeciesCounter();


	ResourceSpecies(const nlohmann::json &resourceSpeciesInfo);
	virtual ~ResourceSpecies();

	const id_type& getResourceSpeciesId() const;

	double getCellMass() const;
	bool getVariableIntrinsicRateOfIncrease() const;
	double getMaxRScale() const;
	void setCellMass(const double& cellMass);
	void setGrowingRateParams(const double& minHR, const double& maxHR, const double& maxRscale, const bool& variableIntrinsicRateOfIncrease);
	void setMinimumEdibleBiomass(const double& minimumNumberOfEdibleSpores, const double& cellMass);
	virtual double getMinHR() const;
	virtual double getMaxHR() const;
	virtual double getRateOfGrowth(Temperature temperature, double moisture, double massRatio);
	virtual double getMinimumEdibleBiomass() const;
	virtual bool isMobile() const;
	virtual float getAttackProbability() const;
	virtual float getExposedAttackProbability() const;
	virtual float getKillProbability() const;
	virtual float getAttackAvoidingProbability() const;
	virtual float getKillAvoidingProbability() const;
	void updateK_Density(const double &resourceMaximumCapacityDensity);
	void generateInstarInvolvedResourceSpecies(
		const Instar &instar, std::vector<ResourceSpecies*> &instarInvolvedResourceSpecies, 
		std::vector<InstarVector<bool>> &alreadyCheckedSpecies
	);
	double calculateInstarK_Density(const Instar &instar, std::vector<InstarVector<bool>> &checkedAnimalSpecies, 
		std::vector<InstarVector<bool>> &cannibalismAnimalSpecies
	);

private:
	static id_type resourceSpeciesCounter;

	const id_type resourceSpeciesId;
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
