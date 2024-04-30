/*
 * Animal.h
 *
 *  Created on: 20 Nov 2012
 *      Author: gabi
 */

#ifndef ANIMAL_H_
#define ANIMAL_H_

#include <cmath>
#include <vector>
#include <unordered_map>
#include <list>
#include <cfloat>
#include <boost/geometry.hpp>
#include <algorithm>
#include <iostream>
#include <boost/geometry/algorithms/distance.hpp>
#include <memory>
#include <tuple>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/LivingBeings/Animals/Genetics/Genome.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Gamete.h"
#include "IBM/World/LivingBeings/Edible.h"
#include "Misc/SimulationConstants.h"
#include "IBM/Maths/MathFunctions.h"
#include "IBM/Maths/Random.h"
#include "Misc/Types.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/Trait.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/LivingBeings/Resources/Resource.h"
#include "Exceptions/LineInfoException.h"
#include "IBM/World/Map/Points/PointContinuous.h"
#include "IBM/World/Map/Points/PointMap.h"
#include "IBM/World/LivingBeings/Animals/AnimalInterface.h"
#include "IBM/World/Map/Geometry/Geometry.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "Misc/CustomIndexedVector.h"



class TerrainCellInterface;


class WorldInterface;



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


class Animal: public AnimalInterface
{
private:
	friend class boost::serialization::access;

public:
	enum class Type : unsigned int
	{
		Default
	};


