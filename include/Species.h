/*
 * species.h
 *
 *  Created on: May 24, 2013
 *      Author: jrbcast
 */

#ifndef WEAVER_SPECIES_H_
#define WEAVER_SPECIES_H_

#include <map>
#include <unordered_map>
#include <vector>
#include "Edible.h"
#include <string>
#include "string/trim.h"
#include "Types.h"

using namespace std;

class ChromosomesGenerator;



class TraitConverter {
public:
	enum Trait {
		energy_tank, //M1_TRAIT1: Energy tank at birth and after molting
		growth, //M1_TRAIT2: Growth
		pheno, //M1_TRAIT3: Phenology

		factorEggMass, //M2_TRAIT1: Body size at birth
		assim, //M2_TRAIT2: Assimilation
		voracity, //M2_TRAIT3: Voracity

		speed, //M3_TRAIT1: Speed
		search_area, //M3_TRAIT2: Search area
		met_rate, //M3_TRAIT3: Metabolic rate


// Comment from here
// Change numberOfTraits: 10 AND
// remove these traits in ranges and limits AND
// remove restrictPerTrati values until leaving only 10 of them AND
// remove correlationCoefficientRhoPerModule until leaving 4 of them AND
// comment the traitsDictionary code section in AnimalSpecies.cpp (instructions there)

		actE_vor, //M4_TRAIT1: Q10 on voracity
		actE_speed, //M4_TRAIT2: Q10 on speed
		actE_search, //M4_TRAIT3: Q10 on search area

		dummy1,
		dummy2,
		dummy3,

		dummy4,
		dummy5,
		dummy6,

		dummy7,
		dummy8,
		dummy9,

		dummy10,
		dummy11,
		dummy12,


// Comment through here

		actE_met, //M5_SINGLETRAIT: Activation energy for metabolic rate
		end_trait
	};

	inline static Trait stringToEnumValue(const std::string &str) { return stringToEnum.at(str); }
	static std::string& printAvailableTraits();
	static size_t size();
private:
	static std::unordered_map<std::string, Trait> stringToEnum;
	static std::string enumHeader;
	static size_t enumSize;
};


typedef TraitConverter::Trait Trait;


class Species
{
private:
	unsigned int id;
	static unsigned int speciesCounter;
	string scientificName;
	float conversionToWetMass;

public:
	Species(const string& scientificName);
	virtual ~Species();

	unsigned int getId() const { return id; };
	string getScientificName() { return scientificName; };
	void modifyScientificName(string newScientificName);
	inline const float& getConversionToWetMass() const { return conversionToWetMass; }
	virtual double getCellMass(){ return 0; };
	inline void setConversionToWetMass(const float& conversionToWetMass) { this->conversionToWetMass = conversionToWetMass; }

	virtual unsigned int getNumberOfTraits() { return 0; };
	virtual unsigned int getNumberOfLociPerTrait() { return 0; };
	virtual unsigned int getTraitsPerModule() const { return 0; };
	virtual unsigned int getNumberOfAllelesPerLocus() const { return 0; };
	virtual unsigned int getNumberOfChiasmasPerChromosome() const { return 0; };
	virtual unsigned int getNumberOfLociPerChromosome() const {	return 0; }
	virtual unsigned int getNumberOfChromosomes() const { return 0; };
	virtual ChromosomesGenerator* getChromosomesGenerator() { return NULL; };
	virtual double getRhoRangePerModule(int moduleNumber) { return 0.0; };
	virtual double getRhoPerModule(int moduleNumber) { return 0.0; };
	virtual double getValueFromNormalDistributionY() { return 0.0; };
	virtual double getValueFromCholMat(int row, int column) { return 0.0; };
	virtual double getVonBertLdistanceMin() { return 0.0; };
	virtual double getVonBertLdistanceMax() { return 0.0; };

	virtual const unordered_map<Trait, double>& getFixedTraits() const { return unordered_map<Trait, double>(); };
	virtual vector<Trait> getVariableTraits() const { return vector<Trait>(); };
	virtual unsigned int getTraitOrder(const Trait& trait) const { return 0; };

