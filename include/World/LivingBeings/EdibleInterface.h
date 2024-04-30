#ifndef EDIBLE_INTERFACE_H_
#define EDIBLE_INTERFACE_H_


#include <list>
#include <ostream>
#include <magic_enum.hpp>

#include "Misc/Types.h"
#include "World/LivingBeings/Species.h"
#include "World/LivingBeings/LifeStage.h"
#include "World/Map/Geometry/Geometry.h"


class EdibleInterface
{
public:
	EdibleInterface();
	virtual ~EdibleInterface();

	virtual const id_type getId() const=0;
	virtual Species* const getSpecies() const=0;
	virtual const Instar& getInstar() const=0;
	virtual const double getSpeed() const=0;
	virtual const double getVoracity() const=0;
	virtual std::string_view getIdStr() const=0;
	virtual const double getCurrentBodySize() const=0;
	virtual int getPredatedByID() const=0;

	virtual void incrementEncountersWithPredator(const int &predatorId)=0;

	virtual const bool isHunting() const=0;
	virtual bool isDepleted(double foodDemand, const double &dryMass) const=0;
	virtual double anyLeft(const double &dryMass) const=0;

	virtual const double getInterpolatedDryMass(const unsigned int evaluationDepth, const double &dryMass) const=0;
	virtual const double turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const=0;

	virtual void setNewLifeStage(const LifeStage newLifeStage)=0;
	virtual void setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath)=0;
	virtual void setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath, int predatorId)=0;

	virtual double substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea)=0;

	virtual const double calculateDryMass() const=0;
	virtual const double calculateWetMass() const=0;
	virtual const double calculateWetMass(const double &dryMass) const=0;
	virtual bool canEatEdible(const EdibleInterface* const &edible, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, const double &dryMass) const=0;
	virtual bool predateEdible(EdibleInterface &edibleToBePredated, const double &targetDryMass, const Ring* const perceptionArea, int day, bool retaliation, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea)=0;
};

#endif /* EDIBLE_INTERFACE_H_ */
