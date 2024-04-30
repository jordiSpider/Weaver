/*
 * AnimalSpecies.h
 *
 *  Created on: Jul 6, 2013
 *      Author: jrbcast
 */

#ifndef WEAVER_ANIMALSPECIES_H_
#define WEAVER_ANIMALSPECIES_H_

#include <vector>
#include <iostream>
#include <math.h>
#include <float.h>
#include <random>
#include <cmath>
#include <list>

#include "ChromosomesGenerator.h"
#include "Species.h"
#include "FungusSpecies.h"
#include "PlantSpecies.h"
#include "Edible.h"
#include "Types.h"
#include "nlohmann/json.h"

using namespace std;

class ChromosomesGenerator;

class AnimalSpecies: public Species
{
private:
	bool extinguished;
	unsigned int defaultHuntingMode;

	std::vector<Species *> edibleAnimalSpecies;
	std::vector<Species *> ediblePlantSpecies;
	std::vector<Species *> edibleFungusSpecies;
	map<Species*, float> ediblePreferences;
	map<Species*, float> edibleProfitabilities;

	float experienceInfluencePerDay;

	std::vector<int> predationEventsOnOtherSpecies;

	unordered_map<Trait, double> fixedTraits;
	unordered_map<Trait, unsigned int> variableTraitsOrder;

	unordered_map<Trait, double> minTraitRanges;
	unordered_map<Trait, double> maxTraitRanges;

	unordered_map<Trait, double> minRestrictedRanges;
	unordered_map<Trait, double> maxRestrictedRanges;

	unordered_map<Trait, double> minTraitLimits;
	unordered_map<Trait, double> maxTraitLimits;

	unordered_map<Trait, double> minObservedPseudoValues;
	unordered_map<Trait, double> maxObservedPseudoValues;

	std::vector<double> initialPopulation;
	std::vector<unsigned long> statisticsInitialPopulation;

	unsigned int numberOfLociPerTrait;
	unsigned int numberOfAllelesPerLocus;
	unsigned int traitsPerModule;

	unsigned int numberOfChromosomes;
	unsigned int numberOfChiasmasPerChromosome;
	unsigned int numberOfLociPerChromosome;

	std::vector<double> restrictPerTrait;
	std::vector<double> rhoPerModule;
	std::vector<double> rhoRangePerModule;
	std::vector<double> weightPerLocus;

	double assignedForMolt;
	double betaScaleTank;
	double excessInvestInSize;
	unsigned int numberOfInstars;
	int pupaPeriodLength;
	double minRelativeHumidityThreshold;

	double maxEncountersT;

	//Constants for interactions
	double minSizeHunted;
	double maxSizeHunted;
	double meanSizeHunted;
	double sdSizeHunted;
	double minVorHunted;
	double maxVorHunted;
	double meanVorHunted;
	double sdVorHunted;
	double minSpeedHunted;
	double maxSpeedHunted;
	double meanSpeedHunted;
	double sdSpeedHunted;

	double minSizeHunter;
	double maxSizeHunter;
	double meanSizeHunter;
	double sdSizeHunter;
	double minVorHunter;
	double maxVorHunter;
	double meanVorHunter;
	double sdVorHunter;
	double minSpeedHunter;
	double maxSpeedHunter;
	double meanSpeedHunter;
	double sdSpeedHunter;
	double minSearchAreaHunter;
	double maxSearchAreaHunter;
	double meanSearchAreaHunter;
	double sdSearchAreaHunter;

    double minProbabilityDensityFunction;
	double maxProbabilityDensityFunction;
	double meanProbabilityDensityFunction;
	double sdProbabilityDensityFunction;
	double minVorXVor;
	double maxVorXVor;
	double meanVorXVor;
	double sdVorXVor;
	double minSpeedRatio;
	double maxSpeedRatio;
	double meanSpeedRatio;
	double sdSpeedRatio;
	
	double meanSizeRatio;
	double sdSizeRatio;

	double maxPredationProbability;
	double maxPredationIndividualWetMass;

	float cellEvaluationBiomass;
	float cellEvaluationRisk;
	float cellEvaluationProConspecific;
	float cellEvaluationAntiConspecific;
	float conspecificWeighing;

	//Added for new growth_curves
	vector<double> devTimeVector;
	double vonBertLinf;
	double vonBertKini;
	double vonBertTime0;
	double LinfKcorr;
	double devTimeConstant;
	double vonBertLdistanceMin;
	double vonBertLdistanceMax;
	double longevitySinceMaturation;
	double reproTimeFactor;
	double tempOptGrowth;
	double tempOptSearch;
	double tempOptVoracity;
	double tempOptSpeed;
	double EdGrowth;
	double EdSearch;
	double EdVoracity;
	double EdSpeed;
	double devInter;
	double fractSearchExtremeT;
	double fractSpeedExtremeT;
	double minVorExtremeT;
	double tempFromLab;
	double tempSizeRuleConstant;

	bool eggClutchFromEquation;
	double forClutchMassCoefficient;
	double forClutchMassScale;
	double forEggMassCoefficient;
	double forEggMassScale;
	bool eggMassFromEquation;
	double eggDryMass;
	double femaleWetMass;

	double scaleForVoracity;
	double scaleForSearchArea;
	double scaleForSpeed;
	double maxPlasticityKVonBertalanffy;
	double minPlasticityKVonBertalanffy;
	double maxPlasticityDueToConditionVor;
	double minPlasticityDueToConditionVor;
	double maxPlasticityDueToConditionSearch;
	double minPlasticityDueToConditionSearch;
	double maxPlasticityDueToConditionSpeed;
	double minPlasticityDueToConditionSpeed;

	float attackProbability;
	float exposedAttackProbability;
	float killProbability;
	float optimalTemperatureModifier;
	float daysWithoutFoodForMetabolicDownregulation;
	float percentageMetabolicDownregulation;
	float percentageCostForMetabolicDownregulationVoracity;
	float percentageCostForMetabolicDownregulationSearchArea;
	float percentageCostForMetabolicDownregulationSpeed;