	virtual const double& getMinTraitRange(const Trait& trait) { return 0.0; };
	virtual const double& getMaxTraitRange(const Trait& trait) { return 0.0; };
	virtual const double& getMinRestrictedRange(const Trait& trait) { return 0.0; };
	virtual const double& getMaxRestrictedRange(const Trait& trait) { return 0.0; };
	virtual const double& getMinTraitLimit(const Trait& trait) { return 0.0; };
	virtual const double& getMaxTraitLimit(const Trait& trait) { return 0.0; };
	virtual const double& getMinObservedPseudoValue(const Trait& trait) { return 0.0; };
	virtual void setMinObservedPseudoValue(const Trait& trait, const double& minObservedPseudoValue) {};
	virtual const double& getMaxObservedPseudoValue(const Trait& trait) { return 0.0; };
	virtual void setMaxObservedPseudoValue(const Trait& trait, const double& maxObservedPseudoValue) {};

	virtual string getPatchesDirectoryName() { return ""; };
	virtual double getMinHR() { return 0.0; };
	virtual double getMaxHR() { return 0.0; };
	virtual double getRateOfGrowth(double temperature, double moisture) { return 0.0; };
	virtual double getMinimumEdibleBiomass() { return 0.0; };
	virtual bool isMobile() { return false; };
	virtual void scaleInitialPopulation(double totalPopulationDensity, double initialEcosystemSize) {};
	virtual const vector<double>& getInitialPopulation() const { return vector<double>(); };
	virtual vector<unsigned long>* getStatisticsInitialPopulation() { return NULL; };
	virtual double getTotalInitialPopulation() { return 0.0; };
	virtual unsigned long getTotalStatisticsInitialPopulation() { return 0; };
	virtual bool isExtinguished() { return false; };
	virtual void setExtinguished(bool extinguished) {};
	virtual vector<int>* getPredationEventsOnOtherSpecies() { return NULL; };
	virtual unsigned int getDefaultHuntingMode() { return 0; };

	virtual double getExcessInvestInSize() { return 0.0; };
	virtual void setExcessInvestInSize(const double& excessInvestInSize) {};
	virtual bool hasIndeterminateGrowth() { return false; };
	virtual void setIndeterminateGrowth(const bool& indeterminateGrowth) {}
	virtual const Instar& getInstarFirstReproduction() const { return Instar(); };
	virtual void setInstarFirstReproduction(const Instar& instarFirstReproduction) {}
	virtual const unsigned int& getInstarsForNextReproduction() const { return 0; };
	virtual void setInstarsForNextReproduction(const unsigned int& instarsForNextReproduction) {}
	virtual const unsigned int& getNumberOfInstars() const { return 0; };
	virtual void setNumberOfInstars(const unsigned int& numberOfInstars) {};
	virtual int getPupaPeriodLength() { return 0; };
	virtual void setPupaPeriodLength(const int& pupaPeriodLength) {};
	virtual double getCoefficientForMassA() { return 0.0; };
	virtual void setCoefficientForMassA(const double& coefficientForMassA) {}
	virtual double getScaleForMassB() { return 0.0; };
	virtual void setScaleForMassB(const double& scaleForMassB) {}

	virtual double getCoefficientForMassAforMature() { return 0.0; };
	virtual void setCoefficientForMassAforMature(const double& coefficientForMassAforMature) {}

	virtual double getScaleForMassBforMature() { return 0.0; };
	virtual void setScaleForMassBforMature(const double& scaleForMassBforMature) {}


	virtual double getAssignedForMolt() { return 0.0; };
	virtual bool hasEggClutchFromEquation() { return false; };
	virtual void setEggClutchFromEquation(const bool& eggClutchFromEquation) {}
	virtual void setEggsPerBatch(const double& eggsPerBatch) {};
	virtual double getForClutchMassCoefficient() { return 0.0; };
	virtual void setForClutchMassCoefficient(const double& forClutchMassCoefficient) {}
	virtual double getForClutchMassScale() { return 0.0; };
	virtual void setForClutchMassScale(const double& forClutchMassScale) {}
	virtual double getMaleReproductionFactor() { return 0.0; };
	virtual void setMaleReproductionFactor(const double& maleReproductionFactor) {}
	virtual int getSexualType() { return 0; };
	virtual void setSexualType(int sexualType) {};
    virtual int getGrowthType() { return 0; };
	virtual void setGrowthType(int growthType) {};
	virtual int getRandomGender() { return 0; };
	virtual double getMinRelativeHumidityThreshold() { return 0.0; };
	virtual void setMinRelativeHumidityThreshold(const double& minRelativeHumidityThreshold) {}
	virtual float getDaysWithoutFoodForMetabolicDownregulation() { return 0.0; };
	virtual void setDaysWithoutFoodForMetabolicDownregulation(const float& daysWithoutFoodForMetabolicDownregulation) {}
	virtual float getPercentageMetabolicDownregulation() { return 0.0; };
	virtual void setPercentageMetabolicDownregulation(const float& percentageMetabolicDownregulation) {}

