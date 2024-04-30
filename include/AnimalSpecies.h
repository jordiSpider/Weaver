/*
 * AnimalSpecies.h
 *
 *  Created on: Jul 6, 2013
 *      Author: jrbcast
 */

#ifndef WEAVER_ANIMALSPECIES_H_
#define WEAVER_ANIMALSPECIES_H_

#include <vector>
#include <unordered_map>
#include <iostream>
#include <cfloat>
#include <random>
#include <cmath>
#include <list>

#include "Species.h"
#include "ResourceSpecies.h"
#include "Locus.h"
#include "Correlosome.h"
#include "Chromosome.h"
#include "Genome.h"
#include "Types.h"
#include "Trait.h"
#include <nlohmann/json.hpp>
#include "Curve.h"
#include "Output.h"
#include "OntogeneticLink.h"


using json = nlohmann::json;

using namespace std;

class ChromosomesGenerator;


class SexualType {
public:
	enum SexualTypeValue
	{
		diploid, 
		haplodiploid,
		asexual
	};

	explicit SexualType(const std::string& typeStr);
	~SexualType() {};

	inline const SexualTypeValue& getValue() const { return value; }
	inline operator int() const { return value; }
	friend inline bool operator==(const SexualType& lhs, const SexualTypeValue& rhs) { return lhs.getValue() == rhs; }
	friend inline bool operator==(const SexualTypeValue& lhs, const SexualType& rhs) { return lhs == rhs.getValue(); }

private:
	static const std::unordered_map<std::string_view, const SexualTypeValue> stringToEnum;
	static const std::string enumValues;

	inline static SexualTypeValue stringToEnumValue(const std::string &str) {
		try
		{
			return stringToEnum.at(str);
		}
		catch(const std::out_of_range& e) 
		{
			throwLineInfoException(fmt::format("Unknown sexual type '{}'. Valid values are {}", str, printAvailableValues()));
		}
	}
	inline static std::string_view printAvailableValues() { return enumValues; }
	inline static std::string_view to_string(const SexualTypeValue& sexualType) { return magic_enum::enum_name(sexualType); }
	static const std::unordered_map<std::string_view, const SexualTypeValue> generateMap();
	inline static constexpr size_t size() { return magic_enum::enum_count<SexualTypeValue>(); }
	static const std::string generateAvailableValues();

	const SexualTypeValue value;
};

class HuntingMode {
public:
	enum HuntingModeValue
	{
		does_not_hunt, 
		sit_and_wait,
		active_hunting
	};

	explicit HuntingMode(const std::string& typeStr);
	~HuntingMode() {};

	inline const HuntingModeValue& getValue() const { return value; }
	inline operator int() const { return value; }
	friend inline bool operator!=(const HuntingMode& lhs, const HuntingModeValue& rhs) { return lhs.getValue() != rhs; }
	friend inline bool operator!=(const HuntingModeValue& lhs, const HuntingMode& rhs) { return lhs != rhs.getValue(); }
	friend inline bool operator==(const HuntingMode& lhs, const HuntingModeValue& rhs) { return lhs.getValue() == rhs; }
	friend inline bool operator==(const HuntingModeValue& lhs, const HuntingMode& rhs) { return lhs == rhs.getValue(); }

private:
	static const std::unordered_map<std::string_view, const HuntingModeValue> stringToEnum;
	static const std::string enumValues;

	inline static HuntingModeValue stringToEnumValue(const std::string &str) {
		try
		{
			return stringToEnum.at(str);
		}
		catch(const std::out_of_range& e) 
		{
			throwLineInfoException(fmt::format("Unknown sexual type '{}'. Valid values are {}", str, printAvailableValues()));
		}
	}
	inline static std::string_view printAvailableValues() { return enumValues; }
	inline static std::string_view to_string(const HuntingModeValue& huntingMode) { return magic_enum::enum_name(huntingMode); }
	static const std::unordered_map<std::string_view, const HuntingModeValue> generateMap();
	inline static constexpr size_t size() { return magic_enum::enum_count<HuntingModeValue>(); }
	static const std::string generateAvailableValues();

	const HuntingModeValue value;
};

class AnimalSpecies: public Species
{
private:
	static id_type animalSpeciesCounter;

	const id_type animalSpeciesId;
	bool extinguished;
	const HuntingMode defaultHuntingMode;

	std::vector<AnimalSpecies *> edibleAnimalSpecies;
	std::vector<ResourceSpecies *> edibleResourceSpecies;
	std::vector<std::vector<std::vector<OntogeneticLink>>> edibleOntogeneticLink;

	float experienceInfluencePerDay;

	std::vector<int> predationEventsOnOtherSpecies;