	std::default_random_engine generatorX;
	std::default_random_engine generatorY;
	std::normal_distribution<double> normalDistributionX;
	std::normal_distribution<double> normalDistributionY;
	vector<vector<double> > cholMat;
	double pseudoGrowthMean;
	double pseudoGrowthSd;

	double coefficientForMassA;
	double scaleForMassB;

	double coefficientForMassAforMature;
	double scaleForMassBforMature;

	double forDensitiesA;
	double forDensitiesB;
	bool indeterminateGrowth;
	Instar instarFirstReproduction;
	unsigned int instarsForNextReproduction;
	int sexualType;
	int growthType;
	float sexRatio;
	int femaleMaxReproductionEvents;
	double eggsPerBatch;
	int maleMaxReproductionEvents;
	double maleReproductionFactor;
	double maleMobility;
	double quitCellThreshold;
	float decreaseOnTraitsDueToEncounters;
	double probabilityDeathFromBackground;
	
	void setFixedTraits(const unordered_map<string,nlohmann::json>& fixedTraits);
	void setTraitRanges(const unordered_map<string,double>& minTraitRanges, const unordered_map<string,double>& maxTraitRanges);
	void setMinTraitRanges(const unordered_map<string,double>& minTraitRanges);
	void setMaxTraitRanges(const unordered_map<string,double>& maxTraitRanges);
	void setTraitLimits(const unordered_map<string,double>& minTraitLimits, const unordered_map<string,double>& maxTraitLimits);
	void setMinTraitLimits(const unordered_map<string,double>& minTraitLimits);
	void setMaxTraitLimits(const unordered_map<string,double>& maxTraitLimits);

	inline void setForEggMassCoefficient(const double& forEggMassCoefficient) { this->forEggMassCoefficient = forEggMassCoefficient; }
	inline void setForEggMassScale(const double& forEggMassScale) { this->forEggMassScale = forEggMassScale; }
	void setEggMassFromEquation(const bool& eggMassFromEquation);
	inline void setEggDryMass(const double& eggDryMass) { this->eggDryMass = eggDryMass; }
	inline void setFemaleWetMass(const double& femaleWetMass) { this->femaleWetMass = femaleWetMass; }

public:
	enum GENDERS
	{
		MALE, FEMALE, HERMAPHRODITE
	};

	enum SEXUAL_TYPES
	{
		DIPLOID, HAPLODIPLOID, ASEXUAL
	};

	friend class Animal;
	friend class ChromosomesGenerator;
	ChromosomesGenerator * chromosomesGenerator;

	AnimalSpecies(const string& scientificName, const unsigned int& numberOfLoci, const unsigned int& numberOfAlleles, const std::vector<double>& restrictPerTrait, const std::vector<double>& rhoPerModule, const unsigned int& traitsPerModule, const unsigned int& numberOfChromosomes, const unsigned int& numberOfChiasmasPerChromosome, const unordered_map<string,nlohmann::json>& fixedTraits, const unordered_map<string,double>& minTraitRanges, 
							  const unordered_map<string,double>& maxTraitRanges, const unordered_map<string,double>& minTraitLimits, 
							  const unordered_map<string,double>& maxTraitLimits, const vector<string>& order);
	virtual ~AnimalSpecies();

	inline virtual size_t getNumberOfTraits() const { return restrictPerTrait.size(); };
	inline virtual size_t getNumberOfFixedTraits() const { return fixedTraits.size(); };
	inline virtual size_t getNumberOfVariableTraits() const { return minTraitRanges.size(); };
	virtual unsigned int getNumberOfLociPerTrait() { return numberOfLociPerTrait; };
	virtual unsigned int getTraitsPerModule() const { return traitsPerModule; };
	virtual unsigned int getNumberOfAllelesPerLocus() const { return numberOfAllelesPerLocus; };
	virtual unsigned int getNumberOfChiasmasPerChromosome() const { return numberOfChiasmasPerChromosome; };
	virtual unsigned int getNumberOfLociPerChromosome() const {	return numberOfLociPerChromosome; }
	virtual unsigned int getNumberOfChromosomes() const { return numberOfChromosomes; };
	virtual ChromosomesGenerator* getChromosomesGenerator() { return chromosomesGenerator; };
	virtual double getRhoRangePerModule(int moduleNumber) { return rhoRangePerModule[moduleNumber]; };
	virtual double getRhoPerModule(int moduleNumber) { return rhoPerModule[moduleNumber]; };
	virtual double getValueFromNormalDistributionY() { return normalDistributionY(generatorY); };
	virtual double getValueFromCholMat(int row, int column) { return cholMat[row][column]; };
	virtual double getVonBertLdistanceMin() { return vonBertLdistanceMin; };
	virtual double getVonBertLdistanceMax() { return vonBertLdistanceMax; };

	inline virtual const unordered_map<Trait, double>& getFixedTraits() const { return fixedTraits; };
	virtual vector<Trait> getVariableTraits() const;
	inline virtual unsigned int getTraitOrder(const Trait& trait) const { return variableTraitsOrder.at(trait); };

	inline virtual const double& getMinTraitRange(const Trait& trait) { return minTraitRanges[trait]; };
	inline virtual const double& getMaxTraitRange(const Trait& trait) { return maxTraitRanges[trait]; };
	inline virtual const double& getMinRestrictedRange(const Trait& trait) { return minRestrictedRanges[trait]; };
	inline virtual const double& getMaxRestrictedRange(const Trait& trait) { return maxRestrictedRanges[trait]; };
	inline virtual const double& getMinTraitLimit(const Trait& trait) { return minTraitLimits[trait]; };
	inline virtual const double& getMaxTraitLimit(const Trait& trait) { return maxTraitLimits[trait]; };
	inline virtual const double& getMinObservedPseudoValue(const Trait& trait) { return minObservedPseudoValues[trait]; };
	virtual void setMinObservedPseudoValue(const Trait& trait, const double& minObservedPseudoValue) { minObservedPseudoValues[trait] = minObservedPseudoValue; };
	inline virtual const double& getMaxObservedPseudoValue(const Trait& trait) { return maxObservedPseudoValues[trait]; };
	virtual void setMaxObservedPseudoValue(const Trait& trait, const double& maxObservedPseudoValue) { maxObservedPseudoValues[trait] = maxObservedPseudoValue; };


