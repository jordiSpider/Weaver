/*
 * Animal.h
 *
 *  Created on: 20 Nov 2012
 *      Author: gabi
 */

#ifndef ANIMAL_NON_STATISTICAL_H_
#define ANIMAL_NON_STATISTICAL_H_

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
#include <fstream>
#include <ostream>

#include "IBM/World/LivingBeings/Animals/Genetics/Gamete.h"
#include "IBM/World/LivingBeings/Edible.h"
#include "Misc/SimulationConstants.h"
#include "IBM/Maths/MathFunctions.h"
#include "IBM/Maths/Random.h"
#include "Misc/Types.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/Trait.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "IBM/World/LivingBeings/Resources/Resource.h"
#include "Exceptions/LineInfoException.h"
#include "IBM/World/Map/Points/PointMap.h"
#include "IBM/World/LivingBeings/Edible.h"
#include "IBM/World/Map/Geometry/Geometry.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "IBM/World/LivingBeings/Animals/AnimalNonStatisticalGrowth.h"
#include "Misc/EnumClass.h"



class TerrainCellInterface;


class World;



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


class AnimalNonStatistical: public Animal
{
private:
	friend class boost::serialization::access;

public:
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
	explicit AnimalNonStatistical(double actualTime, const Instar &instar, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface);
	explicit AnimalNonStatistical(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCellInterface* parentTerrainCellInterface, double newFactorEggMassFromMom, double actualTime, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender);
	/**
	 * Destroys this Animal deleting its data.
	 */
	virtual ~AnimalNonStatistical();

	const double getInterpolatedDryMass(const unsigned int evaluationDepth, const double &dryMass) const; 
	inline const double turnIntoDryMassToBeEaten(const double &predatorVoracity, const float &profitability, const double &leftovers) const { return calculateDryMass(); }
	virtual const double getRemainingVoracity() const;
	inline const HuntingMode& getHuntingMode() const { return huntingMode; }
	inline virtual const bool isHunting() const { return getHuntingMode() != HuntingMode::does_not_hunt; }
	virtual void incrementEncountersWithPredator(const int &predatorId);

	void setExperiencePerSpecies();

	const AnimalNonStatisticalGrowth* const getAnimalGrowth() const;
	AnimalNonStatisticalGrowth* const getMutableAnimalGrowth();

	void setInBreedingZone(const bool newInBreedingZoneValue);

	inline const double& getMassesFirstReproduction() const { return getAnimalGrowth()->getInstarMass(getSpecies()->getGrowthModule()->getInstarFirstReproduction()); }
	inline const double& getMassesLastInstar() const { return getAnimalGrowth()->getInstarMassesVector().back(); }
	