	Trait* traits[Trait::size()];
	TraitType* fixedTraits;
	size_t numberFixedTraits;
	TraitType* variableTraits;
	size_t numberVariableTraits;



	std::vector<double> initialPopulation;
	std::vector<unsigned long> statisticsInitialPopulation;

	unsigned int numberOfLociPerTrait;
	unsigned int numberOfAllelesPerLocus;
	unsigned int traitsPerModule;

	unsigned int numberOfChromosomes;
	unsigned int numberOfChiasmasPerChromosome;
	unsigned int numberOfLociPerChromosome;

	std::vector<double> rhoPerModule;
	std::vector<double> rhoRangePerModule;
	std::vector<double> weightPerLocus;

	double assignedForMolt;
	double betaScaleTank;
	double excessInvestInSize;
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
	double LinfKcorr;
	double devTimeConstant;
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

	double maximumDryMassObserved;

	double coefficientForMassAforMature;
	double scaleForMassBforMature;

	double forDensitiesA;
	double forDensitiesB;
	bool indeterminateGrowth;
	Instar instarFirstReproduction;
	unsigned int instarsForNextReproduction;
	const SexualType sexualType;
	Curve* growthCurve;
	float sexRatio;
	unsigned int size;
	int femaleMaxReproductionEvents;
	double eggsPerBatch;
	int maleMaxReproductionEvents;
	double maleReproductionFactor;
	double maleMobility;
	bool forceQuitCell;
	bool surviveWithoutFood;
	float decreaseOnTraitsDueToEncounters;
	double probabilityDeathFromBackground;

	bool preserveLeftovers;

	vector<Locus*> loci;
	//TODO This has room for efficiency improvements. We could just save the final calculations of the actual positions.
	vector<int> randomlyCreatedPositionsForChromosomes;
	
	void setFixedTraits(const std::unordered_map<std::string,double>& fixedTraits);
	void setVariableTraits(const unordered_map<string,double>& minTraitRanges, const unordered_map<string,double>& maxTraitRanges, 
						   const unordered_map<string,double>& minTraitLimits, const unordered_map<string,double>& maxTraitLimits, 
						   const vector<string>& order, const std::vector<double>& restrictPerTrait);

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

	static const id_type& getAnimalSpeciesCounter();


	AnimalSpecies(const json &info, bool initIndividualsPerDensities);
	virtual ~AnimalSpecies();

	bool getPreserveLeftovers() const { return preserveLeftovers; }

	const id_type& getAnimalSpeciesId() const;

	void initEdibleOntogeneticLink();

	const OntogeneticLink& getEdibleOntogeneticLink(const id_type &preySpeciesId, const Instar &predator, const Instar &prey) const;

	void setEdibleOntogeneticLink(const id_type &preySpeciesId, const Instar &predator, const Instar &prey, const json &linkInfo);

	void addEdibleOntogeneticLink(Species* newSpecies, const json& ontogeneticLink);

	virtual void addEdibleResourceSpecies(ResourceSpecies* newSpecies, const nlohmann::json& ontogeneticLink);
	virtual void addEdibleAnimalSpecies(AnimalSpecies* newSpecies, const nlohmann::json& ontogeneticLink);

	double getEdiblePreference(const id_type &preySpeciesId, const Instar &predator);
	const double& getEdiblePreference(const id_type &preySpeciesId, const Instar &predator, const Instar &prey);
	const double& getEdibleProfitability(const id_type &preySpeciesId, const Instar &predator, const Instar &prey);

	bool canEatEdible(const id_type &preySpeciesId, const Instar &predator, const Instar &prey);

	const double& getMaximumDryMassObserved() const;
	void calculateK_Value();
	void updateMaximumDryMassObserved(double newMaximumDryMass);
  	virtual size_t getNumberOfFixedTraits() const { return numberFixedTraits; }
	virtual size_t getNumberOfVariableTraits() const { return numberVariableTraits; }
	virtual unsigned int getNumberOfLociPerTrait() const { return numberOfLociPerTrait; };
	virtual unsigned int getTraitsPerModule() const { return traitsPerModule; };
	virtual unsigned int getNumberOfAllelesPerLocus() const { return numberOfAllelesPerLocus; };
	virtual unsigned int getNumberOfChiasmasPerChromosome() const { return numberOfChiasmasPerChromosome; };
	virtual unsigned int getNumberOfLociPerChromosome() const {	return numberOfLociPerChromosome; }
	virtual unsigned int getNumberOfChromosomes() const { return numberOfChromosomes; };
	virtual double getRhoRangePerModule(int moduleNumber) const { return rhoRangePerModule.at(moduleNumber); };
	virtual double getRhoPerModule(int moduleNumber) const { return rhoPerModule.at(moduleNumber); };
	inline virtual const double getValueFromNormalDistributionY() { return normalDistributionY(generatorY); };
	virtual double getValueFromCholMat(int row, int column) const { return cholMat.at(row).at(column); };
	inline const vector<Locus*>& getLoci() const { return loci; }
	inline const vector<int>& getRandomlyCreatedPositionsForChromosomes() const { return randomlyCreatedPositionsForChromosomes; }