	virtual bool isMobile() { return true; };
	virtual void scaleInitialPopulation(double totalPopulationDensity, double initialEcosystemSize);
	inline virtual const vector<double>& getInitialPopulation() const { return initialPopulation; };
	virtual vector<unsigned long>* getStatisticsInitialPopulation() { return &statisticsInitialPopulation; };
	virtual double getTotalInitialPopulation();
	virtual unsigned long getTotalStatisticsInitialPopulation();
	virtual bool isExtinguished() { return extinguished; };
	virtual void setExtinguished(bool extinguished) { this->extinguished = extinguished; };
	virtual vector<int>* getPredationEventsOnOtherSpecies();
	virtual unsigned int getDefaultHuntingMode() { return defaultHuntingMode; };

	virtual double getExcessInvestInSize() { return excessInvestInSize; };
	inline virtual void setExcessInvestInSize(const double& excessInvestInSize) { this->excessInvestInSize = excessInvestInSize; }
	virtual bool hasIndeterminateGrowth() { return indeterminateGrowth; };
	inline virtual void setIndeterminateGrowth(const bool& indeterminateGrowth) { this->indeterminateGrowth = indeterminateGrowth; }
	inline virtual const Instar& getInstarFirstReproduction() const { return instarFirstReproduction; };
	inline virtual void setInstarFirstReproduction(const Instar& instarFirstReproduction) { this->instarFirstReproduction = instarFirstReproduction; }
	inline virtual const unsigned int& getInstarsForNextReproduction() const { return instarsForNextReproduction; };
	inline virtual void setInstarsForNextReproduction(const unsigned int& instarsForNextReproduction) { this->instarsForNextReproduction = instarsForNextReproduction; }
	inline virtual const unsigned int& getNumberOfInstars() const { return numberOfInstars; };
	inline virtual void setNumberOfInstars(const unsigned int& numberOfInstars) { this->numberOfInstars = numberOfInstars; };
	virtual int getPupaPeriodLength() { return pupaPeriodLength; };
	inline virtual void setPupaPeriodLength(const int& pupaPeriodLength) { this->pupaPeriodLength = pupaPeriodLength; }
	virtual double getCoefficientForMassA() {return coefficientForMassA;};
/*	virtual double getCoefficientForMassA(int pupaPeriodLength, bool mature) { //int pupaPeriodLength, bool mature
		if(pupaPeriodLength>2 && mature){
		  return coefficientForMassAforMature;
		}else{
		  return coefficientForMassA;
		}
	}*/
	inline virtual void setCoefficientForMassA(const double& coefficientForMassA) { this->coefficientForMassA = coefficientForMassA; }
	virtual double getScaleForMassB() { return scaleForMassB; };
	virtual double getEggDryMass() { return eggDryMass; };

/*	virtual double getScaleForMassB(int pupaPeriodLength, bool mature) {  //int pupaPeriodLength, bool mature
			if(pupaPeriodLength>2 && mature){
			  return scaleForMassBforMature;
			}else{
			  return scaleForMassB;
			}
		}*/

	inline virtual void setScaleForMassB(const double& scaleForMassB) { this->scaleForMassB = scaleForMassB; }

	virtual double getCoefficientForMassAforMature() { return coefficientForMassAforMature; };
	inline virtual void setCoefficientForMassAforMature(const double& coefficientForMassAforMature) { this->coefficientForMassAforMature = coefficientForMassAforMature; };

	virtual double getScaleForMassBforMature() { return scaleForMassBforMature; };
	inline virtual void setScaleForMassBforMature(const double& scaleForMassBforMature) { this->scaleForMassBforMature = scaleForMassBforMature; };

	virtual double getAssignedForMolt() { return assignedForMolt; };
	virtual bool hasEggClutchFromEquation() { return eggClutchFromEquation; };
	inline virtual void setEggClutchFromEquation(const bool& eggClutchFromEquation) { this->eggClutchFromEquation = eggClutchFromEquation; }
	virtual double getForClutchMassCoefficient() { return forClutchMassCoefficient; };
	inline virtual void setForClutchMassCoefficient(const double& forClutchMassCoefficient) { this->forClutchMassCoefficient = forClutchMassCoefficient; }
	virtual double getForClutchMassScale() { return forClutchMassScale; };
	inline virtual void setForClutchMassScale(const double& forClutchMassScale) { this->forClutchMassScale = forClutchMassScale; }
	virtual double getMaleReproductionFactor() { return maleReproductionFactor; };
	inline virtual void setMaleReproductionFactor(const double& maleReproductionFactor) { this->maleReproductionFactor = maleReproductionFactor; }
	virtual int getSexualType() { return sexualType; };
	virtual int getGrowthType() { return growthType; };
	virtual void setSexualType(int sexualType) { this->sexualType = sexualType;	};
	virtual void setGrowthType(int growthType) { this->growthType = growthType;	};
	virtual int getRandomGender();
	virtual double getMinRelativeHumidityThreshold() { return minRelativeHumidityThreshold; };
	virtual void setMinRelativeHumidityThreshold(const double& minRelativeHumidityThreshold);
	virtual float getDaysWithoutFoodForMetabolicDownregulation() { return daysWithoutFoodForMetabolicDownregulation; };
	inline virtual void setDaysWithoutFoodForMetabolicDownregulation(const float& daysWithoutFoodForMetabolicDownregulation) { this->daysWithoutFoodForMetabolicDownregulation = daysWithoutFoodForMetabolicDownregulation; }
	virtual float getPercentageMetabolicDownregulation() { return percentageMetabolicDownregulation; };
	inline virtual void setPercentageMetabolicDownregulation(const float& percentageMetabolicDownregulation) { this->percentageMetabolicDownregulation = percentageMetabolicDownregulation; }