	virtual bool canEatEdible(const EdibleInterface* const &edibleToCheck, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, const double &dryMass) const;
	virtual double calculateEncounterProbability(const EdibleInterface &edibleToBeEncountered, const double &dryMass, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;
	virtual bool encounterEdible(EdibleInterface &searchedAnimal, const double &targetDryMass, float attackOrExposedAttackProbability, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;
	virtual double calculatePredationProbability(const EdibleInterface &edibleToBePredated, const double &targetDryMass, bool retaliation, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW) const;
	virtual bool predateEdible(EdibleInterface &edibleToBePredated, const double &targetDryMass, const Ring* const perceptionArea, const unsigned int numberOfTimeSteps, bool retaliation, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea);
	virtual double calculateEdibilityValue(const EdibleInterface &edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;
	virtual double calculateEdibilityValueWithMass(const EdibleInterface &edibleToBeEvaluated, const double &dryMass, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;
	virtual double getMeanExperience(const unsigned int speciesId) const;

	virtual bool isDepleted(double foodDemand, const double &dryMass) const { return false; };
	virtual double anyLeft(const double &dryMass) const { return 0.0; };
	virtual double substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea) { return 0.0; };

	unsigned int getUpdateLastTimeStep() const;
	void setUpdateLastTimeStep(const unsigned int newUpdateLastTimeStep);

	int getPredatedByID() const;

    void setPredatedByID(int predatorId);

	virtual bool isSated() const;
	virtual bool isExhausted() const;

	void doDefinitive();

	const int getAgeOfLastMoultOrReproduction() const;
	void setAgeOfLastMoultOrReproduction(const int newAgeOfLastMoultOrReproduction);

	virtual const bool isMated() const { return mated; };
	const double& getAgeOfFirstReproduction() const;
	const unsigned int& getReproCounter() const;
	const int& getFecundity() const;
	const int& getGenerationNumberFromFemaleParent() const;
	const int& getGenerationNumberFromMaleParent() const;
	const int& getID_prt1() const;
	const int& getID_prt2() const;
	const int getTodayEncountersWithPredators() const;
	virtual void setNewLifeStage(const LifeStage newLifeStage);
	virtual void setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps);
	virtual void setNewLifeStage(const LifeStage newLifeStage, const unsigned int numberOfTimeSteps, int predatorId);
	virtual double getDateOfDeath() const { return dateOfDeath; };
	virtual void setDateOfDeath(double dateOfDeath) { this->dateOfDeath = dateOfDeath; };

	virtual void printTraits(std::ostream& traitsFile);
	virtual void isReadyToBeBorn(const unsigned int numberOfTimeSteps);
	virtual void isReadyToResumeFromHandlingOrDecreaseHandlingTimer();
	virtual void tune(const Temperature& temperature, const unsigned int numberOfTimeSteps, const double& relativeHumidity, std::ostream& tuneTraitsFile, bool printGrowthData);
	virtual void moveAnimal(const unsigned int numberOfTimeSteps, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, std::ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	virtual double calculatePredatoryRiskEdibilityValue(const EdibleInterface &edibleToBeEvaluated, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;
	virtual bool hasTriedToHunt(Animal* edibleToCheck, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate) const;
	virtual void printVoracities(std::ostream& voracitiesFile);
	virtual void dieFromBackground(const unsigned int numberOfTimeSteps);
	virtual void assimilateFoodMass();
	virtual void metabolize();
	std::unique_ptr<std::list<AnimalNonStatistical*>> breed(const unsigned int numberOfTimeSteps, Temperature temperature, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);
	virtual bool postBreeding();
	virtual void setGenomeFromMatedMale(const AnimalNonStatistical* matedMale);
	virtual const std::list<int> * const getEncounterEvents() const;

	void clearGenomeFromMatedMale();

	virtual void increaseAge(int increase);

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

	//void incrementEncounters_prey();

	
	std::unique_ptr<std::list<AnimalNonStatistical*>> breedAsexually(int objectiveEggsNumber, const unsigned int numberOfTimeSteps, Temperature temperature, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);
	std::unique_ptr<std::list<AnimalNonStatistical*>> breedSexually(int objectiveEggsNumber, const unsigned int numberOfTimeSteps, Temperature temperature, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);

	void printGenetics(std::ostream& geneticsFile);

	double daysExperienced;
	double foodMass;
	double finalJMaxVB;
	double maxNextInstarMassFromVBPlasticity;
	float edibleToBePredatedProfitability;
	double foodMassLeftForNextTimeStep;
	double lastHuntedAnimalDryMass;
	int days_digest;


	unsigned int updateLastTimeStep;
	

	std::list<std::pair<TerrainCellInterface*, Animal*>> eatenTodayList;
	int eatenToday;
	double wetMassAtTheBeginningOfTheTimeStep;
	double oldSearchArea;

	void updateDepth();

	virtual void setInstar(const Instar& newInstar) override;

	const bool hasCapitalBreeding() const;
	void disableCapitalBreeding();

	void searchInCurrentCell(int timeStep, std::list<TerrainCellInterface*> &cellsTrackedToday, double &biomassTrackedToday, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, std::ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	bool searchAnimalToBreed(const unsigned int numberOfTimeSteps);

	bool searchAnimalsAndResourceToEat(const unsigned int numberOfTimeSteps, double &biomassTrackedToday, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, std::ostream& edibilitiesFile, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);

	void checkReadyToReproduce();

	double applyPlasticityDueToConditionToTrait(const double& traitValue, const double& plasticityDueToCondition, const double& h) const;

	void moveOnNextInstar();

	const bool isInHabitatShift() const;

	const bool isInBreedingZone() const;

	const bool isInHabitatShiftBeforeBreeding() const;
	const bool isInHabitatShiftAfterBreeding() const;

	const bool isAtDestination() const;
	const std::pair<PointMap, PointContinuous>& getTargetNeighborToTravelTo() const;

	void becomePredated(int timeStep);

	const double& getAgeForNextReproduction() const;

	/**
	 * Returns the days digest.
	 * @return the days digest
	 */
	int getDaysDigest() const;
	const unsigned int getMaxReproductionEvents() const;
	
	void setAgeForNextReproduction(const double& newAgeForNextReproduction);

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
	
	//double vonBertK;
	
	double currentPostTempGrowth;
	double currentPostTempLinf;

	double maxPostTvor;
	double minPostTvor;
	double maxPostTsearch;
	double minPostTsearch;
	double maxPostTspeed;
	double minPostTspeed;

	bool capitalBreeding;

	bool inHabitatShift;

	bool inBreedingZone;

	bool inHabitatShiftBeforeBreeding;
	bool inHabitatShiftAfterBreeding;

	bool atDestination;
	std::pair<PointMap, PointContinuous> targetNeighborToTravelTo;

	
	double ageForNextReproduction;
	int idFromMatedMale;
	int generationNumberFromMatedMale;
	bool mated = false;
	Genome* genomeFromMatedMale;

	double nextMassPredicted;


	/**
	 * State variables
	 */
	double steps;
	
	// Number of attempts (steps) to perform the movement
	unsigned int stepsAttempted;

	std::list<int> encounterEvents;
	int predatedByID;
	std::vector<double> biomassExperiencedPerSpecies;
	std::vector<double> meanExperiencesPerSpecies;

	//int encounters_prey;
	int todayEncountersWithPredators;
	double daysWithoutFood;
	//int total_prey_encs;
	//int totalEncountersWithPredators;
	bool sated;
	double handlingTimer;
	double timeStepMaximumHandlingTimer;
	double timeStepMaximumSearchArea;
	int ageOfLastMoultOrReproduction;
	double dateOfDeath;
	int lastDayMoved;

	

	
	double assim_ini;
	//double coeff_speed

	int generationNumberFromFemaleParent;
	int generationNumberFromMaleParent;
	int ID_prt1; //TODO puntero al padre mejor? o no? esto se usa?
	int ID_prt2;
	unsigned int reproCounter;
	int fecundity;
	double ageOfFirstReproduction;


	double getTotalMetabolicDryMassLoss(double wetMass, double proportionOfTimeTheAnimalWasMoving);
	void setOtherAttributes(int g_numb_prt1, int g_numb_prt2, int ID_prt1, int ID_prt2);
  	const double& getFoodMassLeftForNextTimeStep() const;
	void setFoodMassLeftForNextTimeStep(const double &foodMassLeftForNextTimeStep);
	void decreaseFoodMassLeftForNextTimeStep(const double &foodToEliminate);
	bool isThereLeftoverFood() const;

	void setInHabitatShift(const bool newInHabitatShiftValue);

	void setInHabitatShiftBeforeBreeding(const bool newInHabitatShiftBeforeBreedingValue);
	void setInHabitatShiftAfterBreeding(const bool newInHabitatShiftAfterBreedingValue);

	void setAtDestination(const bool newAtDestinationValue);
	void setTargetNeighborToTravelTo(const std::pair<PointMap, PointContinuous> newTargetNeighborToTravelTo);

	bool checkBreed(const unsigned int numberOfTimeSteps, AnimalNonStatistical &otherAnimal);
	std::pair<bool, bool> checkAnimalInteraction(const AnimalNonStatistical &otherAnimal, std::list<const EdibleInterface*> &ediblesHasTriedToPredate, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) const;

	virtual void searchTargetToTravelTo(const double &perceptionArea, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)=0;
	virtual void moveOneStep()=0;
	virtual AnimalNonStatistical* createOffspring(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCellInterface* parentTerrainCellInterface, double eggMassAtBirth, double actualTime, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender)=0;
};


std::ostream& operator<<(std::ostream& os, const AnimalNonStatistical& animal);

#endif /* ANIMAL_NON_STATISTICAL_H_ */