	void initializeFixedTraits(double* const animalTraits) const;
	inline const VariableTrait* const getVariableTrait(const TraitType& type) const { return static_cast<const VariableTrait* const>(traits[type]); }
	virtual const TraitType* const getTypeVariableTraits() const;
	inline virtual const unsigned int& getTraitOrder(const TraitType& type) const { return getVariableTrait(type)->getOrder(); }

	inline virtual const double& getMinTraitRange(const TraitType& type) const { return getVariableTrait(type)->getMinTraitRange(); };
	inline virtual const double& getMaxTraitRange(const TraitType& type) const { return getVariableTrait(type)->getMaxTraitRange(); };
	inline virtual const double& getMinRestrictedRange(const TraitType& type) const { return getVariableTrait(type)->getMinRestrictedRange(); };
	inline virtual const double& getMaxRestrictedRange(const TraitType& type) const { return getVariableTrait(type)->getMaxRestrictedRange(); };
	inline virtual const double& getMinTraitLimit(const TraitType& type) const { return getVariableTrait(type)->getMinTraitLimit(); };
	inline virtual const double& getMaxTraitLimit(const TraitType& type) const { return getVariableTrait(type)->getMaxTraitLimit(); };
	inline virtual const double& getMinObservedPseudoValue(const TraitType& type) const { return getVariableTrait(type)->getMinObservedPseudoValue(); };
	virtual void setMinObservedPseudoValue(const TraitType& type, const double& minObservedPseudoValue) { static_cast<VariableTrait*>(traits[type])->setMinObservedPseudoValue(minObservedPseudoValue); }
	inline virtual const double& getMaxObservedPseudoValue(const TraitType& type) const { return getVariableTrait(type)->getMaxObservedPseudoValue(); };
	virtual void setMaxObservedPseudoValue(const TraitType& type, const double& maxObservedPseudoValue) { static_cast<VariableTrait*>(traits[type])->setMaxObservedPseudoValue(maxObservedPseudoValue); }


	virtual bool isMobile() const { return true; };
	virtual void scaleInitialPopulation(double totalPopulationDensity, double initialEcosystemSize);
	inline virtual const vector<double>& getInitialPopulation() const { return initialPopulation; };
	virtual vector<unsigned long>* getStatisticsInitialPopulation() { return &statisticsInitialPopulation; };
	virtual double getTotalInitialPopulation();
	virtual unsigned long getTotalStatisticsInitialPopulation();
	virtual bool isExtinguished() const { return extinguished; };
	virtual void setExtinguished(bool extinguished) { this->extinguished = extinguished; };
	virtual vector<int>* getPredationEventsOnOtherSpecies();
	virtual const HuntingMode& getDefaultHuntingMode() const { return defaultHuntingMode; };

	virtual double getExcessInvestInSize() const { return excessInvestInSize; };
	inline virtual void setExcessInvestInSize(const double& excessInvestInSize) { this->excessInvestInSize = excessInvestInSize; }
	virtual bool hasIndeterminateGrowth() const { return indeterminateGrowth; };
	inline virtual void setIndeterminateGrowth(const bool& indeterminateGrowth) { this->indeterminateGrowth = indeterminateGrowth; }
	inline virtual const Instar& getInstarFirstReproduction() const { return instarFirstReproduction; };
	inline virtual void setInstarFirstReproduction(const Instar& instarFirstReproduction) { this->instarFirstReproduction = instarFirstReproduction; }
	inline virtual const unsigned int& getInstarsForNextReproduction() const { return instarsForNextReproduction; };
	inline virtual void setInstarsForNextReproduction(const unsigned int& instarsForNextReproduction) { this->instarsForNextReproduction = instarsForNextReproduction; }
	virtual int getPupaPeriodLength() const { return pupaPeriodLength; };
	inline virtual void setPupaPeriodLength(const int& pupaPeriodLength) { this->pupaPeriodLength = pupaPeriodLength; }
	virtual double getCoefficientForMassA() const {return coefficientForMassA;};
/*	virtual double getCoefficientForMassA(int pupaPeriodLength, bool mature) { //int pupaPeriodLength, bool mature
		if(pupaPeriodLength>2 && mature){
		  return coefficientForMassAforMature;
		}else{
		  return coefficientForMassA;
		}
	}*/
	inline virtual void setCoefficientForMassA(const double& coefficientForMassA) { this->coefficientForMassA = coefficientForMassA; }
	virtual double getScaleForMassB() const { return scaleForMassB; };
	virtual double getEggDryMass() const { return eggDryMass; };

/*	virtual double getScaleForMassB(int pupaPeriodLength, bool mature) {  //int pupaPeriodLength, bool mature
			if(pupaPeriodLength>2 && mature){
			  return scaleForMassBforMature;
			}else{
			  return scaleForMassB;
			}
		}*/