	virtual bool canEatFungusSpecies(Species * species) { return std::find(edibleFungusSpecies.begin(), edibleFungusSpecies.end(), species) != edibleFungusSpecies.end(); };
	virtual bool canEatAnimalSpecies(Species* species) { return std::find(edibleAnimalSpecies.begin(), edibleAnimalSpecies.end(), species) != edibleAnimalSpecies.end(); };
	virtual bool canEatPlantSpecies(Species * newSpecies) {	return std::find(ediblePlantSpecies.begin(), ediblePlantSpecies.end(), newSpecies) != ediblePlantSpecies.end();	};
	virtual vector<Species*> * getEdibleFungusSpecies() {return &edibleFungusSpecies;};
	virtual void addEdibleFungusSpecies(Species* species);
	virtual vector<Species*> * getEdibleAnimalSpecies() {return &edibleAnimalSpecies;};
	virtual void addEdibleAnimalSpecies(Species* species);
	virtual vector<Species*> * getEdiblePlantSpecies() { return &ediblePlantSpecies; };
	virtual void addEdiblePlantSpecies(Species* species);
	virtual float getEdiblePreference(Species* species) { return ediblePreferences[species]; };
	virtual void addEdiblePreference(Species* speciesToBeAdded, float ediblePreferenceToBeAdded);
	virtual float getEdibleProfitability(Species* species) { return edibleProfitabilities[species]; };
	virtual void addEdibleProfitability(Species* speciesToBeAdded, float edibleProfitabilityToBeAdded);

	virtual float getAttackProbability() { return attackProbability; };
	virtual float getExposedAttackProbability() { return exposedAttackProbability; };
	virtual float getKillProbability() { return killProbability; };
	virtual float getAttackAvoidingProbability() { return Random::randomUniform(); };
	virtual float getKillAvoidingProbability() { return Random::randomUniform(); };

	virtual float getCellEvaluationBiomass() { return cellEvaluationBiomass; };
	virtual float getCellEvaluationRisk() { return cellEvaluationRisk; };
	virtual float getCellEvaluationProConspecific() { return cellEvaluationProConspecific; };
	virtual float getCellEvaluationAntiConspecific() { return cellEvaluationAntiConspecific; };
	virtual float getConspecificWeighing() { return conspecificWeighing; };

	virtual double getMaxPlasticityKVonBertalanffy() { return maxPlasticityKVonBertalanffy; };
	inline virtual void setMaxPlasticityKVonBertalanffy(const double& maxPlasticityKVonBertalanffy) { this->maxPlasticityKVonBertalanffy = maxPlasticityKVonBertalanffy; }
	virtual double getMinPlasticityKVonBertalanffy() { return minPlasticityKVonBertalanffy; };
	inline virtual void setMinPlasticityKVonBertalanffy(const double& minPlasticityKVonBertalanffy) { this->minPlasticityKVonBertalanffy = minPlasticityKVonBertalanffy; }
	virtual double getMaxPlasticityDueToConditionVor() { return maxPlasticityDueToConditionVor; };
	virtual double getMinPlasticityDueToConditionVor() { return minPlasticityDueToConditionVor; };
	virtual double getMaxPlasticityDueToConditionSearch() { return maxPlasticityDueToConditionSearch; };
	virtual double getMinPlasticityDueToConditionSearch() { return minPlasticityDueToConditionSearch; };
	virtual double getMaxPlasticityDueToConditionSpeed() { return maxPlasticityDueToConditionSpeed; };
	virtual double getMinPlasticityDueToConditionSpeed() { return minPlasticityDueToConditionSpeed; };

	virtual double getScaleForVoracity() { return scaleForVoracity; };
	inline virtual void setScaleForVoracity(const double& scaleForVoracity) { this->scaleForVoracity = scaleForVoracity; }
	virtual double getScaleForSearchArea() { return scaleForSearchArea; };
	inline virtual void setScaleForSearchArea(const double& scaleForSearchArea) { this->scaleForSearchArea = scaleForSearchArea; }
	virtual double getScaleForSpeed() { return scaleForSpeed; };
	inline virtual void setScaleForSpeed(const double& scaleForSpeed) { this->scaleForSpeed = scaleForSpeed; }
	virtual double getPseudoGrowthMean() { return pseudoGrowthMean; };
	virtual void setPseudoGrowthMean(double pseudoGrowthMean) { this->pseudoGrowthMean = pseudoGrowthMean; };
	virtual double getPseudoGrowthSd() { return pseudoGrowthSd; };
	virtual void setPseudoGrowthSd(double pseudoGrowthSd) { this->pseudoGrowthSd = pseudoGrowthSd; };
	virtual double getTempFromLab() { return tempFromLab; };
	virtual double getTempOptGrowth() { return tempOptGrowth; };
	virtual double getTempOptSearch() { return tempOptSearch; };
	virtual double getTempOptSpeed() { return tempOptSpeed; };
	virtual double getTempOptVoracity() { return tempOptVoracity; };
	virtual double getEdVoracity() { return EdVoracity; };
	virtual double getEdSearch() { return EdSearch; };
	virtual double getEdSpeed() { return EdSpeed; };
	virtual double getEdGrowth() { return EdGrowth; };
	virtual double getDevInter() { return devInter; };
	virtual double getFractSearchExtremeT() { return fractSearchExtremeT; };
	virtual double getFractSpeedExtremeT() { return fractSpeedExtremeT; };
	virtual double getMinVorExtremeT() { return minVorExtremeT; };
	virtual double getTempSizeRuleConstant() { return tempSizeRuleConstant; };
	inline virtual const double& getDevTime(Instar numberOfInstar) const { return devTimeVector[numberOfInstar.getValue()]; };
	virtual double getDevTimeConstant() { return devTimeConstant; };
	virtual double getVonBertTime0() { return vonBertTime0; };
	virtual double getBetaScaleTank() { return betaScaleTank; };
	virtual float getDecreaseOnTraitsDueToEncounters() { return decreaseOnTraitsDueToEncounters; };
	virtual double getMaleMobility() { return maleMobility; };
	virtual double getQuitCellThreshold() { return quitCellThreshold; };
	virtual double getLongevitySinceMaturation() { return longevitySinceMaturation; };
	virtual double getReproTimeFactor() { return reproTimeFactor; };
	virtual float getPercentageCostForMetabolicDownregulationVoracity() { return percentageCostForMetabolicDownregulationVoracity; };
	virtual float getPercentageCostForMetabolicDownregulationSearchArea() { return percentageCostForMetabolicDownregulationSearchArea; };
	virtual float getPercentageCostForMetabolicDownregulationSpeed() { return percentageCostForMetabolicDownregulationSpeed; };
	virtual float getOptimalTemperatureModifier() { return optimalTemperatureModifier; };
	virtual double getMaxEncountersT() { return maxEncountersT; };

