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
#include <float.h>
#include <iostream>
#include <math.h>

#include "Genome.h"
#include "Gamete.h"
#include "Edible.h"
#include "SimulationConstants.h"
#include "Maths/Math_Functions.h"
#include "Maths/Random.h"
#include "Types.h"
#include "Trait.h"
#include "AnimalSpecies.h"
#include "SimulType.h"
#include "Output.h"
#include "WorldInterface.h"


class TerrainCellInterface;

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
public:

	static id_type animalCounter;

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
	explicit Animal(double eggMassAtBirth, TerrainCellInterface* position, double timeStep, double timeStepsPerDay, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, unsigned int gender,
			WorldInterface* worldInterface, bool temporary=false);
	explicit Animal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, double eggMassAtBirth, TerrainCellInterface* position, double timeStep, double timeStepsPerDay, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, unsigned int gender,
			WorldInterface* worldInterface, bool temporary=false);
	/**
	 * Destroys this Animal deleting its data.
	 */
	virtual ~Animal();

	inline AnimalSpecies* const getSpecies() const override { return static_cast<AnimalSpecies* const>(mySpecies); }
	inline const double calculateDryMass() const { return getCurrentBodySize() + getTrait(Trait::energy_tank); }
	void setDryMass(const double &newDryMass);
	void setDryMass(const double &newEnergyTank, const double &newCurrentBodySize);
	const double getInterpolatedDryMass() const; 
	inline const double turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const { return calculateDryMass(); }
	inline const double calculateWetMass() { return getSpecies()->convertToWetMass(calculateDryMass()); }
	inline const double getVoracity() const { return getTrait(Trait::voracity); }
	inline virtual const double& getMassAtBirth() const { return massAtBirth; }
	inline virtual const double getRemainingVoracity() const { return max(getTrait(Trait::voracity) - foodMass, 0.0); }
	inline virtual const double& getAssim() const { return getTrait(Trait::assim); }
	inline virtual const double getCurrentBodySize() const { return currentBodySize; }
	inline virtual const double getSpeed() const { return getTrait(Trait::speed); }
	inline const HuntingMode& getHuntingMode() const { return huntingMode; }
	inline virtual const bool isHunting() const { return getHuntingMode() != HuntingMode::does_not_hunt; }
	virtual void incrementEncountersWithPredator(const int &predatorId);

	void setExperiencePerSpecies();

	inline const double& getAgeInInstar(const Instar& instar) const { return finalDevTimeVector[instar.getValue()-1]; }
	inline const double& getAgeFirstReproduction() const { return getAgeInInstar(getSpecies()->getInstarFirstReproduction()); }
	inline const double& getAgeLastInstar() const { return finalDevTimeVector.back(); }
	inline const double& getMassesInInstar(const Instar& instar) const { return massesVector[instar.getValue()-1]; }
	inline const double& getMassesFirstReproduction() const { return getMassesInInstar(getSpecies()->getInstarFirstReproduction()); }
	inline const double& getMassesLastInstar() const { return massesVector.back(); }

	const double& getTrait(const TraitType& type) const;
	void setTrait(const TraitType& type, const double& newValue);

	virtual bool canEatEdible(Edible* edibleToCheck, const std::list<Edible*> &ediblesHasTriedToPredate);
	virtual double calculateEncounterProbability(Edible* edibleToBeEncountered, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	virtual bool encounterEdible(Edible* searchedAnimal, float attackOrExposedAttackProbability, int timeStep, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	virtual double calculatePredationProbability(Edible* edibleToBePredated, bool retaliation, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW);
	virtual bool predateEdible(Edible* edibleToBePredated, int day, bool retaliation, std::list<Edible*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea);
	virtual double calculateEdibilityValue(Edible* edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	virtual double calculateEdibilityValueWithMass(Edible* edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	virtual float getMeanExperience(Species* species) { return meanExperiencesPerSpecies.at(species); };

	virtual bool isDepleted(double foodDemand) { return false; };
	virtual double anyLeft() { return 0.0; };
	virtual void substractBiomass(double dryMassToBeSubstracted) {};

	virtual bool isSated() { return sated; };
	virtual bool isExhausted() { return steps >= getTrait(Trait::search_area); };

	virtual bool isMature() { return mature; };
	virtual bool isMated() { return mated; };
	virtual unsigned int getGender() { return gender; };

	inline virtual const LifeStage& getLifeStage() const { return lifeStage; };
	virtual void setNewLifeStage(const LifeStage newLifeStage);
	virtual void setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath);
	virtual void setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath, int predatorId);
	virtual double getDateOfDeath() const { return dateOfDeath; };
	virtual void setDateOfDeath(double dateOfDeath) { this->dateOfDeath = dateOfDeath; };
	virtual int getPredatedByID() const { return predatedByID; };
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
	virtual void tuneTraits(int timeStep, int timeStepsPerDay, float temperature, float relativeHumidity, ostream& tuneTraitsFile, bool printGrowthData, bool fromForceMolting1, SimulType simulType);
	virtual TerrainCellInterface* move(int timeStep, int timeStepsPerDay, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, ostream& edibilitiesFile, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, double pdfThreshold, double muForPDF, double sigmaForPDF, vector<AnimalSpecies*> *existingAnimalSpecies, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	virtual double calculatePredatoryRiskEdibilityValue(Edible* edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	virtual bool hasTriedToHunt(Animal* edibleToCheck, const list<Edible*> &ediblesHasTriedToPredate);
	virtual void printVoracities(int timeStep, int timeStepsPerDay, ostream& voracitiesFile, SimulType simulType);
	virtual void dieFromBackground(int timeStep, bool growthAndReproTest);
	virtual void assimilateFoodMass(int timeStep);
	virtual void metabolize(int timeStep, int timeStepPerDay, SimulType simulType);
	virtual void grow(int timeStep, int timeStepsPerDay);
	list<Animal*> * breed(int timeStep, int timeStepsPerDay, float temperature, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);
	virtual bool postBreeding(int timeStep, int timeStepsPerDay);
	virtual const Genome& getGenome() const { return genome; };
	virtual int getGenerationNumberFromMaleParent() const { return generationNumberFromMaleParent; };
	virtual void setGenomeFromMatedMale(const Animal* matedMale);
	virtual list<int> * getEncounterEvents();

	friend std::ostream& operator<<(std::ostream& os, const Animal& animal);

	void clearGenomeFromMatedMale();
	double getSearchArea() { return getTrait(Trait::search_area); };

	virtual void increaseAge(int increase);

	inline void doDefinitive() { 
		Edible::doDefinitive();
		Animal::animalCounter++;
	};

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

	double getInterpolatedHuntedVoracity(double huntedVoracity);
	double getInterpolatedHunterVoracity();
	double getInterpolatedVoracityProduct(double hunterVoracity);
	double getInterpolatedHuntedBodySize(double huntedBodySize);
	double getInterpolatedHunterBodySize();
	double getInterpolatedPDF(double probabilityDensityFunction);
	double getInterpolatedSpeedRatio(double hunterSpd);

	friend double getLog_mass_ratio(Animal* hunter, Animal* hunted);

	double getSearchAreaHunter();

	double getSpeedHunter();

	virtual inline TerrainCellInterface* getPosition(){ return position; };

	//void incrementEncounters_prey();

	int computeEggBatchNumber();
	list<Animal*> * breedAsexually( int objectiveEggsNumber,int timeStep, int timeStepsPerDay, float temperature, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);
	list<Animal*> * breedSexually( int objectiveEggsNumber,int timeStep, int timeStepsPerDay, float temperature, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);

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
	

	std::list<pair<TerrainCellInterface *, Animal *>> eatenTodayList;
	int eatenToday;
	double wetMassAtTheBeginningOfTheTimeStep;
	double oldSearchArea;
	
	void updateLimits();
	void sumPseudoGrowthMean();
	void calculatePseudoGrowthMean();
	void sumPseudoGrowthSd();
	void calculatePseudoGrowthSd();


	float forceMolting(int timeStepsPerDay, double ageAtInitialization, Instar instarAtInitialization, SimulType simulType);
	float forceMolting2(int timeStepsPerDay, double ageAtInitialization, Instar instarAtInitialization);



	bool searchInCurrentCell(int timeStep, int timeStepsPerDay, std::list<TerrainCellInterface *> &cellsTrackedToday, double &biomassTrackedToday, std::list<Edible*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, std::vector<AnimalSpecies*> *existingAnimalSpecies, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	bool searchAnimalToBreed(int timeStep, int timeStepsPerDay);

	bool searchAnimalsAndResourceToEat(int timeStep, int timeStepsPerDay, double &biomassTrackedToday, std::list<Edible*> &ediblesHasTriedToPredate, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, std::vector<AnimalSpecies*> *existingAnimalSpecies, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);

	TerrainCellInterface* searchTargetToTravelTo(bool &hasEvaluatedTheWholeWorld, std::list<TerrainCellInterface *> &cellsTrackedToday, const std::list<Edible*> &ediblesHasTriedToPredate, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);

	/**
	 * Moves one step from the current position. The decision is made
	 * considering both preys and predators in the neighbor cells.
	 */
	void moveOneStep(const TerrainCellInterface* const targetNeighborToTravelTo);

	void setPosition(TerrainCellInterface* newPosition) { this->position = newPosition; };
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



	void updateBiomassExperiencedPerSpecies(int timeStepsPerDay);

	void assimilateLastHuntedAnimalAndComputeHandlingTime();

protected:
	
	WorldInterface* worldInterface;

	HuntingMode huntingMode;
	Genome genome;
	double eggDryMassAtBirth;
	double factorEggMassFromMom;
	double traits[Trait::size()] {};
	const TraitType* const variableTraits;
	//double vonBertK;

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
	TerrainCellInterface* position;

	/**
	 * State variables
	 */
	double steps;
	
	// Number of attempts (steps) to perform the movement
	unsigned int stepsAttempted;
  
	LifeStage lifeStage;

	list<int> encounterEvents;
	int predatedByID;
	map<const Species* const, double> biomassExperiencedPerSpecies;
	map<const Species* const, double> meanExperiencesPerSpecies;

	//int encounters_prey;
	int todayEncountersWithPredators;
	double daysWithoutFood;
	//int total_prey_encs;
	//int totalEncountersWithPredators;
	bool mature;
	bool sated;
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

	double lengthAtBirth;

	CurveParams* growthCurveParams;

	double getValueGrowthCurve(const double &age, const double &midpointValue);

	void initTraits();
	double getTotalMetabolicDryMassLoss(double wetMass, double temperature, double proportionOfTimeTheAnimalWasMoving, int timeStepsPerDay, SimulType simulType);
	inline void deleteHomologousCorrelosomes() { genome.deleteHomologousCorrelosomes(); }
	void setOtherAttributes(double eggMassAtBirth, TerrainCellInterface* position, double timeStep, double timeStepsPerDay, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, unsigned int gender);
  const double& getFoodMassLeftForNextTimeStep() const;
	void setFoodMassLeftForNextTimeStep(const double &foodMassLeftForNextTimeStep);
	void decreaseFoodMassLeftForNextTimeStep(const double &foodToEliminate);
	bool isThereLeftoverFood() const;
};

#endif /* WEAVER_ANIMAL_H_ */