	inline virtual void setScaleForMassB(const double& scaleForMassB) { this->scaleForMassB = scaleForMassB; }

	virtual double getCoefficientForMassAforMature() const { return coefficientForMassAforMature; };
	inline virtual void setCoefficientForMassAforMature(const double& coefficientForMassAforMature) { this->coefficientForMassAforMature = coefficientForMassAforMature; };

	virtual double getScaleForMassBforMature() const { return scaleForMassBforMature; };
	inline virtual void setScaleForMassBforMature(const double& scaleForMassBforMature) { this->scaleForMassBforMature = scaleForMassBforMature; };

	virtual double getAssignedForMolt() const { return assignedForMolt; };
	virtual bool hasEggClutchFromEquation() const { return eggClutchFromEquation; };
	inline virtual void setEggClutchFromEquation(const bool& eggClutchFromEquation) { this->eggClutchFromEquation = eggClutchFromEquation; }
	virtual double getForClutchMassCoefficient() const { return forClutchMassCoefficient; };
	inline virtual void setForClutchMassCoefficient(const double& forClutchMassCoefficient) { this->forClutchMassCoefficient = forClutchMassCoefficient; }
	virtual double getForClutchMassScale() const { return forClutchMassScale; };
	inline virtual void setForClutchMassScale(const double& forClutchMassScale) { this->forClutchMassScale = forClutchMassScale; }
	virtual double getMaleReproductionFactor() const { return maleReproductionFactor; };
	inline virtual void setMaleReproductionFactor(const double& maleReproductionFactor) { this->maleReproductionFactor = maleReproductionFactor; }
	virtual const SexualType& getSexualType() const { return sexualType; };
	virtual const Curve* const getGrowthCurve() const { return growthCurve; }
	virtual int getRandomGender() const;
	virtual double getMinRelativeHumidityThreshold() const { return minRelativeHumidityThreshold; };
	virtual void setMinRelativeHumidityThreshold(const double& minRelativeHumidityThreshold);
	virtual float getDaysWithoutFoodForMetabolicDownregulation() const { return daysWithoutFoodForMetabolicDownregulation; };
	inline virtual void setDaysWithoutFoodForMetabolicDownregulation(const float& daysWithoutFoodForMetabolicDownregulation) { this->daysWithoutFoodForMetabolicDownregulation = daysWithoutFoodForMetabolicDownregulation; }
	virtual float getPercentageMetabolicDownregulation() const { return percentageMetabolicDownregulation; };
	inline virtual void setPercentageMetabolicDownregulation(const float& percentageMetabolicDownregulation) { this->percentageMetabolicDownregulation = percentageMetabolicDownregulation; }

	virtual bool canEatResourceSpecies(ResourceSpecies * species) const { return std::find(edibleResourceSpecies.begin(), edibleResourceSpecies.end(), species) != edibleResourceSpecies.end(); };
	inline bool canEatAnimalSpecies(const AnimalSpecies* species) const { return std::find(edibleAnimalSpecies.begin(), edibleAnimalSpecies.end(), species) != edibleAnimalSpecies.end(); };
	virtual const vector<ResourceSpecies*> * getEdibleResourceSpecies() const {return &edibleResourceSpecies;};
	inline const vector<AnimalSpecies*> * getEdibleAnimalSpecies() const {return &edibleAnimalSpecies;};

	virtual float getAttackProbability() const { return attackProbability; };
	virtual float getExposedAttackProbability() const { return exposedAttackProbability; };
	virtual float getKillProbability() const { return killProbability; };
	virtual float getAttackAvoidingProbability() const { return Random::randomUniform(); };
	virtual float getKillAvoidingProbability() const { return Random::randomUniform(); };

	virtual float getCellEvaluationBiomass() const { return cellEvaluationBiomass; };
	virtual float getCellEvaluationRisk() const { return cellEvaluationRisk; };
	virtual float getCellEvaluationProConspecific() const { return cellEvaluationProConspecific; };
	virtual float getCellEvaluationAntiConspecific() const { return cellEvaluationAntiConspecific; };
	virtual float getConspecificWeighing() const { return conspecificWeighing; };