 	virtual double getMinSizeHunted() { return minSizeHunted; };
	virtual double getMaxSizeHunted() { return maxSizeHunted; };
	virtual double getMeanSizeHunted() { return meanSizeHunted; };
	
	virtual void setMinSizeHunted(double minSizeHunted) {	this->minSizeHunted = minSizeHunted; };
	virtual void setMaxSizeHunted(double maxSizeHunted) {	this->maxSizeHunted = maxSizeHunted; };
	virtual void setMeanSizeHunted(double meanSizeHunted) {	this->meanSizeHunted = meanSizeHunted; };
	virtual double getSdSizeHunted() { return sdSizeHunted; };
	virtual void setSdSizeHunted(double sdSizeHunted) {	this->sdSizeHunted = sdSizeHunted; };
	
	virtual double getMinVorHunted() { return minVorHunted; };
	virtual double getMaxVorHunted() { return maxVorHunted; };
	virtual double getMeanVorHunted() { return meanVorHunted; };
	
	virtual void setMinVorHunted(double minVorHunted) {	this->minVorHunted = minVorHunted; };
	virtual void setMaxVorHunted(double maxVorHunted) {	this->maxVorHunted = maxVorHunted; };
	virtual void setMeanVorHunted(double meanVorHunted) { this->meanVorHunted = meanVorHunted; };
	virtual double getSdVorHunted() { return sdVorHunted; };
	virtual void setSdVorHunted(double sdVorHunted) {	this->sdVorHunted = sdVorHunted; };
	
	virtual double getMinSpeedHunted() { return minSpeedHunted; };
	virtual double getMaxSpeedHunted() { return maxSpeedHunted; };
	virtual double getMeanSpeedHunted() { return meanSpeedHunted; };
	
	virtual void setMinSpeedHunted(double minSpeedHunted) {	this->minSpeedHunted = minSpeedHunted; };
	virtual void setMaxSpeedHunted(double maxSpeedHunted) {	this->maxSpeedHunted = maxSpeedHunted; };
	virtual void setMeanSpeedHunted(double meanSpeedHunted) { this->meanSpeedHunted = meanSpeedHunted; };
	virtual double getSdSpeedHunted() { return sdSpeedHunted; };
	virtual void setSdSpeedHunted(double sdSpeedHunted) { this->sdSpeedHunted = sdSpeedHunted; };

	virtual double getMinSizeHunter() { return minSizeHunter; };
	virtual void setMinSizeHunter(double minSizeHunter) {	this->minSizeHunter = minSizeHunter; };
	virtual double getMaxSizeHunter() { return maxSizeHunter; };
	virtual void setMaxSizeHunter(double maxSizeHunter) {	this->maxSizeHunter = maxSizeHunter; };
	virtual double getMeanSizeHunter() { return meanSizeHunter; };
	virtual void setMeanSizeHunter(double meanSizeHunter) {	this->meanSizeHunter = meanSizeHunter; };
	virtual double getSdSizeHunter() { return sdSizeHunter; };
	virtual void setSdSizeHunter(double sdSizeHunter) {	this->sdSizeHunter = sdSizeHunter; };

	virtual double getMinVorHunter() { return minVorHunter; };
	virtual void setMinVorHunter(double minVorHunter) {	this->minVorHunter = minVorHunter; };
	virtual double getMaxVorHunter() { return maxVorHunter; };
	virtual void setMaxVorHunter(double maxVorHunter) {	this->maxVorHunter = maxVorHunter; };
	virtual double getMeanVorHunter() { return meanVorHunter; };
	virtual void setMeanVorHunter(double meanVorHunter) {	this->meanVorHunter = meanVorHunter; };
	virtual double getSdVorHunter() { return sdVorHunter; };
	virtual void setSdVorHunter(double sdVorHunter) {	this->sdVorHunter = sdVorHunter; };

	virtual double getMinSpeedHunter() { return minSpeedHunter; };
	virtual void setMinSpeedHunter(double minSpeedHunter) { this->minSpeedHunter = minSpeedHunter; };
	virtual double getMaxSpeedHunter() { return maxSpeedHunter; };
	virtual void setMaxSpeedHunter(double maxSpeedHunter) { this->maxSpeedHunter = maxSpeedHunter; };
	virtual double getMeanSpeedHunter() { return meanSpeedHunter; };
	virtual void setMeanSpeedHunter(double meanSpeedHunter) { this->meanSpeedHunter = meanSpeedHunter; };
	virtual double getSdSpeedHunter() { return sdSpeedHunter; };
	virtual void setSdSpeedHunter(double sdSpeedHunter) { this->sdSpeedHunter = sdSpeedHunter; };

	virtual double getMinSearchAreaHunter() { return minSearchAreaHunter; };
	virtual void setMinSearchAreaHunter(double minSearchAreaHunter) {	this->minSearchAreaHunter = minSearchAreaHunter; };
	virtual double getMaxSearchAreaHunter() { return maxSearchAreaHunter; };
	virtual void setMaxSearchAreaHunter(double maxSearchAreaHunter) {	this->maxSearchAreaHunter = maxSearchAreaHunter; };
	virtual double getMeanSearchAreaHunter() { return meanSearchAreaHunter; };
	virtual void setMeanSearchAreaHunter(double meanSearchAreaHunter) {	this->meanSearchAreaHunter = meanSearchAreaHunter; };
	virtual double getSdSearchAreaHunter() { return sdSearchAreaHunter; };
	virtual void setSdSearchAreaHunter(double sdSearchAreaHunter) {	this->sdSearchAreaHunter = sdSearchAreaHunter; };