	virtual bool canEatFungusSpecies(Species* species) { return false; };
	virtual bool canEatAnimalSpecies(Species* species) { return false; };
	virtual bool canEatPlantSpecies(Species* species) { return false; };
	virtual vector<Species*> * getEdibleFungusSpecies() { return NULL; };
	virtual void addEdibleFungusSpecies(Species* species) {};
	virtual vector<Species*> * getEdibleAnimalSpecies() { return NULL; };
	virtual void addEdibleAnimalSpecies(Species* species) {};
	virtual vector<Species*> * getEdiblePlantSpecies() { return NULL; };
	virtual void addEdiblePlantSpecies(Species* species) {};
	virtual float getEdiblePreference(Species* species) { return 0.0; };
	virtual void addEdiblePreference(Species* speciesToBeAdded, float ediblePreferenceToBeAdded) {};
	virtual float getEdibleProfitability(Species* species) { return 0.0; };
	virtual void addEdibleProfitability(Species* speciesToBeAdded, float edibleProfitabilityToBeAdded) {};

	virtual double getEggDryMass() { return 0.0; };

	virtual float getAttackProbability() { return 0.0; };
	virtual float getExposedAttackProbability() { return 0.0; };
	virtual float getKillProbability() { return 0.0; };
	virtual float getAttackAvoidingProbability() { return 0.0; };
	virtual float getKillAvoidingProbability() { return 0.0; };

	virtual float getCellEvaluationBiomass() { return 0.0; };
	virtual float getCellEvaluationRisk() { return 0.0; };
	virtual float getCellEvaluationProConspecific() { return 0.0; };
	virtual float getCellEvaluationAntiConspecific() { return 0.0; };
	virtual float getConspecificWeighing() { return 0.0; };

	virtual double getMaxPlasticityKVonBertalanffy() { return 0.0; };
	virtual void setMaxPlasticityKVonBertalanffy(const double& maxPlasticityKVonBertalanffy) {}
	virtual double getMinPlasticityKVonBertalanffy() { return 0.0; };
	virtual void setMinPlasticityKVonBertalanffy(const double& minPlasticityKVonBertalanffy) {}
	virtual double getMaxPlasticityDueToConditionVor() { return 0.0; };
	virtual double getMinPlasticityDueToConditionVor() { return 0.0; };
	virtual double getMaxPlasticityDueToConditionSearch() { return 0.0; };
	virtual double getMinPlasticityDueToConditionSearch() { return 0.0; };
	virtual double getMaxPlasticityDueToConditionSpeed() { return 0.0; };
	virtual double getMinPlasticityDueToConditionSpeed() { return 0.0; };
	virtual double getScaleForVoracity() { return 0.0; };
	virtual void setScaleForVoracity(const double& scaleForVoracity) {}
	virtual double getScaleForSearchArea() { return 0.0; };
	virtual void setScaleForSearchArea(const double& scaleForSearchArea) {}
	virtual double getScaleForSpeed() { return 0.0; };
	virtual void setScaleForSpeed(const double& scaleForSpeed) {}
	virtual double getPseudoGrowthMean() { return 0.0; };
	virtual void setPseudoGrowthMean(double pseudoGrowthMean) {};
	virtual double getPseudoGrowthSd() { return 0.0; };
	virtual void setPseudoGrowthSd(double pseudoGrowthSd) {};
	virtual double getTempFromLab() { return 0.0; }
	virtual double getTempOptGrowth() { return 0.0; }
	virtual double getTempOptSearch() { return 0.0; }
	virtual double getTempOptSpeed() { return 0.0; }
	virtual double getTempOptVoracity() { return 0.0; }
	virtual double getEdVoracity() { return 0.0; }
	virtual double getEdSearch() { return 0.0; }
	virtual double getEdSpeed() { return 0.0; }
	virtual double getEdGrowth() { return 0.0; }
	virtual double getDevInter() { return 0.0; }
	virtual double getFractSearchExtremeT() { return 0.0; }
	virtual double getFractSpeedExtremeT() { return 0.0; }
	virtual double getMinVorExtremeT() { return 0.0; }
	virtual double getTempSizeRuleConstant() { return 0.0; };
	virtual const vector<double>& getDevTimeVector() const { return vector<double>(); }
	virtual const double& getDevTime(Instar numberOfInstar) const { return 0.0; };
	virtual double getDevTimeConstant() { return 0.0; };
	virtual double getVonBertTime0() { return 0.0; };
	virtual double getBetaScaleTank() { return 0.0; };
	virtual float getDecreaseOnTraitsDueToEncounters() { return 0.0; };
	virtual double getMaleMobility() { return 0.0; };
	virtual double getQuitCellThreshold() { return 0.0; };
	virtual double getLongevitySinceMaturation() { return 0.0; };
	virtual double getReproTimeFactor() { return 0.0; };
	virtual float getPercentageCostForMetabolicDownregulationVoracity() { return 0.0; };
	virtual float getPercentageCostForMetabolicDownregulationSearchArea() { return 0.0; };
	virtual float getPercentageCostForMetabolicDownregulationSpeed() { return 0.0; };
	virtual float getOptimalTemperatureModifier() { return 0.0; };
	virtual double getMaxEncountersT() { return 0.0; };