	virtual double getMaxPlasticityKVonBertalanffy() const { return maxPlasticityKVonBertalanffy; };
	inline virtual void setMaxPlasticityKVonBertalanffy(const double& maxPlasticityKVonBertalanffy) { this->maxPlasticityKVonBertalanffy = maxPlasticityKVonBertalanffy; }
	virtual double getMinPlasticityKVonBertalanffy() const { return minPlasticityKVonBertalanffy; };
	inline virtual void setMinPlasticityKVonBertalanffy(const double& minPlasticityKVonBertalanffy) { this->minPlasticityKVonBertalanffy = minPlasticityKVonBertalanffy; }
	virtual double getMaxPlasticityDueToConditionVor() const { return maxPlasticityDueToConditionVor; };
	virtual double getMinPlasticityDueToConditionVor() const { return minPlasticityDueToConditionVor; };
	virtual double getMaxPlasticityDueToConditionSearch() const { return maxPlasticityDueToConditionSearch; };
	virtual double getMinPlasticityDueToConditionSearch() const { return minPlasticityDueToConditionSearch; };
	virtual double getMaxPlasticityDueToConditionSpeed() const { return maxPlasticityDueToConditionSpeed; };
	virtual double getMinPlasticityDueToConditionSpeed() const { return minPlasticityDueToConditionSpeed; };

	virtual double getScaleForVoracity() const { return scaleForVoracity; };
	inline virtual void setScaleForVoracity(const double& scaleForVoracity) { this->scaleForVoracity = scaleForVoracity; }
	virtual double getScaleForSearchArea() const { return scaleForSearchArea; };
	inline virtual void setScaleForSearchArea(const double& scaleForSearchArea) { this->scaleForSearchArea = scaleForSearchArea; }
	virtual double getScaleForSpeed() const { return scaleForSpeed; };
	inline virtual void setScaleForSpeed(const double& scaleForSpeed) { this->scaleForSpeed = scaleForSpeed; }
	virtual double getPseudoGrowthMean() const { return pseudoGrowthMean; };
	virtual void setPseudoGrowthMean(double pseudoGrowthMean) { this->pseudoGrowthMean = pseudoGrowthMean; };
	virtual double getPseudoGrowthSd() const { return pseudoGrowthSd; };
	virtual void setPseudoGrowthSd(double pseudoGrowthSd) { this->pseudoGrowthSd = pseudoGrowthSd; };
	virtual double getTempFromLab() const { return tempFromLab; };
	virtual double getTempOptGrowth() const { return tempOptGrowth; };
	virtual double getTempOptSearch() const { return tempOptSearch; };
	virtual double getTempOptSpeed() const { return tempOptSpeed; };
	virtual double getTempOptVoracity() const { return tempOptVoracity; };
	virtual double getEdVoracity() const { return EdVoracity; };
	virtual double getEdSearch() const { return EdSearch; };
	virtual double getEdSpeed() const { return EdSpeed; };
	virtual double getEdGrowth() const { return EdGrowth; };
	virtual double getDevInter() const { return devInter; };
	virtual double getFractSearchExtremeT() const { return fractSearchExtremeT; };
	virtual double getFractSpeedExtremeT() const { return fractSpeedExtremeT; };
	virtual double getMinVorExtremeT() const { return minVorExtremeT; };
	virtual double getTempSizeRuleConstant() const { return tempSizeRuleConstant; };
	inline virtual const double& getDevTime(Instar numberOfInstar) const { return devTimeVector[numberOfInstar.getValue()]; };
	virtual double getDevTimeConstant() const { return devTimeConstant; };
	virtual double getBetaScaleTank() const { return betaScaleTank; };
	virtual float getDecreaseOnTraitsDueToEncounters() const { return decreaseOnTraitsDueToEncounters; };
	virtual double getMaleMobility() const { return maleMobility; };
	virtual bool isForceQuitCell() const { return forceQuitCell; };
	virtual bool isSurviveWithoutFood() const { return surviveWithoutFood; };
	virtual double getLongevitySinceMaturation() const { return longevitySinceMaturation; };
	virtual double getReproTimeFactor() const { return reproTimeFactor; };
	virtual float getPercentageCostForMetabolicDownregulationVoracity() const { return percentageCostForMetabolicDownregulationVoracity; };
	virtual float getPercentageCostForMetabolicDownregulationSearchArea() const { return percentageCostForMetabolicDownregulationSearchArea; };
	virtual float getPercentageCostForMetabolicDownregulationSpeed() const { return percentageCostForMetabolicDownregulationSpeed; };
	virtual float getOptimalTemperatureModifier() const { return optimalTemperatureModifier; };
	virtual double getMaxEncountersT() const { return maxEncountersT; };