	#ifdef DEBUG
	std::list<int> searchedAnimalsToday;
	std::list<int> encounteredAnimalsToday;
	std::list<int> huntedAnimalsToday;
	std::list<int> predatedAnimalsToday;
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
	explicit Animal(TerrainCellInterface* const terrainCellInterface);
	explicit Animal(double newFactorEggMassFromMom, double timeStep, const Instar &instar, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender gender,
			bool temporary=false);
	explicit Animal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCellInterface* parentTerrainCellInterface, double newFactorEggMassFromMom, double timeStep, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender gender,
			bool temporary=false);
	/**
	 * Destroys this Animal deleting its data.
	 */
	virtual ~Animal();

	inline const double calculateDryMass() const { return getCurrentBodySize() + getTrait(Trait::Type::energy_tank); }
	void setDryMass(const double &newDryMass);
	void setDryMass(const double &newEnergyTank, const double &newCurrentBodySize);
	const double getInterpolatedDryMass(const unsigned int evaluationDepth, const double &dryMass) const; 
	inline const double turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const { return calculateDryMass(); }
	const double calculateWetMass() const;
	const double calculateWetMass(const double &dryMass) const;
	inline const double getVoracity() const { return getTrait(Trait::Type::voracity); }
	inline virtual const double& getMassAtBirth() const { return massAtBirth; }
	virtual const double getRemainingVoracity() const;
	virtual const double& getAssim() const;
	inline virtual const double getCurrentBodySize() const { return currentBodySize; }
	inline virtual const double getSpeed() const { return getTrait(Trait::Type::speed); }
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

	const double& getTrait(const Trait::Type& name) const;
	void setTrait(const Trait::Type& name, const double& newValue);

	virtual bool canEatEdible(const EdibleInterface* const &edibleToCheck, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, const double &dryMass) const;
	virtual double calculateEncounterProbability(const EdibleInterface &edibleToBeEncountered, const double &dryMass, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;
	virtual bool encounterEdible(EdibleInterface &searchedAnimal, const double &targetDryMass, float attackOrExposedAttackProbability, int timeStep, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;
	virtual double calculatePredationProbability(const EdibleInterface &edibleToBePredated, const double &targetDryMass, bool retaliation, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW) const;
	virtual bool predateEdible(EdibleInterface &edibleToBePredated, const double &targetDryMass, const Ring* const perceptionArea, int day, bool retaliation, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea);
	virtual double calculateEdibilityValue(const EdibleInterface &edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;
	virtual double calculateEdibilityValueWithMass(const EdibleInterface &edibleToBeEvaluated, const double &dryMass, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;
	virtual double getMeanExperience(const unsigned int speciesId) const;

	virtual bool isDepleted(double foodDemand, const double &dryMass) const { return false; };
	virtual double anyLeft(const double &dryMass) const { return 0.0; };
	virtual double substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea) { return 0.0; };

	virtual bool isSated() const;
	virtual bool isExhausted() const;

	virtual bool isMature() { return mature; };
	virtual bool isMated() { return mated; };
	virtual AnimalSpecies::Gender getGender() const { return gender; };

	virtual const LifeStage& getLifeStage() const;
	const double& getPhenoIni() const;
	const int& getDateEgg() const;
	const int& getAgeFirstRep() const;
	const int& getRepCount() const;
	const int& getFecundity() const;
	const int& getGenerationNumberFromFemaleParent() const;
	const int& getGenerationNumberFromMaleParent() const;
	const int& getID_prt1() const;
	const int& getID_prt2() const;
	const double& getVoracityIni() const;
	const double& getSearchAreaIni() const;
	const double& getSpeedAreaIni() const;
	const double& getTankIni() const;
	const int getTodayEncountersWithPredators() const;
	virtual void setNewLifeStage(const LifeStage newLifeStage);
	virtual void setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath);
	virtual void setNewLifeStage(const LifeStage newLifeStage, double dayOfDeath, int predatorId);
	virtual double getDateOfDeath() const { return dateOfDeath; };
	virtual void setDateOfDeath(double dateOfDeath) { this->dateOfDeath = dateOfDeath; };
	virtual int getPredatedByID() const { return predatedByID; };
	virtual void setPredatedByID(int predatorId) { this->predatedByID = predatorId; };

	const double& getCurrentAge() const;
	virtual std::pair<bool, bool> interpolateTraits();
	virtual void printTraits(std::ostream& traitsFile);
	virtual void adjustTraits();
	virtual void isReadyToBeBorn(int timeStep, int timeStepsPerDay);
	virtual void isReadyToResumeFromPupaOrDecreasePupaTimer();
	virtual void isReadyToResumeFromHandlingOrDecreaseHandlingTimer();
	virtual void isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(double relativeHumidity);
	virtual void calculateGrowthCurves(Temperature temperature, std::ostream& tuneTraitsFile, bool printGrowthData, double ageAtInitialization);
	const double& getEggDryMassAtBirth() const;
	virtual void tuneTraits(int timeStep, int timeStepsPerDay, Temperature temperature, float relativeHumidity, std::ostream& tuneTraitsFile, bool printGrowthData, bool fromForceMolting1);
	virtual void moveAnimal(int timeStep, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, std::ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	virtual double calculatePredatoryRiskEdibilityValue(const EdibleInterface &edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;
	virtual bool hasTriedToHunt(AnimalInterface* edibleToCheck, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate) const;
	virtual void printVoracities(int timeStep, std::ostream& voracitiesFile);
	virtual void dieFromBackground(int timeStep);
	virtual void assimilateFoodMass(int timeStep);
	virtual void metabolize(int timeStep);
	virtual void grow(int timeStep);
	std::unique_ptr<std::list<AnimalInterface*>> breed(int timeStep, Temperature temperature, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);
	virtual bool postBreeding(int timeStep);
	virtual const Genome& getGenome() const { return genome; };
	virtual void setGenomeFromMatedMale(const AnimalInterface* matedMale);
	virtual const std::list<int> * const getEncounterEvents() const;

	void clearGenomeFromMatedMale();
	double getSearchArea() { return getTrait(Trait::Type::search_area); };

	virtual void increaseAge(int increase);

	void doDefinitive();

	/**
	 * Returns the chromosomes of this Animal.
	 * @return the array of pairs of Chromosome of this Animal
	 */
	std::vector<std::pair<Chromosome*, Chromosome*> >* getChromosomes();

	/**
	 * Returns true if the state of this Animal is ACTIVE, false otherwise.
	 * @return true if the state of this Animal is ACTIVE, false otherwise
	 */
	bool isActive();

	double getInterpolatedHuntedVoracity(double huntedVoracity) const;
	double getInterpolatedHunterVoracity() const;
	double getInterpolatedVoracityProduct(double hunterVoracity) const;
	double getInterpolatedHuntedBodySize(double huntedBodySize) const;
	double getInterpolatedHunterBodySize() const;
	double getInterpolatedPDF(double probabilityDensityFunction) const;
	double getInterpolatedSpeedRatio(double hunterSpd) const;

	// friend double getLog_mass_ratio(Animal* hunter, Animal* hunted);

	double getSearchAreaHunter();

	const unsigned int& getCellDepthOnActualInstar() const;

	double getSpeedHunter();

	const double getCurrentLength() const;

	//void incrementEncounters_prey();

	int computeEggBatchNumber();
	std::unique_ptr<std::list<AnimalInterface*>> breedAsexually( int objectiveEggsNumber,int timeStep, Temperature temperature, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);
	std::unique_ptr<std::list<AnimalInterface*>> breedSexually( int objectiveEggsNumber,int timeStep, Temperature temperature, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);

	void printGenetics(std::ostream& geneticsFile);

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
	

	int eatenToday;
	double wetMassAtTheBeginningOfTheTimeStep;
	double oldSearchArea;
	
	void updateLimits();
	void sumPseudoGrowthMean();
	void calculatePseudoGrowthMean();
	void sumPseudoGrowthSd();
	void calculatePseudoGrowthSd();

	void updateDepth();

	Temperature forceMolting();
	Temperature forceMolting2();

	virtual void setInstar(const Instar& newInstar) override;

	void searchInCurrentCell(int timeStep, std::list<TerrainCellInterface*> &cellsTrackedToday, double &biomassTrackedToday, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, std::ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	bool searchAnimalToBreed(int timeStep);

	bool searchAnimalsAndResourceToEat(int timeStep, double &biomassTrackedToday, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, std::ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);

	void setDateEgg(double newDateEgg) { this->date_egg = newDateEgg; };
	void setMassAtBirth(double massAtBirth) { this->massAtBirth = massAtBirth; };

	void checkReadyToReproduce();

	const PointContinuous& getPosition() const;
	PointContinuous& getMutablePosition();
	void setPosition(const std::vector<double> &newPosition);
	void setPosition(const PointContinuous &newPosition);

	const bool isInHabitatShift() const;

	const bool isInBreedingZone() const;

	const bool isInHabitatShiftBeforeBreeding() const;
	const bool isInHabitatShiftAfterBreeding() const;

	const bool isAtDestination() const;
	const std::pair<PointMap, PointContinuous>& getTargetNeighborToTravelTo() const;

	void becomePredated(int timeStep);


	/**
	 * Returns the days digest.
	 * @return the days digest
	 */
	int getDaysDigest() const;
	int getMaxReproductionEvents();

	void updateBiomassExperiencedPerSpecies();

	void assimilateLastHuntedAnimalAndComputeHandlingTime();

	/**
     * @brief Serialize the Trait object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
	
	HuntingMode huntingMode;
	Genome genome;
	double eggDryMassAtBirth;
	double factorEggMassFromMom;
	CustomIndexedVector<Trait::Type, double> traits;
	const std::vector<Trait::Type>* variableTraits;
	//double vonBertK;

	std::vector<double> actualMoltingTimeVector;
	std::vector<double> actualMoltingMassVector;
	std::vector<double> finalDevTimeVector;
	std::vector<double> lengthsVector; //Dinosaurs ini
	std::vector<double> massesVector;
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

	PointContinuous position;

	bool inHabitatShift;

	bool inBreedingZone;

	bool inHabitatShiftBeforeBreeding;
	bool inHabitatShiftAfterBreeding;

	bool atDestination;
	std::pair<PointMap, PointContinuous> targetNeighborToTravelTo;

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

	/**
	 * State variables
	 */
	double steps;
	
	// Number of attempts (steps) to perform the movement
	unsigned int stepsAttempted;
  
	LifeStage lifeStage;

	std::list<int> encounterEvents;
	int predatedByID;
	std::vector<double> biomassExperiencedPerSpecies;
	std::vector<double> meanExperiencesPerSpecies;

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
	AnimalSpecies::Gender gender;

	CurveParams* growthCurveParams;

	double getValueGrowthCurve(const double &age, const double &midpointValue) const;

	void initTraits();
	double getTotalMetabolicDryMassLoss(double wetMass, double proportionOfTimeTheAnimalWasMoving);
	inline void deleteHomologousCorrelosomes() { genome.deleteHomologousCorrelosomes(); }
	void setOtherAttributes(double newFactorEggMassFromMom, double timeStep, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies::Gender gender);
	void setFactorEggMassFromMom(const double &newFactorEggMassFromMom);
	void setEggDryMassAtBirth(const double &newEggDryMassAtBirth);
	const double& getFactorEggMassFromMom() const;
	void calculateEggDryMassAtBirth();
	void calculateFactorEggMassFromMom();
  	const double& getFoodMassLeftForNextTimeStep() const;
	void setFoodMassLeftForNextTimeStep(const double &foodMassLeftForNextTimeStep);
	void decreaseFoodMassLeftForNextTimeStep(const double &foodToEliminate);
	bool isThereLeftoverFood() const;
	void moveOnNextInstar();
	void setCurrentAge(const double &newCurrentAge);

	void setEnergyTank(const double &newValue);

	void setInHabitatShift(const bool newInHabitatShiftValue);

	void setInBreedingZone(const bool newInBreedingZoneValue);

	void setInHabitatShiftBeforeBreeding(const bool newInHabitatShiftBeforeBreedingValue);
	void setInHabitatShiftAfterBreeding(const bool newInHabitatShiftAfterBreedingValue);

	void setAtDestination(const bool newAtDestinationValue);
	void setTargetNeighborToTravelTo(const std::pair<PointMap, PointContinuous> newTargetNeighborToTravelTo);

	bool checkBreed(int timeStep, AnimalInterface &otherAnimal);
	std::pair<bool, bool> checkAnimalInteraction(const AnimalInterface &otherAnimal, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;

	virtual void searchTargetToTravelTo(const double &perceptionArea, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)=0;
	virtual void moveOneStep()=0;
	virtual AnimalInterface* createOffspring(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCellInterface* parentTerrainCellInterface, double eggMassAtBirth, double timeStep, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender gender,
			bool temporary=false)=0;
};


std::ostream& operator<<(std::ostream& os, const Animal& animal);

#endif /* ANIMAL_H_ */
