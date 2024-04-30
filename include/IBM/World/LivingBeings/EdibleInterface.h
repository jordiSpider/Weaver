#ifndef EDIBLE_INTERFACE_H_
#define EDIBLE_INTERFACE_H_


#include <list>
#include <ostream>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Misc/Types.h"
#include "IBM/World/LivingBeings/Species.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "IBM/World/Map/Geometry/Geometry.h"


class EdibleInterface
{
private:
	friend class boost::serialization::access;

public:
	EdibleInterface();
	virtual ~EdibleInterface();

	virtual const id_type getId() const=0;
	virtual const Species* const getSpecies() const=0;
	virtual Species* const getMutableSpecies()=0;
	virtual const Instar& getInstar() const=0;
	virtual const double getSpeed() const=0;
	virtual const double getVoracity() const=0;
	virtual std::string_view getIdStr() const=0;
	virtual int getPredatedByID() const=0;

	virtual void incrementEncountersWithPredator(const int &predatorId)=0;

	virtual const bool isHunting() const=0;
	virtual bool isDepleted(double foodDemand, const double &dryMass) const=0;
	virtual double anyLeft(const double &dryMass) const=0;

	virtual const double getInterpolatedDryMass(const unsigned int evaluationDepth, const double &dryMass) const=0;
	virtual const double turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const=0;

	virtual void setNewLifeStage(const LifeStage newLifeStage)=0;
	virtual void setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps)=0;
	virtual void setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps, int predatorId)=0;

	virtual double substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea)=0;

	virtual const double calculateDryMass() const=0;
	virtual const double calculateWetMass() const=0;
	virtual const double calculateWetMass(const double &dryMass) const=0;
	virtual bool canEatEdible(const EdibleInterface* const &edible, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, const double &dryMass) const=0;
	virtual bool predateEdible(EdibleInterface &edibleToBePredated, const double &targetDryMass, const Ring* const perceptionArea, const unsigned int numberOfTimeSteps, bool retaliation, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea)=0;

	/**
	 * @brief Serialize the EdibleInterface object.
	 * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
	 * @param ar The archive to use.
	 * @param version The version of the serialization format.
	 */
	template <class Archive>
	void serialize(Archive &ar, const unsigned int version);
};

#endif /* EDIBLE_INTERFACE_H_ */
