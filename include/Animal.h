/*
 * Animal.h
 *
 *  Created on: 20 Nov 2012
 *      Author: gabi
 */

#ifndef WEAVER_ANIMAL_H_
#define WEAVER_ANIMAL_H_
#include <cmath>
#include <vector>
#include <unordered_map>
#include <list>
#include "Chromosome.h"
#include "ChromosomesGenerator.h"
#include "Genome.h"
#include "SimulationConstants.h"
#include "AnimalSpecies.h"
#include "Fungus.h"
#include <float.h>
#include <iostream>
#include <math.h>
#include "Types.h"

class TerrainCell;

using namespace std;

//TODO Modify the bullet related to traits here.
/**
 * These are the specifications of each individual. An Animal has at least the following properties:
 * 	- An unique ID.
 * 	- An array of pairs of Chromosome, each pair containing the genetic material related to each trait.
 * 	- An array of traits ordered (according to GlobalFunctions).
 *
 * 	The following are state variables:
 * 	- The current position of the animal.
 * 	- The number of steps actually walked in one day.
 * 	- The amount of food eaten in one day.
 * 	- The current state of the animal: unborn, active, starved, predated or reproduced.
 *	- The date of death.
 *
 *	We also store the initial values of some traits: size, tank, speed, assim, voracity, search_area.
 *
 * 	This class also contains the generic methods in charge of the movement, growth and predation.
 */

class Animal: public Edible
{
protected:

	//AnimalSpecies* mySpecies;
	unsigned int huntingMode;
	Genome* genome;
	double eggDryMassAtBirth;
	double factorEggMassFromMom;
	unordered_map<Trait, double> fixedTraits;
	unordered_map<Trait, double> variableTraits;
	//double vonBertK;

	double Linf;
	vector<double> actualMoltingTimeVector;
	vector<double> actualMoltingMassVector;
	vector<double> finalDevTimeVector;
	vector<double> lengthsVector; //Dinosaurs ini
	vector<double> massesVector;
/* 	double* lengthsVector;
	double* massesVector;  //Dinosaurs end
 */	double currentBodySize;
	double currentPostTempGrowth;
	double currentPostTempLinf;
	double thisAnimalVonBertTime0;

	double maxPostTvor;
	double minPostTvor;
	double maxPostTsearch;
	double minPostTsearch;
	double maxPostTspeed;
	double minPostTspeed;

	double lastMassBeforeMaturationOrOviposition;
	double clutchDryMass;
	double massOfMaturationOrLastReproduction;
	double ageOfMaturation;
	double ageOfFirstMaturation;
	double ageForNextReproduction;
	double massForNextReproduction;
	double eggDryMassForClutch;
	double massAtBirth;
	int idFromMatedMale;
	int generationNumberFromMatedMale;
	bool mated = false;
	Genome* genomeFromMatedMale;
	double pseudoGrowth;
	TerrainCell* position;
	TerrainCell* targetNeighborToTravelTo;

	/**
	 * State variables
	 */
	double steps;
	unsigned int stepsAttempted;
	unsigned int lifeStage;

	list<int> encounterEvents;
	list<Edible*> ediblesHasTriedToPredate;
	int predatedByID;
	map<Species*, double> abundancesExperiencedPerSpecies;
	map<Species*, double> meanExperiencesPerSpecies;

	//int encounters_prey;
	int todayEncountersWithPredators;
	double daysWithoutFood;
	//int total_prey_encs;
	//int totalEncountersWithPredators;
	bool mature;
	bool sated;
	Instar instar;
	int pupaTimer;
	double handlingTimer;
	double timeStepMaximumHandlingTimer;
	double timeStepMaximumSearchArea;
	int diapauseTimeSteps;
	int ageOfLastMoultOrReproduction;
	double dateOfDeath;
	int lastDayMoved;

	double pheno_ini;
	int date_egg;

	double tank_ini;
	double speed_ini;
	double assim_ini;
	//double coeff_speed

	double voracity_ini;
	double search_area_ini;

	int generationNumberFromFemaleParent;
	int generationNumberFromMaleParent;
	int ID_prt1; //TODO puntero al padre mejor? o no? esto se usa?
	int ID_prt2;
	int rep_count;
	int fecundity;
	int age_first_rep;
	unsigned int gender;

	void initTraits();
	double getTotalMetabolicDryMassLoss(double wetMass, double temperature, double proportionOfTimeTheAnimalWasMoving, int timeStepsPerDay);

public:

	friend class TerrainCell;

	enum HUNTING_MODES
	{
		DOES_NOT_HUNT, SIT_AND_WAIT, // Sit and Wait
		ACTIVE_HUNTING
	// Active Hunting
	};

	enum LIFE_STAGES
	{
		UNBORN, ACTIVE, STARVED, PREDATED, REPRODUCING, PUPA, SATIATED, HANDLING, DIAPAUSE, BACKGROUND, SENESCED, SHOCKED
	};