	virtual double getMinSizeHunted() { return 0.0; };
	virtual double getMaxSizeHunted() { return 0.0; };
	virtual double getMeanSizeHunted() { return 0.0; };
	virtual void setMinSizeHunted(double minSizeHunted) {};
	virtual void setMaxSizeHunted(double maxSizeHunted) {};
	virtual void setMeanSizeHunted(double meanSizeHunted) {};
	virtual double getSdSizeHunted() { return 0.0; };
	virtual void setSdSizeHunted(double sdSizeHunted) {};
	virtual double getMinVorHunted() { return 0.0; };
	virtual double getMaxVorHunted() { return 0.0; };
	virtual double getMeanVorHunted() { return 0.0; };
	virtual void setMinVorHunted(double minVorHunted) {};
	virtual void setMaxVorHunted(double maxVorHunted) {};
	virtual void setMeanVorHunted(double meanVorHunted) {};
	virtual double getSdVorHunted() { return 0.0; };
	virtual void setSdVorHunted(double sdVorHunted) {};
	virtual double getMinSpeedHunted() { return 0.0; };
	virtual double getMaxSpeedHunted() { return 0.0; };
	virtual double getMeanSpeedHunted() { return 0.0; };
	virtual void setMinSpeedHunted(double minSpeedHunted) {};
	virtual void setMaxSpeedHunted(double maxSpeedHunted) {};
	virtual void setMeanSpeedHunted(double meanSpeedHunted) {};
	virtual double getSdSpeedHunted() { return 0.0; };
	virtual void setSdSpeedHunted(double sdSpeedHunted) {};

	virtual double getMinSizeHunter() { return 0.0; };
	virtual void setMinSizeHunter(double minSizeHunter) {};
	virtual double getMaxSizeHunter() { return 0.0; };
	virtual void setMaxSizeHunter(double maxSizeHunter) {};

	virtual double getMeanSizeHunter() { return 0.0; };
	virtual void setMeanSizeHunter(double meanSizeHunter) {};
	virtual double getSdSizeHunter() { return 0.0; };
	virtual void setSdSizeHunter(double sdSizeHunter) {};
	
	virtual double getMinVorHunter() { return 0.0; };
	virtual void setMinVorHunter(double minVorHunter) {};
	virtual double getMaxVorHunter() { return 0.0; };
	virtual void setMaxVorHunter(double sdVorHunter) {};
		
	virtual double getMeanVorHunter() { return 0.0; };
	virtual void setMeanVorHunter(double meanVorHunter) {};
	virtual double getSdVorHunter() { return 0.0; };
	virtual void setSdVorHunter(double sdVorHunter) {};
	
	virtual double getMinSpeedHunter() { return 0.0; };
	virtual void setMinSpeedHunter(double minSpeedHunter) {};
	virtual double getMaxSpeedHunter() { return 0.0; };
	virtual void setMaxSpeedHunter(double maxSpeedHunter) {};

