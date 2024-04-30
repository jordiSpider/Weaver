#ifndef RESOURCE_HPP_
#define RESOURCE_HPP_

#include <list>
#include <vector>
#include <math.h>
#include "Edible.h"
#include "SimulationConstants.h"
#include "Maths/Constants.h"
#include "LineInfoException.h"
#include "SimulType.h"
#include "ResourceSpecies.h"
#include "Output.h"
#include "Types.h"
#include "Maths/Math_Functions.h"

#include <float.h>
#include <iostream>
#include <cmath>



class Resource: public Edible
{
public:
	static id_type resourceCounter;

	Resource(ResourceSpecies* const mySpecies, float biomass, double resourceMaximumCapacity, bool competitionAmongResourceSpecies, double massRatio, bool patchSpread);
	virtual ~Resource();

	ResourceSpecies* const getSpecies() const override;

	virtual const double getVoracity() const;
	virtual const double getCurrentBodySize() const;
	virtual const double getSpeed() const;
	virtual const bool isHunting() const;
	virtual void incrementEncountersWithPredator(const int &predatorId);

	virtual bool canEatEdible(Edible* edible, const std::list<Edible*> &ediblesHasTriedToPredate);

	virtual int getPredatedByID() const;

	virtual void setNewLifeStage(const LifeStage newLifeStage);
	virtual void setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath);
	virtual void setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath, int predatorId);

	void setResourceMaximumCapacity(const double& resourceMaximumCapacity);
	double getResourceMaximumCapacity() const;
	

	const double calculateDryMass() const; 
	const double getInterpolatedDryMass() const;  
																									
	const double turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const;
	const double calculateWetMass();
	virtual bool predateEdible(Edible* edibleToBePredated, int day, bool retaliation, std::list<Edible*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea);

	virtual void substractBiomass(double dryMassToBeSubstracted);
	virtual bool isExtinct();
	
	virtual bool isDepleted(double foodDemand);

	virtual double anyLeft();


	// In order to effectively increase biomass from resource coming from other cells, this must be called
	virtual void processExternalContributions(double totalMaximumResourceCapacity, double totalBiomass);
	// Grows the resource within this cell and returns the new population
	virtual double grow(int timeStep, double totalMaximumResourceCapacity, double moisture, double temperature, double totalBiomass, SimulType simulType); //Dinosaurs debug   , int timeStepsPerDay
	virtual void setBiomass(double biomass);
	
	//arthro and for dinos
	virtual void addBiomass(double addedMass, bool competition, double maxCapacity, int size, double totBiomass);

	// Set this resource population to 0
	void erradicate();

	bool canSpread() const;

protected:
	double biomass;
	// This is to avoid reprocessing the same resource on the same day
	bool newlyAdded;
	double newBiomass;

	double resourceMaximumCapacity;

	const bool competitionAmongResourceSpecies;

	const double massRatio;

	const bool patchSpread;

	double getNewBiomass(const double &rateOfIncrease, const double &timeStepsPerDay, SimulType simulType) const;
};

#endif /* RESOURCE_HPP_ */