	#ifdef _DEBUG
	list<int> searchedAnimalsToday;
	list<int> encounteredAnimalsToday;
	list<int> huntedAnimalsToday;
	list<int> predatedAnimalsToday;
	#endif

	/**
	 * Constructs an animal with the specified id and chromosomes,
	 * emplacing it at the specified position.It also initializes
	 * the array of traits based on the array of pairs of chromosomes.
	 * @param id the unique id of this Animal
	 * @param chromosomes an array of pairs of Chromosome, each pair
	 * containing the genetic material related to each trait
	 * @param position the Cell for this animal to be emplaced
	 */
	//Animal(int id, vector<pair<Chromosome*, Chromosome*> >* chromosomes, TerrainCell* position);
	Animal(Genome* genome, double eggMassAtBirth, TerrainCell* position, double timeStep, double timeStepsPerDay, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, Species* mySpecies, unsigned int gender,
			unsigned int huntingMode, bool temporary=false);
	/**
	 * Destroys this Animal deleting its data.
	 */
	virtual ~Animal();

	virtual double calculateDryMass() {	return currentBodySize + getTrait(Trait::energy_tank); };
	virtual double turnIntoDryMassToBeEaten(double predatorVoracity, float profitability, double leftovers) {	return calculateDryMass(); };
	virtual double calculateWetMass() {	return mySpecies->getConversionToWetMass() * calculateDryMass(); };
	virtual double getVoracity() { return getTrait(Trait::voracity); };
	virtual double getMassAtBirth() { return massAtBirth; };
	virtual double getRemainingVoracity() { return max(getTrait(Trait::voracity) - foodMass, 0.0); };
	virtual double getAssim() { return getTrait(Trait::assim); };
	virtual double getCurrentBodySize() { return currentBodySize; };
	virtual double getSpeed() { return getTrait(Trait::speed); };
	virtual int getHuntingMode() { return huntingMode; };
	inline virtual const Instar& getInstar() const { return instar; };
	virtual void incrementEncountersWithPredator(int predatorId);

	inline const double& getAgeInInstar(const Instar& instar) const { return finalDevTimeVector[instar.getValue()-1]; }
	inline const double& getAgeFirstReproduction() const { return getAgeInInstar(mySpecies->getInstarFirstReproduction()); }
	inline const double& getAgeLastInstar() const { return finalDevTimeVector.back(); }
	inline const double& getMassesInInstar(const Instar& instar) const { return massesVector[instar.getValue()-1]; }
	inline const double& getMassesFirstReproduction() const { return getMassesInInstar(mySpecies->getInstarFirstReproduction()); }
	inline const double& getMassesLastInstar() const { return massesVector.back(); }

	const double& getTrait(const Trait& trait) const;
	void setTrait(const Trait& trait, const double& newValue);

	virtual bool canEatEdible(Edible* edibleToCheck);
	virtual double calculateEncounterProbability(Edible* edibleToBeEncountered);
	virtual bool encounterEdible(Edible* searchedAnimal, float attackOrExposedAttackProbability, int timeStep, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile);
	virtual double calculatePredationProbability(Edible* edibleToBePredated, bool retaliation);
	virtual bool predateEdible(Edible* edibleToBePredated, int day, bool retaliation, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile);
	virtual double calculateEdibilityValue(Edible* edibleToBeEvaluated);
	virtual double calculateEdibilityValueWithMass(Edible* edibleToBeEvaluated);
	virtual float getMeanExperience(Species* species) { return meanExperiencesPerSpecies[species]; };

	virtual bool isSated() { return sated; };
	virtual bool isExhausted() { return steps >= getTrait(Trait::search_area); };

	virtual bool isMature() { return mature; };
	virtual bool isMated() { return mated; };
	virtual unsigned int getGender() { return gender; };

	virtual unsigned int getLifeStage() { return lifeStage; };
	virtual void setNewLifeStage(unsigned int newLifeStage);
	virtual void setNewLifeStage(unsigned int newLifeStage, double dayOfDeath);
	virtual void setNewLifeStage(unsigned int newLifeStage, double dayOfDeath, int predatorId);
	virtual double getDateOfDeath() { return dateOfDeath; };
	virtual void setDateOfDeath(double dateOfDeath) { this->dateOfDeath = dateOfDeath; };
	virtual int getPredatedByID() { return predatedByID; };
	virtual void setPredatedByID(int predatorId) { this->predatedByID = predatorId; };

