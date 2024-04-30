#ifndef RESOURCE_INTERFACE_H_
#define RESOURCE_INTERFACE_H_

#include <memory>
#include <vector>

#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "IBM/World/LivingBeings/Edible.h"
#include "IBM/World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "Exceptions/LineInfoException.h"


class TerrainCellInterface;
class SpatialTree;


class ResourceInterface : public Edible
{
protected:
	double biomass;
	bool fullCapacity;

public:
	ResourceInterface(const id_type id, Species* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar, const double &biomass);
	virtual ~ResourceInterface();

	const ResourceSpecies* const getSpecies() const;
	ResourceSpecies* const getMutableSpecies();
	const double getSpeed() const;
	const double getVoracity() const;
	int getPredatedByID() const;
	void incrementEncountersWithPredator(const int &predatorId);
	const bool isHunting() const;
	double anyLeft(const double &dryMass) const;
	const double getInterpolatedDryMass(const unsigned int evaluationDepth, const double &dryMass) const;
	const double turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const;
	void setNewLifeStage(const LifeStage newLifeStage);
	void setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps);
	void setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps, int predatorId);
	virtual double substractBiomass(double dryMassToBeSubstracted);
	double substractBiomass(double dryMassToBeSubstracted, const double &minimumDryMass);
	virtual double substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea);
	virtual double substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea, const double &minimumDryMass);
	virtual void removeBiomass();
	void updateFullCapacityStatus();
	const double calculateDryMass() const;
	const double calculateWetMass() const;
	const double calculateWetMass(const double &dryMass) const;
	bool canEatEdible(const EdibleInterface* const &edible, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, const double &dryMass) const;
	bool predateEdible(EdibleInterface &edibleToBePredated, const double &targetDryMass, const Ring* const perceptionArea, const unsigned int numberOfTimeSteps, bool retaliation, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea);

	bool isDepleted(double foodDemand, const double &dryMass) const;

	virtual double getResourceMaximumCapacity() const=0;
	virtual bool canSpread() const=0;
	virtual void update()=0;
	void setBiomass(const double &newBiomass);
	const double& getBiomass() const;
	virtual double addBiomass(double addedMass, const TerrainCellInterface* const source)=0;
	virtual double addBiomass(double addedMass, const std::unique_ptr<std::vector<unsigned int>> &childrenPositions)=0;
	virtual void setFullCapacity(const bool newFullCapacityValue, const TerrainCellInterface* const source)=0;
	virtual void setFullCapacity(const bool newFullCapacityValue, const std::unique_ptr<std::vector<unsigned int>> &childrenPositions)=0;
	void setFullCapacity(const bool newFullCapacityValue);
	const bool isFullCapacity() const;
};

#endif /* RESOURCE_INTERFACE_H_ */