 	virtual double getMinSizeHunted() const { return minSizeHunted; };
	virtual double getMaxSizeHunted() const { return maxSizeHunted; };
	virtual double getMeanSizeHunted() const { return meanSizeHunted; };
	
	virtual void setMinSizeHunted(double minSizeHunted) {	this->minSizeHunted = minSizeHunted; };
	virtual void setMaxSizeHunted(double maxSizeHunted) {	this->maxSizeHunted = maxSizeHunted; };
	virtual void setMeanSizeHunted(double meanSizeHunted) {	this->meanSizeHunted = meanSizeHunted; };
	virtual double getSdSizeHunted() const { return sdSizeHunted; };
	virtual void setSdSizeHunted(double sdSizeHunted) {	this->sdSizeHunted = sdSizeHunted; };
	
	virtual double getMinVorHunted() const { return minVorHunted; };
	virtual double getMaxVorHunted() const { return maxVorHunted; };
	virtual double getMeanVorHunted() const { return meanVorHunted; };
	
	virtual void setMinVorHunted(double minVorHunted) {	this->minVorHunted = minVorHunted; };
	virtual void setMaxVorHunted(double maxVorHunted) {	this->maxVorHunted = maxVorHunted; };
	virtual void setMeanVorHunted(double meanVorHunted) { this->meanVorHunted = meanVorHunted; };
	virtual double getSdVorHunted() { return sdVorHunted; };
	virtual void setSdVorHunted(double sdVorHunted) {	this->sdVorHunted = sdVorHunted; };
	
	virtual double getMinSpeedHunted() const { return minSpeedHunted; };
	virtual double getMaxSpeedHunted() const { return maxSpeedHunted; };
	virtual double getMeanSpeedHunted() const { return meanSpeedHunted; };
	
	virtual void setMinSpeedHunted(double minSpeedHunted) {	this->minSpeedHunted = minSpeedHunted; };
	virtual void setMaxSpeedHunted(double maxSpeedHunted) {	this->maxSpeedHunted = maxSpeedHunted; };
	virtual void setMeanSpeedHunted(double meanSpeedHunted) { this->meanSpeedHunted = meanSpeedHunted; };
	virtual double getSdSpeedHunted() const { return sdSpeedHunted; };
	virtual void setSdSpeedHunted(double sdSpeedHunted) { this->sdSpeedHunted = sdSpeedHunted; };

	virtual double getMinSizeHunter() const { return minSizeHunter; };
	virtual void setMinSizeHunter(double minSizeHunter) {	this->minSizeHunter = minSizeHunter; };
	virtual double getMaxSizeHunter() const { return maxSizeHunter; };
	virtual void setMaxSizeHunter(double maxSizeHunter) {	this->maxSizeHunter = maxSizeHunter; };
	virtual double getMeanSizeHunter() const { return meanSizeHunter; };
	virtual void setMeanSizeHunter(double meanSizeHunter) {	this->meanSizeHunter = meanSizeHunter; };
	virtual double getSdSizeHunter() const{ return sdSizeHunter; };
	virtual void setSdSizeHunter(double sdSizeHunter) {	this->sdSizeHunter = sdSizeHunter; };

	virtual double getMinVorHunter() const { return minVorHunter; };
	virtual void setMinVorHunter(double minVorHunter) {	this->minVorHunter = minVorHunter; };
	virtual double getMaxVorHunter() const { return maxVorHunter; };
	virtual void setMaxVorHunter(double maxVorHunter) {	this->maxVorHunter = maxVorHunter; };
	virtual double getMeanVorHunter() const { return meanVorHunter; };
	virtual void setMeanVorHunter(double meanVorHunter) {	this->meanVorHunter = meanVorHunter; };
	virtual double getSdVorHunter() const { return sdVorHunter; };
	virtual void setSdVorHunter(double sdVorHunter) {	this->sdVorHunter = sdVorHunter; };

	virtual double getMinSpeedHunter() const { return minSpeedHunter; };
	virtual void setMinSpeedHunter(double minSpeedHunter) { this->minSpeedHunter = minSpeedHunter; };
	virtual double getMaxSpeedHunter() const { return maxSpeedHunter; };
	virtual void setMaxSpeedHunter(double maxSpeedHunter) { this->maxSpeedHunter = maxSpeedHunter; };
	virtual double getMeanSpeedHunter() const { return meanSpeedHunter; };
	virtual void setMeanSpeedHunter(double meanSpeedHunter) { this->meanSpeedHunter = meanSpeedHunter; };
	virtual double getSdSpeedHunter() const { return sdSpeedHunter; };
	virtual void setSdSpeedHunter(double sdSpeedHunter) { this->sdSpeedHunter = sdSpeedHunter; };