    virtual double getMeanSpeedHunter() { return 0.0; };
	virtual void setMeanSpeedHunter(double meanSpeedHunter) {};
	virtual double getSdSpeedHunter() { return 0.0; };
	virtual void setSdSpeedHunter(double sdSpeedHunter) {};

	virtual double getMeanSearchAreaHunter() { return 0.0; };
	virtual void setMeanSearchAreaHunter(double meanSearchAreaHunter) {};
	virtual double getSdSearchAreaHunter() { return 0.0; };
	virtual void setSdSearchAreaHunter(double sdSearchAreaHunter) {};

	virtual double getMinProbabilityDensityFunction() { return 0.0; };
	virtual void setMinProbabilityDensityFunction(double minProbabilityDensityFunction) {};
	virtual double getMaxProbabilityDensityFunction() { return 0.0; };
	virtual void setMaxProbabilityDensityFunction(double maxProbabilityDensityFunction) {};

	virtual double getMeanProbabilityDensityFunction() { return 0.0; };
	virtual void setMeanProbabilityDensityFunction(double meanProbabilityDensityFunction) {};
	virtual double getSdProbabilityDensityFunction() { return 0.0; };
	virtual void setSdProbabilityDensityFunction(double sdProbabilityDensityFunction) {};
	
	virtual double getMinVorXVor() { return 0.0; };
	virtual void setMinVorXVor(double minVorXVor) {};
	virtual double getMaxVorXVor() { return 0.0; };
	virtual void setMaxVorXVor(double maxVorXVor) {};

	virtual double getMeanVorXVor() { return 0.0; };
	virtual void setMeanVorXVor(double meanVorXVor) {};
	virtual double getSdVorXVor() { return 0.0; };
	virtual void setSdVorXVor(double sdVorXVor) {};

	virtual double getMinSpeedRatio() { return 0.0; };
	virtual void setMinSpeedRatio(double minSpeedRatio) {};
	virtual double getMaxSpeedRatio() { return 0.0; };
	virtual void setMaxSpeedRatio(double maxSpeedRatio) {};

	virtual double getMeanSpeedRatio() { return 0.0; };
	virtual void setMeanSpeedRatio(double meanSpeedRatio) {};
	virtual double getSdSpeedRatio() { return 0.0; };
	virtual void setSdSpeedRatio(double sdSpeedRatio) {};

	virtual double getMeanSizeRatio() { return 0.0; };
	virtual void setMeanSizeRatio(double meanSizeRatio) {};
	virtual double getSdSizeRatio() { return 0.0; };
	virtual void setSdSizeRatio(double sdSizeRatio) {};

	virtual float getExperienceInfluencePerDay() {return 0.0; };
	virtual void addPredationEventOnOtherSpecies(int predatedSpeciesId) {};

	virtual double getProbabilityDeathFromBackground() { return 0.0; };
	virtual double getFemaleMaxReproductionEvents() { return 0.0; };
	virtual double getEggsPerBatch() { return 0.0; };
	virtual double getMaleMaxReproductionEvents() { return 0.0; };


	virtual void setInitialPredationEventsOnOtherSpecies(unsigned int numberOfSpecies) {};

	virtual void calculatePseudoGrowthMean() {};
	virtual void calculatePseudoGrowthSd() {};

	virtual void interactionRanges(Edible* hunterAnimal, Edible* huntedAnimal, double muForPDF, double sigmaForPDF) {};
	virtual void sumStatisticMeans(Edible* hunterAnimal, Edible* huntedAnimal, double muForPDF, double sigmaForPDF) {};
	virtual void computeStatisticMeans(unsigned int numberOfAttacks) {};
	virtual void sumStatisticSds(Edible* hunterAnimal, Edible* huntedAnimal, double muForPDF, double sigmaForPDF) {};
	virtual void computeStatisticSds(unsigned int numberOfAttacks) {};

	virtual void initWetBiomassDensitiesPerAge(float temperature, int timeStepsPerDay) {};
	virtual bool canEatAnyFungusSpecies() { return false; };
	virtual bool canEatAnyAnimalSpecies() { return false; };
	virtual vector<Species*> * getInvolvedFungusSpecies() { return NULL; };

	bool operator<(const Species& other) const;
};

#endif /* WEAVER_SPECIES_H_ */