	virtual double getMinProbabilityDensityFunction() { return minProbabilityDensityFunction; };
	virtual void setMinProbabilityDensityFunction(double minProbabilityDensityFunction) {	this->minProbabilityDensityFunction = minProbabilityDensityFunction; };
	virtual double getMaxProbabilityDensityFunction() { return maxProbabilityDensityFunction; };
	virtual void setMaxProbabilityDensityFunction(double maxProbabilityDensityFunction) {	this->maxProbabilityDensityFunction = maxProbabilityDensityFunction; };
	virtual double getMeanProbabilityDensityFunction() { return meanProbabilityDensityFunction; };
	virtual void setMeanProbabilityDensityFunction(double meanProbabilityDensityFunction) {	this->meanProbabilityDensityFunction = meanProbabilityDensityFunction; };
	virtual double getSdProbabilityDensityFunction() { return sdProbabilityDensityFunction; };
	virtual void setSdProbabilityDensityFunction(double sdProbabilityDensityFunction) {	this->sdProbabilityDensityFunction = sdProbabilityDensityFunction; };

	virtual double getMinVorXVor() { return minVorXVor; };
	virtual void setMinVorXVor(double minVorXVor) { this->minVorXVor = minVorXVor; };
	virtual double getMaxVorXVor() { return maxVorXVor; };
	virtual void setMaxVorXVor(double maxVorXVor) { this->maxVorXVor = maxVorXVor; };
	virtual double getMeanVorXVor() { return meanVorXVor; };
	virtual void setMeanVorXVor(double meanVorXVor) { this->meanVorXVor = meanVorXVor; };
	virtual double getSdVorXVor() { return sdVorXVor; };
	virtual void setSdVorXVor(double sdVorXVor) { this->sdVorXVor = sdVorXVor; };

	virtual double getMinSpeedRatio() { return minSpeedRatio; };
	virtual void setMinSpeedRatio(double minSpeedRatio) {	this->minSpeedRatio = minSpeedRatio; };
	virtual double getMaxSpeedRatio() { return maxSpeedRatio; };
	virtual void setMaxSpeedRatio(double maxSpeedRatio) {	this->maxSpeedRatio = maxSpeedRatio; };
	virtual double getMeanSpeedRatio() { return meanSpeedRatio; };
	virtual void setMeanSpeedRatio(double meanSpeedRatio) {	this->meanSpeedRatio = meanSpeedRatio; };
	virtual double getSdSpeedRatio() { return sdSpeedRatio; };
	virtual void setSdSpeedRatio(double sdSpeedRatio) {	this->sdSpeedRatio = sdSpeedRatio; };

	virtual double getMeanSizeRatio() { return meanSizeRatio; };
	virtual void setMeanSizeRatio(double meanSizeRatio) { this->meanSizeRatio = meanSizeRatio; };
	virtual double getSdSizeRatio() { return sdSizeRatio; };
	virtual void setSdSizeRatio(double sdSizeRatio) { this->sdSizeRatio = sdSizeRatio; };

	virtual float getExperienceInfluencePerDay() { return experienceInfluencePerDay; };
	virtual void addPredationEventOnOtherSpecies(int predatedSpeciesId) { predationEventsOnOtherSpecies.at(predatedSpeciesId)++; };

	virtual double getProbabilityDeathFromBackground() { return probabilityDeathFromBackground; };
	virtual double getFemaleMaxReproductionEvents() { return femaleMaxReproductionEvents; };
	virtual double getEggsPerBatch() { return eggsPerBatch; };
	virtual double getMaleMaxReproductionEvents() { return maleMaxReproductionEvents; };


	virtual void setInitialPredationEventsOnOtherSpecies(unsigned int numberOfSpecies);

	virtual void calculatePseudoGrowthMean();
	virtual void calculatePseudoGrowthSd();

	virtual void sumStatisticMeans(Edible* hunterAnimal, Edible* huntedAnimal, double muForPDF, double sigmaForPDF);
	virtual void interactionRanges(Edible* hunterAnimal, Edible* huntedAnimal, double muForPDF, double sigmaForPDF);
	virtual void computeStatisticMeans(unsigned int numberOfAttacks);
	virtual void sumStatisticSds(Edible* hunterAnimal, Edible* huntedAnimal, double muForPDF, double sigmaForPDF);
	virtual void computeStatisticSds(unsigned int numberOfAttacks);

	virtual void initWetBiomassDensitiesPerAge(float temperature, int timeStepsPerDay);
	virtual bool canEatAnyFungusSpecies() { return !edibleFungusSpecies.empty(); };
	virtual bool canEatAnyAnimalSpecies() { return !edibleAnimalSpecies.empty(); };
	virtual vector<Species*> * getInvolvedFungusSpecies();

	void setDefaultHuntingMode(unsigned int huntingMode);
	void setCorrelationCoeficientRHO(double newRho);
	void setQ10phenology(double q10pheno);
	void setQ10digestion(double q10digest);
	void setAssignedForMolt(const double& ASSIGNED);
	inline void setBetaScaleTank(const double& betaScaleTank) { this->betaScaleTank = betaScaleTank; }

	inline void setMaxEncountersT(const double& maxEncountersT) { this->maxEncountersT = maxEncountersT; }
	void setMaxSearchAreaT( double MAX_SEARCH_AREA_T);


	void calculateMinSizeHunted(double minSizeHunted);
	void calculateMaxSizeHunted(double maxSizeHunted);
	void sumMeanSizeHunted(double meanSizeHunted);
	void sumSdSizeHunted(double sdSizeHunted);
	void calculateMinVorHunted(double minVorHunted);
	void calculateMaxVorHunted(double maxVorHunted);
	void sumMeanVorHunted(double meanVorHunted);
	void sumSdVorHunted(double sdVorHunted);
	void calculateMinSpeedHunted(double minSpeedHunted);
	void calculateMaxSpeedHunted(double maxSpeedHunted);
	void sumMeanSpeedHunted(double meanSpeedHunted);
	void sumSdSpeedHunted(double sdSpeedHunted);
	void calculateMinSizeHunter(double minSizeHunter);
	void calculateMaxSizeHunter(double maxSizeHunter);	
	void sumMeanSizeHunter(double meanSizeHunter);
	void sumSdSizeHunter(double sdSizeHunter);
	
	
	void calculateMinVorHunter(double minVorHunter);
	void calculateMaxVorHunter(double maxVorHunter);
	void sumMeanVorHunter(double meanVorHunter);
	void sumSdVorHunter(double sdVorHunter);
	