	virtual pair<bool, bool> interpolateTraits();
	virtual void printTraits(ostream& traitsFile);
	virtual void adjustTraits();
	virtual void isReadyToBeBorn(int timeStep, int timeStepsPerDay);
	virtual void isReadyToResumeFromPupaOrDecreasePupaTimer();
	virtual void isReadyToResumeFromHandlingOrDecreaseHandlingTimer();
	virtual void isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(float relativeHumidity);
	virtual void calculateGrowthCurves(float temperature, ostream& tuneTraitsFile, bool printGrowthData, double ageAtInitialization);
	virtual double getEggDryMassAtBirth() { return eggDryMassAtBirth; };
	virtual void tuneTraits(int timeStep, int timeStepsPerDay, float temperature, float relativeHumidity, ostream& tuneTraitsFile, bool printGrowthData, bool fromForceMolting1);
	virtual TerrainCell* move(int timeStep, int timeStepsPerDay, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile);
	virtual double calculatePredatoryRiskEdibilityValue(Edible* edibleToBeEvaluated);
	virtual bool hasTriedToHunt(Edible* edibleToCheck);
	virtual void printVoracities(int timeStep, int timeStepsPerDay, ostream& voracitiesFile);
	virtual void dieFromBackground(int timeStep);
	virtual void assimilateFoodMass(int timeStep);
	virtual void metabolize(int timeStep, int timeStepPerDay);
	virtual void grow(int timeStep, int timeStepsPerDay);
	virtual list<Edible*> * breed(int timeStep, int timeStepsPerDay, float temperature);
	virtual bool postBreeding(int timeStep, int timeStepsPerDay);
	virtual const Genome* getGenome() const { return genome; };
	virtual int getGenerationNumberFromMaleParent() const { return generationNumberFromMaleParent; };
	virtual void setGenomeFromMatedMale(const Edible* matedMale);
	virtual list<int> * getEncounterEvents();

	virtual ostream& print(ostream& os);

	void clearGenomeFromMatedMale();
	double getSearchArea() { return getTrait(Trait::search_area); };

	virtual void increaseAge(int increase);

	/**
	 * Returns the chromosomes of this Animal.
	 * @return the array of pairs of Chromosome of this Animal
	 */
	vector<pair<Chromosome*, Chromosome*> >* getChromosomes();

	/**
	 * Returns true if the state of this Animal is ACTIVE, false otherwise.
	 * @return true if the state of this Animal is ACTIVE, false otherwise
	 */
	bool isActive();

	double getNormalizedHuntedVoracity(double huntedVoracity);
	double getNormalizedHunterVoracity();
	double getNormalizedVoracityProduct(double hunterVoracity);
	double getNormalizedHuntedBodySize(double huntedBodySize);
	double getNormalizedHunterBodySize();
	double getNormalizedPDF(double probabilityDensityFunction);
	double getNormalizedSpeedRatio(double hunterSpd);

	friend double getLog_mass_ratio(Animal* hunter, Animal* hunted);

	double getSearchAreaHunter();

	double getSpeedHunter();

	virtual inline TerrainCell* getPosition(){ return position; };

	//void incrementEncounters_prey();

	int computeEggBatchNumber();
	list<Edible*> * breedAsexually( int objectiveEggsNumber,int timeStep, int timeStepsPerDay, float temperature );
	list<Edible*> * breedSexually( int objectiveEggsNumber,int timeStep, int timeStepsPerDay, float temperature );

	void printGenetics(ostream& geneticsFile);

	double nextDinoMassPredicted;
	double currentAge;
	double daysExperienced;
	double tankAtGrowth;
	double slopeTarget;
	double interceptTarget;
	double foodMass;
	double finalJMaxVB;
	double maxNextInstarMassFromVBPlasticity;
	float edibleToBePredatedProfitability;
	double foodMassLeftForNextTimeStep;
	double lastHuntedAnimalDryMass;
	int days_digest;
	

	std::list<pair<TerrainCell *, Animal *>> eatenTodayList;
	std::list<TerrainCell *> cellsTrackedToday;
	double biomassTrackedToday;
	bool hasEvaluatedTheWholeWorld;
	int eatenToday;
	double wetMassAtTheBeginningOfTheTimeStep;
	double oldSearchArea;
	
	void updateLimits();
	void sumPseudoGrowthMean();
	void calculatePseudoGrowthMean();
	void sumPseudoGrowthSd();
	void calculatePseudoGrowthSd();


	float forceMolting(int timeStepsPerDay, double ageAtInitialization, Instar instarAtInitialization);
	float forceMolting2(int timeStepsPerDay, double ageAtInitialization, Instar instarAtInitialization);



	bool searchAnimalsAndFungiToEat(int timeStep, int timeStepsPerDay, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile);

	/**
	 * Moves one step from the current position. The decision is made
	 * considering both preys and predators in the neighbor cells.
	 */
	void moveOneStep();

	void setPosition(TerrainCell* newPosition) { this->position = newPosition; };
	void setDateEgg(double newDateEgg) { this->date_egg = newDateEgg; };
	void setMassAtBirth(double massAtBirth) { this->massAtBirth = massAtBirth; };

	void checkReadyToReproduce();





	void becomePredated(int timeStep);


	/**
	 * Returns the days digest.
	 * @return the days digest
	 */
	int getDaysDigest();
	int getMaxReproductionEvents();



	void updateAbundancesExperiencedPerSpecies(int timeStepsPerDay);

	void assimilateLastHuntedAnimalAndComputeHandlingTime();



};

#endif /* WEAVER_ANIMAL_H_ */
