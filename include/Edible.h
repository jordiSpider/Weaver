/*
 * Edible.h
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#ifndef EDIBLE_H_
#define EDIBLE_H_

#include <ostream>
#include <string>

#include <magic_enum.hpp>
#include "Types.h"
#include "Species.h"
#include "Types.h"
#include "LineInfoException.h"
#include <list>


class TerrainCellInterface;

class Edible {
private:
	static id_type edibleId;

	id_type id;
	std::string idStr;

protected:
	Species* const mySpecies;

	bool temporary;
	Instar instar;

	explicit Edible(Species* const mySpecies, const bool temporary=false);
	~Edible();

public:
	class LifeStage {
	public:
		enum LifeStageValue : unsigned int
		{
			UNBORN, 
			ACTIVE, 
			STARVED, 
			PREDATED, 
			REPRODUCING, 
			PUPA, 
			SATIATED, 
			HANDLING, 
			DIAPAUSE, 
			BACKGROUND, 
			SENESCED, 
			SHOCKED
		};

		inline static constexpr size_t size() { return magic_enum::enum_count<LifeStageValue>(); }
		inline static std::string_view to_string(const LifeStageValue& lifeStage) { return magic_enum::enum_name(lifeStage); }
		inline static constexpr auto getEnumValues() { return magic_enum::enum_values<LifeStageValue>(); }

		LifeStage(const LifeStageValue value);
		LifeStage(const LifeStage &other);

		inline const LifeStageValue getValue() const { return value; }
		inline operator int() const { return value; }
		inline bool operator<(const LifeStage& other) const { return this->value < other.value; }
		inline bool operator==(const LifeStage& other) const { return this->value == other.value; }
		friend inline bool operator==(const LifeStage &lhs, const LifeStageValue rhs) { return lhs.value == rhs; }
		friend inline bool operator==(const LifeStageValue lhs, const LifeStage &rhs) { return lhs == rhs.value; }

		friend std::ostream& operator<<(std::ostream& os, const LifeStage lifeStage);

	private:
		LifeStageValue value;
	};

	inline static void resetIds(id_type newValue) { edibleId = newValue; }

	// Getters
	inline const id_type getId() const { return id; }
	inline std::string_view getIdStr() const { return idStr; }
	inline virtual Species* const getSpecies() const { return mySpecies; }

	// Setters
	inline void doDefinitive() { 
		id = Edible::edibleId++;
		generateIdStr();
		temporary = false;
	};
	inline void generateIdStr() { idStr = std::string(MAX_NUM_DIGITS_ID - std::to_string(id).length(), '0') + std::to_string(id); }
	
	const Instar& getInstar() const;

	friend std::ostream& operator<<(std::ostream& os, const Edible& edible);

	// Abstract methods
	virtual const double calculateDryMass() const=0;
	virtual const double getInterpolatedDryMass() const=0;
	virtual const double turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const=0;
	virtual const double calculateWetMass()=0;
	virtual const double getVoracity() const=0;
	virtual const double getCurrentBodySize() const=0;
	virtual const double getSpeed() const=0;
	virtual const bool isHunting() const=0;
	virtual void incrementEncountersWithPredator(const int &predatorId)=0;
	virtual TerrainCellInterface* getPosition() { throwLineInfoException("Incorrect call"); }
	virtual bool canEatEdible(Edible* edible, const std::list<Edible*> &ediblesHasTriedToPredate)=0;
	virtual bool predateEdible(Edible* edibleToBePredated, int day, bool retaliation, std::list<Edible*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea)=0;
	virtual int getPredatedByID() const=0;
	virtual void setNewLifeStage(const LifeStage newLifeStage)=0;
	virtual void setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath)=0;
	virtual void setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath, int predatorId)=0;
	virtual bool isDepleted(double foodDemand)=0;
	virtual double anyLeft()=0;
	virtual void substractBiomass(double dryMassToBeSubstracted)=0;
};

typedef Edible::LifeStage LifeStage;

#endif /* EDIBLE_H_ */
