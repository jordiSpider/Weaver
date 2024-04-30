/*
 * Edible.h
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#ifndef EDIBLE_H_
#define EDIBLE_H_

#include <ostream>
#include <list>
#include "Types.h"

class Species;
class TerrainCell;
class Genome;

using namespace std;

class Edible {
private:
	id_type id;
	static int edibleId;

protected:
	Species* mySpecies;

public:
	Edible(Species* mySpecies, bool temporary=false);
	virtual ~Edible();

	void doDefinitive() { id = edibleId++; };

	id_type getId() const { return id; };
	inline std::string getIdStr() { 
		stringstream ss;
		ss << std::setw(MAX_NUM_DIGITS_ID) << std::setfill('0') << id;
		return ss.str();
	};
	static void resetIds() { Edible::edibleId = 0; };
	void decrementIds() { Edible::edibleId--; };
	Species* getSpecies() { return mySpecies; };

	virtual double calculateDryMass()=0;
	virtual double turnIntoDryMassToBeEaten(double predatorVoracity, float profitability, double leftovers)=0;
	virtual double calculateWetMass()=0;
	virtual double getVoracity() { return 0.0; };
	virtual double getMassAtBirth() { return 0.0; };
	virtual double getRemainingVoracity() { return 0.0; };
	virtual double getAssim() { return 0.0; };
	virtual double getCurrentBodySize() { return 0.0; };
	virtual double getSpeed() { return 0.0; };
	virtual int getHuntingMode() { return -1; };
	virtual void incrementEncountersWithPredator(int predatorId) {};
	virtual TerrainCell* getPosition()=0;

	virtual bool canEatEdible(Edible* edible) { return false; };
	virtual double calculateEncounterProbability(Edible* edibleToBeEncountered) { return 0.0; };
	virtual bool encounterEdible(Edible* searchedAnimal, float attackOrExposedProbability, int timeStep, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile) { return false; };
	virtual double calculatePredationProbability(Edible* edibleToBePredated, bool retaliation) { return 0.0; };
	virtual bool predateEdible(Edible* edibleToBePredated, int day, bool retaliation, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile) { return false; };
	virtual double calculateEdibilityValue(Edible* edibleToBeEvaluated) { return 0.0; };
	virtual double calculateEdibilityValueWithMass(Edible* edibleToBeEvaluated) { return 0.0; };
	virtual float getMeanExperience(Species* species) { return 0.0; };

	virtual bool isSated() { return false; };
	virtual bool isExhausted() { return false; };

	virtual bool isMature() { return false; };
	virtual bool isMated() { return false; };
	virtual unsigned int getGender() { return 0; };

	virtual unsigned int getLifeStage() { return 0; };
	virtual void setNewLifeStage(unsigned int newLifeStage) {};
	virtual void setNewLifeStage(unsigned int newLifeStage, double dayOfDeath) {};
	virtual void setNewLifeStage(unsigned int newLifeStage, double dayOfDeath, int predatorId) {};
	virtual double getDateOfDeath() { return -1.0; };
	virtual void setDateOfDeath(double dayOfDeath) {};
	virtual int getPredatedByID() { return -1; };
	virtual void setPredatedByID(int predatorId) {};

	virtual pair<bool, bool> interpolateTraits() { return make_pair(false, false); };
	virtual void printTraits(ostream& traitsFile) {};
	virtual void adjustTraits() {};
	virtual void isReadyToBeBorn(int timeStep, int timeStepsPerDay) {};
	virtual void isReadyToResumeFromPupaOrDecreasePupaTimer() {};
	virtual void isReadyToResumeFromHandlingOrDecreaseHandlingTimer() {};
	virtual void isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(float relativeHumidity) {};
	virtual void calculateGrowthCurves(float temperature, ostream& tuneTraitsFile, bool printGrowthData, double ageAtInitialization) {}; //Dinosaurs
	virtual double getEggDryMassAtBirth() { return 0.0; };
	virtual void tuneTraits(int timeStep, int timeStepsPerDay, float temperature, float relativeHumidity, ostream& tuneTraitsFile, bool printGrowthData, bool fromForceMolting1) {};
	virtual TerrainCell* move(int timeStep, int timeStepsPerDay, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile) { return NULL; };
	virtual double calculatePredatoryRiskEdibilityValue(Edible* edibleToBeEvaluated) { return 0.0; };
	virtual bool hasTriedToHunt(Edible* edibleToCheck) { return false; };
	virtual void printVoracities(int timeStep, int timeStepsPerDay, ostream& voracitiesFile) {};
	virtual void dieFromBackground(int timeStep) {};
	virtual void assimilateFoodMass(int timeStep) {};
	virtual void metabolize(int timeStep, int timeStepPerDay) {};
	virtual void grow(int timeStep, int timeStepsPerDay) {};
	virtual list<Edible*> * breed(int timeStep, int timeStepsPerDay, float temperature) { return NULL; };
	virtual bool postBreeding(int timeStep, int timeStepsPerDay) { return false; };
	virtual const Genome* getGenome() const { return NULL; };
	virtual int getGenerationNumberFromMaleParent() const { return 0; };
	virtual void setGenomeFromMatedMale(Edible* matedMale) {};
	virtual void setPosition(TerrainCell* newPosition) {};
	virtual void setDateEgg(double newDateEgg) {};
	virtual void setMassAtBirth(double massAtBirth) {};
	virtual list<int> * getEncounterEvents() { return NULL; };

	virtual void increaseAge(int increase) {};

	virtual void substractBiomass(double dryMassToBeSubstracted) {};
	virtual bool isExtinct() { return false; };
	virtual bool isDepleted(double foodDemand) { return false; };
	virtual double anyLeft() { return 0.0; };
	virtual void processExternalContributions() {};
	virtual double grow(int timeStep) { return 0.0; }; //Dinosaur debugging , int timeStepsPerDay
	virtual void setBiomass(double biomass) {};
	virtual void addBiomass(float addedMass, bool competition, double maxCapacity, int size, double totBiomass) {};

	friend ostream& operator<<(ostream& os, Edible& edible) { return edible.print(os); }
	virtual ostream& print(ostream& os) { return os; };
};



#endif /* EDIBLE_H_ */