	virtual double getMinSearchAreaHunter() const { return minSearchAreaHunter; };
	virtual void setMinSearchAreaHunter(double minSearchAreaHunter) {	this->minSearchAreaHunter = minSearchAreaHunter; };
	virtual double getMaxSearchAreaHunter() const { return maxSearchAreaHunter; };
	virtual void setMaxSearchAreaHunter(double maxSearchAreaHunter) {	this->maxSearchAreaHunter = maxSearchAreaHunter; };
	virtual double getMeanSearchAreaHunter() const { return meanSearchAreaHunter; };
	virtual void setMeanSearchAreaHunter(double meanSearchAreaHunter) {	this->meanSearchAreaHunter = meanSearchAreaHunter; };
	virtual double getSdSearchAreaHunter() const { return sdSearchAreaHunter; };
	virtual void setSdSearchAreaHunter(double sdSearchAreaHunter) {	this->sdSearchAreaHunter = sdSearchAreaHunter; };

	virtual double getMinProbabilityDensityFunction() const { return minProbabilityDensityFunction; };
	virtual void setMinProbabilityDensityFunction(double minProbabilityDensityFunction) {	this->minProbabilityDensityFunction = minProbabilityDensityFunction; };
	virtual double getMaxProbabilityDensityFunction() const { return maxProbabilityDensityFunction; };
	virtual void setMaxProbabilityDensityFunction(double maxProbabilityDensityFunction) {	this->maxProbabilityDensityFunction = maxProbabilityDensityFunction; };
	virtual double getMeanProbabilityDensityFunction() const { return meanProbabilityDensityFunction; };
	virtual void setMeanProbabilityDensityFunction(double meanProbabilityDensityFunction) {	this->meanProbabilityDensityFunction = meanProbabilityDensityFunction; };
	virtual double getSdProbabilityDensityFunction() const { return sdProbabilityDensityFunction; };
	virtual void setSdProbabilityDensityFunction(double sdProbabilityDensityFunction) {	this->sdProbabilityDensityFunction = sdProbabilityDensityFunction; };

	virtual double getMinVorXVor() const { return minVorXVor; };
	virtual void setMinVorXVor(double minVorXVor) { this->minVorXVor = minVorXVor; };
	virtual double getMaxVorXVor() const { return maxVorXVor; };
	virtual void setMaxVorXVor(double maxVorXVor) { this->maxVorXVor = maxVorXVor; };
	virtual double getMeanVorXVor() const { return meanVorXVor; };
	virtual void setMeanVorXVor(double meanVorXVor) { this->meanVorXVor = meanVorXVor; };
	virtual double getSdVorXVor() const { return sdVorXVor; };
	virtual void setSdVorXVor(double sdVorXVor) { this->sdVorXVor = sdVorXVor; };

	virtual double getMinSpeedRatio() const { return minSpeedRatio; };
	virtual void setMinSpeedRatio(double minSpeedRatio) {	this->minSpeedRatio = minSpeedRatio; };
	virtual double getMaxSpeedRatio() const { return maxSpeedRatio; };
	virtual void setMaxSpeedRatio(double maxSpeedRatio) {	this->maxSpeedRatio = maxSpeedRatio; };
	virtual double getMeanSpeedRatio() const { return meanSpeedRatio; };
	virtual void setMeanSpeedRatio(double meanSpeedRatio) {	this->meanSpeedRatio = meanSpeedRatio; };
	virtual double getSdSpeedRatio() const { return sdSpeedRatio; };
	virtual void setSdSpeedRatio(double sdSpeedRatio) {	this->sdSpeedRatio = sdSpeedRatio; };

	virtual double getMeanSizeRatio() const { return meanSizeRatio; };
	virtual void setMeanSizeRatio(double meanSizeRatio) { this->meanSizeRatio = meanSizeRatio; };
	virtual double getSdSizeRatio() const { return sdSizeRatio; };
	virtual void setSdSizeRatio(double sdSizeRatio) { this->sdSizeRatio = sdSizeRatio; };

	virtual float getExperienceInfluencePerDay() const { return experienceInfluencePerDay; };
	virtual void addPredationEventOnOtherSpecies(int predatedSpeciesId) { predationEventsOnOtherSpecies.at(predatedSpeciesId)++; };

	virtual double getProbabilityDeathFromBackground() const { return probabilityDeathFromBackground; };
	virtual double getFemaleMaxReproductionEvents() const { return femaleMaxReproductionEvents; };
	virtual double getEggsPerBatch() const { return eggsPerBatch; };
	virtual double getMaleMaxReproductionEvents() const { return maleMaxReproductionEvents; };