	void calculateMinSpeedHunter(double minSpeedHunter);
	void calculateMaxSpeedHunter(double maxSpeedHunter);
	void sumMeanSpeedHunter(double meanSpeedHunter);
	void sumSdSpeedHunter(double sdSpeedHunter);
	
	void calculateMinSearchAreaHunter(double minSearchAreaHunter);
	void calculateMaxSearchAreaHunter(double maxSearchAreaHunter);
	void sumMeanSearchAreaHunter(double meanSearchAreaHunter);
	void sumSdSearchAreaHunter(double sdSearchAreaHunter);
	
	
	void calculateMinProbabilityDensityFunction(double minProbabilityDensityFunction);
	void calculateMaxProbabilityDensityFunction(double maxProbabilityDensityFunction);
	void sumMeanProbabilityDensityFunction(double meanProbabilityDensityFunction);
	void sumSdProbabilityDensityFunction(double sdProbabilityDensityFunction);
	
	
	void calculateMinVorXVor(double minVorXVor);
	void calculateMaxVorXVor(double maxVorXVor);
	void sumMeanVorXVor(double meanVorXVor);
	void sumSdVorXVor(double sdVorXVor);
	
	void calculateMinSpeedRatio(double minSpeedRatio);
	void calculateMaxSpeedRatio(double maxSpeedRatio);
	void sumMeanSpeedRatio(double meanSpeedRatio);
	void sumSdSpeedRatio(double sdSpeedRatio);
	
	void sumMeanSizeRatio(double meanSizeRatio);
	void sumSdSizeRatio(double sdSizeRatio);

	void calculateMeanSizeHunted(int population);
	void calculateSdSizeHunted(int population);
	void calculateMeanVorHunted(int population);
	void calculateSdVorHunted(int population);
	void calculateMeanSpeedHunted(int population);
	void calculateSdSpeedHunted(int population);
	void calculateMeanSizeHunter(int population);
	void calculateSdSizeHunter(int population);
	void calculateMeanVorHunter(int population);
	void calculateSdVorHunter(int population);
	void calculateMeanSpeedHunter(int population);
	void calculateSdSpeedHunter(int population);
	void calculateMeanSearchAreaHunter(int population);
	void calculateSdSearchAreaHunter(int population);
	void calculateMeanProbabilityDensityFunction(int population);
	void calculateSdProbabilityDensityFunction(int population);
	void calculateMeanVorXVor(int population);
	void calculateSdVorXVor(int population);
	void calculateMeanSpeedRatio(int population);
	void calculateSdSpeedRatio(int population);
	void calculateMeanSizeRatio(int population);
	void calculateSdSizeRatio(int population);

	//Added for new growth_curves
	inline void setDevTimeVector(const vector<double>& devTimeVector){ this->devTimeVector = devTimeVector; }
	void setVonBertLinf(double vonBertLinf);
	void setVonBertKini(double vonBertKini);
	inline void setVonBertTime0(const double& vonBertTime0) { this->vonBertTime0 = vonBertTime0; }
	void setLinfKcorr(const double& LinfKcorr);
	inline void setDevTimeConstant(const double& devTimeConstant) { this->devTimeConstant = devTimeConstant; }
	inline void setVonBertLdistanceMin(const double& vonBertLdistanceMin) { this->vonBertLdistanceMin = vonBertLdistanceMin; }
	inline void setVonBertLdistanceMax(const double& vonBertLdistanceMax) { this->vonBertLdistanceMax = vonBertLdistanceMax; }
	inline void setLongevitySinceMaturation(const double& longevitySinceMaturation) { this->longevitySinceMaturation = longevitySinceMaturation; }
	inline void setReproTimeFactor(const double& reproTimeFactor) { this->reproTimeFactor = reproTimeFactor; }
	inline void setTempOptGrowth(const double& tempOptGrowth) { this->tempOptGrowth = tempOptGrowth; }
	inline void setTempOptVoracity(const double& tempOptVoracity) { this->tempOptVoracity = tempOptVoracity; }
	inline void setTempOptSearch(const double& tempOptSearch) { this->tempOptSearch = tempOptSearch; }
	inline void setTempOptSpeed(const double& tempOptSpeed) { this->tempOptSpeed = tempOptSpeed; }
	inline void setEdGrowth(const double& EdGrowth) { this->EdGrowth = EdGrowth; }
	inline void setEdVoracity(const double& EdVoracity) { this->EdVoracity = EdVoracity; }
	inline void setEdSearch(const double& EdSearch) { this->EdSearch = EdSearch; }
	inline void setEdSpeed(const double& EdSpeed) { this->EdSpeed = EdSpeed; }
	inline void setDevInter(const double& devInter) { this->devInter = devInter; }
	inline void setFractSearchExtremeT(const double& fractSearchExtremeT) { this->fractSearchExtremeT = fractSearchExtremeT; }
	inline void setFractSpeedExtremeT(const double& fractSpeedExtremeT) { this->fractSpeedExtremeT = fractSpeedExtremeT; }
	inline void setMinVorExtremeT(const double& minVorExtremeT) { this->minVorExtremeT = minVorExtremeT; }
	inline void setTempFromLab(const double& tempFromLab) { this->tempFromLab = tempFromLab; }
	inline void setTempSizeRuleConstant(const double& tempSizeRuleConstant) { this->tempSizeRuleConstant = tempSizeRuleConstant; }