	virtual void setInitialPredationEventsOnOtherSpecies(unsigned int numberOfSpecies);

	virtual void calculatePseudoGrowthMean();
	virtual void calculatePseudoGrowthSd();

	virtual void sumStatisticMeans(double hunterAnimalBodySize, double hunterAnimalVoracity, double hunterAnimalSpeed, double hunterAnimalDryMass, double huntedAnimalBodySize, double huntedAnimalVoracity, double huntedAnimalSpeed, double huntedAnimalDryMass, double muForPDF, double sigmaForPDF);
	virtual void interactionRanges(double hunterAnimalBodySize, double hunterAnimalVoracity, double hunterAnimalSpeed, double hunterAnimalDryMass, double huntedAnimalBodySize, double huntedAnimalVoracity, double huntedAnimalSpeed, double huntedAnimalDryMass, double muForPDF, double sigmaForPDF);
	virtual void computeStatisticMeans(unsigned int numberOfAttacks);
	virtual void sumStatisticSds(double hunterAnimalBodySize, double hunterAnimalVoracity, double hunterAnimalSpeed, double hunterAnimalDryMass, double huntedAnimalBodySize, double huntedAnimalVoracity, double huntedAnimalSpeed, double huntedAnimalDryMass, double muForPDF, double sigmaForPDF);
	virtual void computeStatisticSds(unsigned int numberOfAttacks);

	virtual void initWetBiomassDensitiesPerAge(float temperature, int timeStepsPerDay);
	virtual bool canEatAnyResourceSpecies() { return !edibleResourceSpecies.empty(); };
	virtual bool canEatAnyAnimalSpecies() { return !edibleAnimalSpecies.empty(); };
	virtual vector<ResourceSpecies*> * getInvolvedResourceSpecies();

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
	void setLinfKcorr(const double& LinfKcorr);
	inline void setDevTimeConstant(const double& devTimeConstant) { this->devTimeConstant = devTimeConstant; }
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
	void setSexRatio(const float& sexRatio) { this->sexRatio = sexRatio; }

	unsigned int getSize() { return size; }
	void setSize(const unsigned int size) { this->size = size; }

	inline void setFemaleMaxReproductionEvents(const int& femaleMaxReproductionEvents) { this->femaleMaxReproductionEvents = femaleMaxReproductionEvents; }
	void setEggsPerBatch(const double& eggsPerBatch) { this->eggsPerBatch = eggsPerBatch; }
	void setMaleMaxReproductionEvents(const int& maleMaxReproductionEvents) { this->maleMaxReproductionEvents = maleMaxReproductionEvents; }

	void setExperienceInfluencePerDay(const float& daysToRememberAbundancesExperienced);
	inline void setMaleMobility(const double& maleMobility) { this->maleMobility = maleMobility; }
	inline void setForceQuitCell(const bool forceQuitCell) { this->forceQuitCell = forceQuitCell; }
	inline void setSurviveWithoutFood(const bool surviveWithoutFood) { this->surviveWithoutFood = surviveWithoutFood; }
	inline void setDecreaseOnTraitsDueToEncounters(const float& decreaseOnTraitsDueToEncounters) { this->decreaseOnTraitsDueToEncounters = decreaseOnTraitsDueToEncounters; }
	void setDryProportionOfMass(double CONTROL_DRY_BODY_MASS);
	inline void setProbabilityDeathFromBackground(const double& probabilityDeathFromBackground) { this->probabilityDeathFromBackground = probabilityDeathFromBackground; }









	inline void setInitialPopulation(const vector<double>& initialPopulation) { this->initialPopulation = initialPopulation; }
	inline void setStatisticsInitialPopulation(const vector<unsigned long>& statisticsInitialPopulation) { this->statisticsInitialPopulation = statisticsInitialPopulation; }


	void setTraits(const unordered_map<string,double>& fixedTraits, const unordered_map<string,double>& minTraitRanges, 
				   const unordered_map<string,double>& maxTraitRanges, const unordered_map<string,double>& minTraitLimits, 
				   const unordered_map<string,double>& maxTraitLimits, const vector<string>& order, const std::vector<double>& restrictPerTrait);
	void resetLimits();
	void resetPseudoGrowthMean();
	void resetPseudoGrowthSd();





	void fillInvolvedResourceSpecies(vector<ResourceSpecies*> * involvedResourceSpecies, vector<AnimalSpecies*> * alreadyCheckedAnimalSpecies, const vector<AnimalSpecies*> * involvedAnimalSpecies);

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