	void setMassInfo(const float& conversionToWetMass, const bool& eggClutchFromEquation, const double& forClutchMassCoefficient,
					 const double& forClutchMassScale, const double& forEggMassCoefficient, const double& forEggMassScale,
					 const double& eggDryMass, const double& femaleWetMass, const bool& eggMassFromEquation);




	void setMaxPlasticityDueToConditionVor(const double& maxPlasticityDueToConditionVor);
	void setMinPlasticityDueToConditionVor(const double& minPlasticityDueToConditionVor);
	void setMaxPlasticityDueToConditionSearch(const double& maxPlasticityDueToConditionSearch);
	void setMinPlasticityDueToConditionSearch(const double& minPlasticityDueToConditionSearch);
	void setMaxPlasticityDueToConditionSpeed(const double& maxPlasticityDueToConditionSpeed);
	void setMinPlasticityDueToConditionSpeed(const double& minPlasticityDueToConditionSpeed);

	float getCoefficientForNumberOfEvaluationsFromSearchArea();
	void setCoefficientForNumberOfEvaluationsFromSearchArea(float coefficientForNumberOfEvaluationsFromSearchArea);

	inline void setAttackProbability(const float& attackProbability) { this->attackProbability = attackProbability; }
	inline void setExposedAttackProbability(const float& exposedAttackProbability) { this->exposedAttackProbability = exposedAttackProbability; }
	inline void setKillProbability(const float& killProbability) { this->killProbability = killProbability; }
	inline void setOptimalTemperatureModifier(const float& optimalTemperatureModifier) { this->optimalTemperatureModifier = optimalTemperatureModifier; }


	inline void setPercentageCostForMetabolicDownregulationVoracity(const float& percentageCostForMetabolicDownregulationVoracity) { this->percentageCostForMetabolicDownregulationVoracity = percentageCostForMetabolicDownregulationVoracity; }
	inline void setPercentageCostForMetabolicDownregulationSearchArea(const float& percentageCostForMetabolicDownregulationSearchArea) { this->percentageCostForMetabolicDownregulationSearchArea = percentageCostForMetabolicDownregulationSearchArea; }
	inline void setPercentageCostForMetabolicDownregulationSpeed(const float& percentageCostForMetabolicDownregulationSpeed) { this->percentageCostForMetabolicDownregulationSpeed = percentageCostForMetabolicDownregulationSpeed; }


	inline void setForDensitiesA(const double& forDensitiesA) { this->forDensitiesA = forDensitiesA; }
	inline void setForDensitiesB(const double& forDensitiesB) { this->forDensitiesB = forDensitiesB; }
	void setEcosystemSize(double forDensitiesScale);




	float getSexRatio() { return sexRatio; };
	inline void setSexRatio(const float& sexRatio) { this->sexRatio = sexRatio; }

	inline void setFemaleMaxReproductionEvents(const int& femaleMaxReproductionEvents) { this->femaleMaxReproductionEvents = femaleMaxReproductionEvents; }
	void setEggsPerBatch(const double& eggsPerBatch) { this->eggsPerBatch = eggsPerBatch; }
	void setMaleMaxReproductionEvents(const int& maleMaxReproductionEvents) { this->maleMaxReproductionEvents = maleMaxReproductionEvents; }

	void setExperienceInfluencePerDay(const float& daysToRememberAbundancesExperienced);
	inline void setMaleMobility(const double& maleMobility) { this->maleMobility = maleMobility; }
	inline void setQuitCellThreshold(const double& quitCellThreshold) { this->quitCellThreshold = quitCellThreshold; }
	inline void setDecreaseOnTraitsDueToEncounters(const float& decreaseOnTraitsDueToEncounters) { this->decreaseOnTraitsDueToEncounters = decreaseOnTraitsDueToEncounters; }
	void setDryProportionOfMass(double CONTROL_DRY_BODY_MASS);
	inline void setProbabilityDeathFromBackground(const double& probabilityDeathFromBackground) { this->probabilityDeathFromBackground = probabilityDeathFromBackground; }









	inline void setInitialPopulation(const vector<double>& initialPopulation) { this->initialPopulation = initialPopulation; }
	inline void setStatisticsInitialPopulation(const vector<unsigned long>& statisticsInitialPopulation) { this->statisticsInitialPopulation = statisticsInitialPopulation; }


	const double& getTrait(const Trait& trait) const;
	void setVariableTraitsOrder(const vector<string>& order);
	void setTraits(const unordered_map<string,nlohmann::json>& fixedTraits, const unordered_map<string,double>& minTraitRanges, 
				   const unordered_map<string,double>& maxTraitRanges, const unordered_map<string,double>& minTraitLimits, 
				   const unordered_map<string,double>& maxTraitLimits, const vector<string>& order);
	void resetLimits();
	void resetPseudoGrowthMean();
	void resetPseudoGrowthSd();
	void computeRestrictedTraits();





	void fillInvolvedFungusSpecies(vector<Species*> * involvedFungusSpecies, vector<Species*> * alreadyCheckedAnimalSpecies, vector<Species*> * involvedAnimalSpecies);

	virtual double getMaxPredationProbability();
	void setMaxPredationProbability(double maxPredationProbability);

	virtual double getMaxPredationIndividualWetMass();
	void setMaxPredationIndividualWetMass(double maxPredationIndividualWetMass);

	inline void setCellEvaluationBiomass(const float& cellEvaluationBiomass) { this->cellEvaluationBiomass = cellEvaluationBiomass; }
	inline void setCellEvaluationRisk(const float& cellEvaluationRisk) { this->cellEvaluationRisk = cellEvaluationRisk; }
	inline void setCellEvaluationProConspecific(const float& cellEvaluationProConspecific) { this->cellEvaluationProConspecific = cellEvaluationProConspecific; }
	inline void setCellEvaluationAntiConspecific(const float& cellEvaluationAntiConspecific) { this->cellEvaluationAntiConspecific = cellEvaluationAntiConspecific; }
	inline void setConspecificWeighing(const float& conspecificWeighing) { this->conspecificWeighing = conspecificWeighing; }

	inline virtual const vector<double>& getDevTimeVector() const { return devTimeVector; }
};

#endif /* WEAVER_ANIMALSPECIES_H_ */
