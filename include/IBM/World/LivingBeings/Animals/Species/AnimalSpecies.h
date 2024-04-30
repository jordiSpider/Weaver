/*
 * AnimalSpecies.h
 *
 *  Created on: Jul 6, 2013
 *      Author: jrbcast
 */

#ifndef ANIMAL_SPECIES_H_
#define ANIMAL_SPECIES_H_

#include <memory>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <limits>
#include <cfloat>
#include <random>
#include <cmath>
#include <list>
#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/LivingBeings/Species.h"
#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Locus.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Correlosome.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Chromosome.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Genome.h"
#include "Misc/Types.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/Trait.h"
#include "Misc/OntogeneticLink.h"
#include "Exceptions/LineInfoException.h"
#include "IBM/Maths/Random.h"
#include "IBM/Maths/MathFunctions.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "IBM/Maths/Conversions.h"
#include "IBM/World/LivingBeings/Animals/Species/GrowthModule.h"
#include "Misc/CustomIndexedVector.h"
#include "IBM/World/LivingBeings/Animals/AnimalInterface.h"



using json = nlohmann::json;

class ChromosomesGenerator;

class Map;
class World;

class EdibleSearchParams;
class AnimalSearchParams;


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

	const SexualTypeValue& getValue() const { return value; }
	operator int() const { return value; }
	friend bool operator==(const SexualType& lhs, const SexualTypeValue& rhs) { return lhs.getValue() == rhs; }
	friend bool operator==(const SexualTypeValue& lhs, const SexualType& rhs) { return lhs == rhs.getValue(); }

	/**
     * @brief Serialize the Trait object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
	static const std::unordered_map<std::string_view, const SexualTypeValue> stringToEnum;
	static const std::string enumValues;

	friend class boost::serialization::access;

	static SexualTypeValue stringToEnumValue(const std::string &str) {
		try
		{
			return stringToEnum.at(str);
		}
		catch(const std::out_of_range& e) 
		{
			throwLineInfoException(fmt::format("Unknown sexual type '{}'. Valid values are {}", str, printAvailableValues()));
		}
	}
	static std::string_view printAvailableValues() { return enumValues; }
	static std::string_view to_string(const SexualTypeValue& sexualType) { return magic_enum::enum_name(sexualType); }
	static const std::unordered_map<std::string_view, const SexualTypeValue> generateMap();
	static constexpr size_t size() { return magic_enum::enum_count<SexualTypeValue>(); }
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

	const HuntingModeValue& getValue() const { return value; }
	operator int() const { return value; }
	friend bool operator!=(const HuntingMode& lhs, const HuntingModeValue& rhs) { return lhs.getValue() != rhs; }
	friend bool operator!=(const HuntingModeValue& lhs, const HuntingMode& rhs) { return lhs != rhs.getValue(); }
	friend bool operator==(const HuntingMode& lhs, const HuntingModeValue& rhs) { return lhs.getValue() == rhs; }
	friend bool operator==(const HuntingModeValue& lhs, const HuntingMode& rhs) { return lhs == rhs.getValue(); }

	/**
     * @brief Serialize the Trait object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
	static const std::unordered_map<std::string_view, const HuntingModeValue> stringToEnum;
	static const std::string enumValues;

	friend class boost::serialization::access;

	static HuntingModeValue stringToEnumValue(const std::string &str) {
		try
		{
			return stringToEnum.at(str);
		}
		catch(const std::out_of_range& e) 
		{
			throwLineInfoException(fmt::format("Unknown sexual type '{}'. Valid values are {}", str, printAvailableValues()));
		}
	}
	static std::string_view printAvailableValues() { return enumValues; }
	static std::string_view to_string(const HuntingModeValue& huntingMode) { return magic_enum::enum_name(huntingMode); }
	static const std::unordered_map<std::string_view, const HuntingModeValue> generateMap();
	static constexpr size_t size() { return magic_enum::enum_count<HuntingModeValue>(); }
	static const std::string generateAvailableValues();

	const HuntingModeValue value;
};

class AnimalSpecies: public Species
{
private:
	static id_type animalSpeciesCounter;

	const id_type animalSpeciesId;
	const HuntingMode defaultHuntingMode;

	InstarVector<std::vector<std::unordered_map<Species*, std::vector<Instar>>>> edibleSpecies;

	std::vector<std::vector<std::vector<OntogeneticLink>>> edibleOntogeneticLink;

	InstarVector<EdibleSearchParams> edibleSearchParams;
	std::vector<EdibleSearchParams> breedSearchParams;
	InstarVector<EdibleSearchParams> cellEvaluationSearchParams;
	std::unique_ptr<EdibleSearchParams> matureFemalesSearchParams;
	std::unique_ptr<AnimalSearchParams> populationSearchParams;
	std::vector<AnimalSearchParams> lifeStageSearchParams;

	float experienceInfluencePerDay;

	std::vector<int> predationEventsOnOtherSpecies;

	CustomIndexedVector<Trait::Type, Trait*> traits;

	std::vector<std::pair<Trait::Type, TraitDefinitionSection::Elements>> individualLevelTraitElements;

	std::vector<Trait::Type> temperatureDependentTraits;

	unsigned int numberOfLinksAsPredator;
	unsigned int numberOfLinksAsPrey;

	InstarVector<double> initialPopulation;
	unsigned int statisticsIndividualsPerInstar;

	unsigned int numberOfLociPerTrait;
	unsigned int numberOfAllelesPerLocus;
	unsigned int traitsPerModule;

	unsigned int numberOfChromosomes;
	unsigned int numberOfChiasmasPerChromosome;
	unsigned int numberOfLociPerChromosome;

	std::vector<double> rhoPerModule;
	std::vector<unsigned int> rhoRangePerModule;
	std::vector<double> weightPerLocus;

	double assignedForMolt;
	double betaScaleTank;
	double excessInvestInSize;
	double minRelativeHumidityThreshold;

	double maxEncountersT;

	//Constants for interactions
	double minSizeHunted;
	double maxSizeHunted;
	double minVorHunted;
	double maxVorHunted;

	double minSizeHunter;
	double maxSizeHunter;
	double minVorHunter;
	double maxVorHunter;
	double minSearchAreaHunter;
	double maxSearchAreaHunter;

    double minProbabilityDensityFunction;
	double maxProbabilityDensityFunction;
	double minVorXVor;
	double maxVorXVor;
	double minSpeedRatio;
	double maxSpeedRatio;

	double maxPredationProbability;
	double maxPredationIndividualWetMass;

	float cellEvaluationBiomass;
	float cellEvaluationRisk;
	float cellEvaluationProConspecific;
	float cellEvaluationAntiConspecific;
	float conspecificWeighing;

	GrowthModule* growthModule;

	//Added for new growth_curves
	CustomIndexedVector<Instar, double> instarDevTimeVector;
	double vonBertLinf;
	double vonBertKini;
	double longevitySinceMaturation;
	double reproTimeFactor;
	double devInter;
	double fractSearchExtremeT;
	double fractSpeedExtremeT;
	const Temperature tempFromLab;

	bool eggClutchFromEquation;
	double forClutchMassCoefficient;
	double forClutchMassScale;
	
	double scaleForVoracity;
	double scaleForSearchArea;
	double scaleForSpeed;
	double maxPlasticityKVonBertalanffy;
	double minPlasticityKVonBertalanffy;
	double plasticityDueToConditionVor;
	double plasticityDueToConditionSearch;
	double plasticityDueToConditionSpeed;

	float attackProbability;
	float exposedAttackProbability;
	float killProbability;
	float optimalTemperatureModifier;
	float daysWithoutFoodForMetabolicDownregulation;
	float percentageMetabolicDownregulation;
	float percentageCostForMetabolicDownregulationVoracity;
	float percentageCostForMetabolicDownregulationSearchArea;
	float percentageCostForMetabolicDownregulationSpeed;

	double coefficientForMassA;
	double scaleForMassB;

	double maximumDryMassObserved;

	double coefficientForMassAforMature;
	double scaleForMassBforMature;

	double forDensitiesA;
	double forDensitiesB;
	unsigned int instarsForNextReproduction;
	const SexualType sexualType;
	float sexRatio;
	unsigned int size;
	int femaleMaxReproductionEvents;

	bool eggsPerBatchFromEquation;
	double eggsPerBatch;
	double interceptForEggBatchFromEquation;
	double slopeForEggBatchFromEquation;

	double totFec;

	int maleMaxReproductionEvents;
	double maleReproductionFactor;
	double maleMobility;
	bool surviveWithoutFood;
	float activityUnderPredationRisk;
	double probabilityDeathFromBackground;
	InstarVector<std::vector<ResourceSpecies*>> involvedResourceSpecies;

	bool capitalBreeding;
	unsigned int timeOfReproEventDuringCapitalBreeding;
	unsigned int numberOfCapitalBreeds;

	bool habitatShiftBeforeBreeding;
	bool habitatShiftAfterBreeding;

	double habitatShiftBeforeBreedingFactor;
	double habitatShiftAfterBreedingFactor;

	std::vector<bool> habitatShiftPerInstar;
	double habitatShiftFactor;

	const InstarVector<double> cellSizePerInstar;
	InstarVector<unsigned int> cellDepthPerInstar;

	bool forcePresenceAllResourcesInvolved;
	bool preserveLeftovers;

	double newAAdult;
	double newB;

	double h_enhancement;

	unsigned int maxEncountersPerDay;

	std::vector<Locus*> loci;
	//TODO This has room for efficiency improvements. We could just save the final calculations of the actual positions.
	std::vector<int> randomlyCreatedPositionsForChromosomes;
	
	friend class boost::serialization::access;

	void addEdibleLink(
		Species::Type::TypeValue speciesType, Species* newSpecies, const std::vector<Instar> &predatorInstarVector, 
		const std::vector<Instar> &preyInstarVector, const json& linkValue
	);

	void setCapitalBreeding(const json &info);

	bool checkInsideRestrictedRange() const;

	const double& getBetaScaleTank() const;

	// std::pair<double, double> calculateHunterSizeRanges(const double& minDryMassHunter, const double& maxDryMassHunter) const;

	// void setHunterInteractionRanges();
	
public:
	class Gender {
	public:
		enum GenderValue : unsigned int
		{
			MALE, 
			FEMALE, 
			HERMAPHRODITE
		};

		inline static constexpr size_t size() { return magic_enum::enum_count<GenderValue>(); }
		inline static std::string_view to_string(const GenderValue& gender) { return magic_enum::enum_name(gender); }
		inline static const auto& getEnumValues() { return enumValues; }

		/**
		 * @brief Serialize the Trait object.
		 * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
		 * @param ar The archive to use.
		 * @param version The version of the serialization format.
		 */
		template <class Archive>
		void serialize(Archive &ar, const unsigned int version);

	private:
		static const std::vector<GenderValue> enumValues;

		friend class boost::serialization::access;

		static const std::vector<GenderValue> createEnumValues();
	};

	static const id_type& getAnimalSpeciesCounter();


	AnimalSpecies(const json &info, bool initIndividualsPerDensities, World* const world);
	virtual ~AnimalSpecies();

	bool getForcePresenceAllResourcesInvolved() const { return forcePresenceAllResourcesInvolved; }

	bool getPreserveLeftovers() const { return preserveLeftovers; }

	void calculateCellDepthPerInstar(const Map* const map);

	void generateInvolvedResourceSpecies(const std::vector<const Species*> &existingSpecies);

	const id_type& getAnimalSpeciesId() const;

	const double calculateDryMass(const double& length, const bool mature) const;

	const double calculateDryLength(const double& dryMass, const bool mature) const;

	void initEdibleOntogeneticLink(const std::vector<const Species*>& existingSpecies);

	bool isInsideRestrictedRanges(const CustomIndexedVector<Trait::Type, CustomIndexedVector<TraitDefinitionSection::Elements, double>>& baseTraitElementVector) const;

	const InstarVector<unsigned int>& getCellDepthPerInstar() const;
	const unsigned int getInstarCellDepth(const Instar &instar) const;

	const OntogeneticLink& getEdibleOntogeneticLink(const id_type &preySpeciesId, const Instar &predator, const Instar &prey) const;

	void setEdibleOntogeneticLink(const id_type &preySpeciesId, const Instar &predator, const Instar &prey, const json &linkInfo);

	void addEdibleOntogeneticLink(Species::Type::TypeValue speciesType, Species* newSpecies, const json& ontogeneticLink);

	std::pair<double, double> decomposeMassElements(const double &dryMass, const double &energyTankTraitValue) const;
	std::pair<double, double> decomposeMassElements(const double &dryMass, const double& investment, const double &energyTankTraitValue) const;

	void obtainEdibleSearchParams(World* const world);
	void obtainBreedSearchParams(World* const world);
	void obtainCellEvaluationSearchParams(World* const world);
	void obtainMatureFemalesSearchParams(World* const world);
	void obtainPopulationSearchParams(World* const world);
	void obtainLifeStageSearchParams(World* const world);

	const bool occursHabitatShiftBeforeBreeding() const;
	const bool occursHabitatShiftAfterBreeding() const;

	void generateInstarInvolvedResourceSpecies(
		const Instar &instar, std::vector<ResourceSpecies*> &instarInvolvedResourceSpecies, 
		std::vector<InstarVector<bool>> &alreadyCheckedSpecies
	);

	const EdibleSearchParams& getEdibleSearchParams(const Instar &instar) const;
	const EdibleSearchParams& getBreedSearchParams(const Gender::GenderValue &gender) const;
	const EdibleSearchParams& getCellEvaluationSearchParams(const Instar &instar) const;
	const EdibleSearchParams& getMatureFemalesSearchParams() const;
	const AnimalSearchParams& getPopulationSearchParams() const;
	const AnimalSearchParams& getLifeStageSearchParams(const LifeStage::LifeStageValue &lifeStage) const;

	void checkOntogeneticLinksPreference() const;

	bool eatsAnimalSpecies(const Instar &instar) const;

	double getEdiblePreference(const id_type &preySpeciesId, const Instar &predator) const;
	const double& getEdiblePreference(const id_type &preySpeciesId, const Instar &predator, const Instar &prey) const;
	const double& getEdibleProfitability(const id_type &preySpeciesId, const Instar &predator, const Instar &prey) const;

	bool canEatEdible(const id_type &preySpeciesId, const Instar &predator, const Instar &prey) const;

	const GrowthModule* const getGrowthModule() const;
	GrowthModule* const getMutableGrowthModule();

	const double& getMaximumDryMassObserved() const;
	void calculateK_Density(std::vector<InstarVector<bool>> &checkedAnimalSpecies, std::vector<InstarVector<bool>> &cannibalismAnimalSpecies);
	double calculateInstarK_Density(const Instar &instar, std::vector<InstarVector<bool>> &checkedAnimalSpecies, 
		std::vector<InstarVector<bool>> &cannibalismAnimalSpecies
	);
	void updateMaximumDryMassObserved(double newMaximumDryMass);
  	unsigned int getNumberOfIndividualLevelTraits() const { return individualLevelTraitElements.size(); }
	virtual unsigned int getNumberOfLociPerTrait() const { return numberOfLociPerTrait; };
	virtual unsigned int getTraitsPerModule() const { return traitsPerModule; };
	virtual unsigned int getNumberOfAllelesPerLocus() const { return numberOfAllelesPerLocus; };
	virtual unsigned int getNumberOfChiasmasPerChromosome() const { return numberOfChiasmasPerChromosome; };
	virtual unsigned int getNumberOfLociPerChromosome() const {	return numberOfLociPerChromosome; }
	virtual unsigned int getNumberOfChromosomes() const { return numberOfChromosomes; };
	virtual unsigned int getRhoRangePerModule(int moduleNumber) const { return rhoRangePerModule.at(moduleNumber); };
	virtual const std::vector<unsigned int>& getRhoRangePerModuleVector() const { return rhoRangePerModule; };
	virtual double getRhoPerModule(int moduleNumber) const { return rhoPerModule.at(moduleNumber); };
	virtual const std::vector<double>& getRhoPerModuleVector() const { return rhoPerModule; };
	inline const std::vector<Locus*>& getLoci() const { return loci; }
	inline const std::vector<int>& getRandomlyCreatedPositionsForChromosomes() const { return randomlyCreatedPositionsForChromosomes; }

	const Trait& getTrait(const Trait::Type name) const;
	Trait& getMutableTrait(const Trait::Type name);
	const std::vector<std::pair<Trait::Type, TraitDefinitionSection::Elements>>& getIndividualLevelTraitElements() const;
	const std::vector<Trait::Type>& getTemperatureDependentTraits() const;

	const bool habitatShiftOccurs(const Instar &instar) const;
	const double& getHabitatShiftBeforeBreedingFactor() const;
	const double& getHabitatShiftAfterBreedingFactor() const;
	const double& getHabitatShiftFactor() const;


	virtual bool isMobile() const { return true; };
	virtual void scaleInitialPopulation(double totalPopulationDensity, double initialEcosystemSize);
	inline virtual const InstarVector<double>& getInitialPopulation() const { return initialPopulation; };
	virtual const unsigned int& getStatisticsIndividualsPerInstar() const { return statisticsIndividualsPerInstar; };
	virtual unsigned int getTotalInitialPopulation() const;
	virtual unsigned int getTotalStatisticsInitialPopulation();
	
	
	virtual const std::vector<int>& getPredationEventsOnOtherSpecies() const;
	virtual const HuntingMode& getDefaultHuntingMode() const { return defaultHuntingMode; };

	virtual double getExcessInvestInSize() const { return excessInvestInSize; };
	inline virtual void setExcessInvestInSize(const double& excessInvestInSize) { this->excessInvestInSize = excessInvestInSize; }
	inline virtual const unsigned int& getInstarsForNextReproduction() const { return instarsForNextReproduction; };
	inline virtual void setInstarsForNextReproduction(const unsigned int& instarsForNextReproduction) { this->instarsForNextReproduction = instarsForNextReproduction; }
	void setHabitatShiftBeforeBreeding(const bool newHabitatShiftBeforeBreedingValue, const json &newHabitatShiftBeforeBreedingFactor);
	void setHabitatShiftAfterBreeding(const bool newHabitatShiftAfterBreedingValue, const json &newHabitatShiftAfterBreedingFactor);
	void setHabitatShift(const std::vector<unsigned int> &habitatShift, const json &newHabitatShiftFactor);
	virtual double getCoefficientForMassA() const {return coefficientForMassA;};
	inline virtual void setCoefficientForMassA(const double& coefficientForMassA) { this->coefficientForMassA = coefficientForMassA; }
	virtual double getScaleForMassB() const { return scaleForMassB; };
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
	virtual AnimalSpecies::Gender::GenderValue getRandomGender() const;
	virtual double getMinRelativeHumidityThreshold() const { return minRelativeHumidityThreshold; };
	virtual void setMinRelativeHumidityThreshold(const double& minRelativeHumidityThreshold);
	virtual float getDaysWithoutFoodForMetabolicDownregulation() const { return daysWithoutFoodForMetabolicDownregulation; };
	inline virtual void setDaysWithoutFoodForMetabolicDownregulation(const float& daysWithoutFoodForMetabolicDownregulation) { this->daysWithoutFoodForMetabolicDownregulation = daysWithoutFoodForMetabolicDownregulation; }
	virtual float getPercentageMetabolicDownregulation() const { return percentageMetabolicDownregulation; };
	inline virtual void setPercentageMetabolicDownregulation(const float& percentageMetabolicDownregulation) { this->percentageMetabolicDownregulation = percentageMetabolicDownregulation; }

	const std::vector<std::unordered_map<Species*, std::vector<Instar>>>& getInstarEdibleSpecies(const Instar &instar) const;
	const std::unordered_map<Species*, std::vector<Instar>>& getInstarEdibleAnimalSpecies(const Instar &instar) const;
	const std::unordered_map<Species*, std::vector<Instar>>& getInstarEdibleResourceSpecies(const Instar &instar) const;

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
	virtual double getPlasticityDueToConditionVor() const { return plasticityDueToConditionVor; };
	virtual double getPlasticityDueToConditionSearch() const { return plasticityDueToConditionSearch; };
	virtual double getPlasticityDueToConditionSpeed() const { return plasticityDueToConditionSpeed; };

	virtual double getScaleForVoracity() const { return scaleForVoracity; };
	inline virtual void setScaleForVoracity(const double& scaleForVoracity) { this->scaleForVoracity = scaleForVoracity; }
	virtual double getScaleForSearchArea() const { return scaleForSearchArea; };
	inline virtual void setScaleForSearchArea(const double& scaleForSearchArea) { this->scaleForSearchArea = scaleForSearchArea; }
	virtual double getScaleForSpeed() const { return scaleForSpeed; };
	inline virtual void setScaleForSpeed(const double& scaleForSpeed) { this->scaleForSpeed = scaleForSpeed; }
	virtual const Temperature& getTempFromLab() const { return tempFromLab; };
	virtual double getDevInter() const { return devInter; };
	virtual double getFractSearchExtremeT() const { return fractSearchExtremeT; };
	virtual double getFractSpeedExtremeT() const { return fractSpeedExtremeT; };
	inline virtual const double& getInstarDevTime(const Instar& instar) const { return instarDevTimeVector[instar]; };
	virtual float getActivityUnderPredationRisk() const { return activityUnderPredationRisk; };
	virtual double getMaleMobility() const { return maleMobility; };
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
	
	virtual void setMinSizeHunted(double minSizeHunted) {	this->minSizeHunted = minSizeHunted; };
	virtual void setMaxSizeHunted(double maxSizeHunted) {	this->maxSizeHunted = maxSizeHunted; };
	
	virtual double getMinVorHunted() const { return minVorHunted; };
	virtual double getMaxVorHunted() const { return maxVorHunted; };
	
	virtual void setMinVorHunted(double minVorHunted) {	this->minVorHunted = minVorHunted; };
	virtual void setMaxVorHunted(double maxVorHunted) {	this->maxVorHunted = maxVorHunted; };

	virtual double getMinSizeHunter() const { return minSizeHunter; };
	virtual void setMinSizeHunter(double minSizeHunter) {	this->minSizeHunter = minSizeHunter; };
	virtual double getMaxSizeHunter() const { return maxSizeHunter; };
	virtual void setMaxSizeHunter(double maxSizeHunter) {	this->maxSizeHunter = maxSizeHunter; };

	virtual double getMinVorHunter() const { return minVorHunter; };
	virtual void setMinVorHunter(double minVorHunter) {	this->minVorHunter = minVorHunter; };
	virtual double getMaxVorHunter() const { return maxVorHunter; };
	virtual void setMaxVorHunter(double maxVorHunter) {	this->maxVorHunter = maxVorHunter; };

	virtual double getMinSearchAreaHunter() const { return minSearchAreaHunter; };
	virtual void setMinSearchAreaHunter(double minSearchAreaHunter) {	this->minSearchAreaHunter = minSearchAreaHunter; };
	virtual double getMaxSearchAreaHunter() const { return maxSearchAreaHunter; };
	virtual void setMaxSearchAreaHunter(double maxSearchAreaHunter) {	this->maxSearchAreaHunter = maxSearchAreaHunter; };

	virtual double getMinProbabilityDensityFunction() const { return minProbabilityDensityFunction; };
	virtual void setMinProbabilityDensityFunction(double minProbabilityDensityFunction) {	this->minProbabilityDensityFunction = minProbabilityDensityFunction; };
	virtual double getMaxProbabilityDensityFunction() const { return maxProbabilityDensityFunction; };
	virtual void setMaxProbabilityDensityFunction(double maxProbabilityDensityFunction) {	this->maxProbabilityDensityFunction = maxProbabilityDensityFunction; };

	virtual double getMinVorXVor() const { return minVorXVor; };
	virtual void setMinVorXVor(double minVorXVor) { this->minVorXVor = minVorXVor; };
	virtual double getMaxVorXVor() const { return maxVorXVor; };
	virtual void setMaxVorXVor(double maxVorXVor) { this->maxVorXVor = maxVorXVor; };

	const double& getTotFec() const;

	virtual double getMinSpeedRatio() const { return minSpeedRatio; };
	virtual void setMinSpeedRatio(double minSpeedRatio) {	this->minSpeedRatio = minSpeedRatio; };
	virtual double getMaxSpeedRatio() const { return maxSpeedRatio; };
	virtual void setMaxSpeedRatio(double maxSpeedRatio) {	this->maxSpeedRatio = maxSpeedRatio; };

	virtual float getExperienceInfluencePerDay() const { return experienceInfluencePerDay; };
	virtual void addPredationEventOnOtherSpecies(int predatedSpeciesId) { predationEventsOnOtherSpecies.at(predatedSpeciesId)++; };

	virtual double getProbabilityDeathFromBackground() const { return probabilityDeathFromBackground; };
	virtual double getFemaleMaxReproductionEvents() const { return femaleMaxReproductionEvents; };
	virtual double getMaleMaxReproductionEvents() const { return maleMaxReproductionEvents; };
	
	const bool getEggsPerBatchFromEquation() const;
	const double& getEggsPerBatch() const;
	const double& getInterceptForEggBatchFromEquation() const;
	const double& getSlopeForEggBatchFromEquation() const;

	double calculateEggsPerBatch(const double& individualDryMass) const;

	virtual void setInitialPredationEventsOnOtherSpecies(unsigned int numberOfSpecies);

	virtual void interactionRanges(AnimalInterface& predator, AnimalInterface& prey, double muForPDF, double sigmaForPDF);

	//virtual void initWetBiomassDensitiesPerAge(Temperature temperature, int timeStepsPerDay);
	virtual const std::vector<ResourceSpecies*>& getInstarInvolvedResourceSpecies(const Instar &instar) const;

	const double& getNewAAdult() const;
	const double& getNewB() const;

	const double& getH_Enhancement() const;

	void updateMaxEncountersPerDay(const unsigned int newMaxEncountersPerDay);

	const unsigned int getMaxEncountersPerDay() const;

	void setCorrelationCoeficientRHO(double newRho);
	void setQ10phenology(double q10pheno);
	void setQ10digestion(double q10digest);
	void setAssignedForMolt(const double& ASSIGNED);
	inline void setBetaScaleTank(const double& betaScaleTank) { this->betaScaleTank = betaScaleTank; }

	inline void setMaxEncountersT(const double& maxEncountersT) { this->maxEncountersT = maxEncountersT; }
	void setMaxSearchAreaT( double MAX_SEARCH_AREA_T);


	void calculateMinSizeHunted(double minSizeHunted);
	void calculateMaxSizeHunted(double maxSizeHunted);
	void calculateMinVorHunted(double minVorHunted);
	void calculateMaxVorHunted(double maxVorHunted);
	void calculateMinSizeHunter(double minSizeHunter);
	void calculateMaxSizeHunter(double maxSizeHunter);
	
	
	void calculateMinVorHunter(double minVorHunter);
	void calculateMaxVorHunter(double maxVorHunter);
	
	void calculateMinSearchAreaHunter(double minSearchAreaHunter);
	void calculateMaxSearchAreaHunter(double maxSearchAreaHunter);
	
	
	void calculateMinProbabilityDensityFunction(double minProbabilityDensityFunction);
	void calculateMaxProbabilityDensityFunction(double maxProbabilityDensityFunction);
	
	
	void calculateMinVorXVor(double minVorXVor);
	void calculateMaxVorXVor(double maxVorXVor);
	
	void calculateMinSpeedRatio(double minSpeedRatio);
	void calculateMaxSpeedRatio(double maxSpeedRatio);

	//Added for new growth_curves
	inline void setInstarDevTimeVector(const CustomIndexedVector<Instar, double>& newInstarDevTimeVector){ instarDevTimeVector = newInstarDevTimeVector; }
	void setVonBertLinf(double vonBertLinf);
	void setVonBertKini(double vonBertKini);
	inline void setLongevitySinceMaturation(const double& longevitySinceMaturation) { this->longevitySinceMaturation = longevitySinceMaturation; }
	inline void setReproTimeFactor(const double& reproTimeFactor) { this->reproTimeFactor = reproTimeFactor; }
	inline void setDevInter(const double& devInter) { this->devInter = devInter; }
	inline void setFractSearchExtremeT(const double& fractSearchExtremeT) { this->fractSearchExtremeT = fractSearchExtremeT; }
	inline void setFractSpeedExtremeT(const double& fractSpeedExtremeT) { this->fractSpeedExtremeT = fractSpeedExtremeT; }

	void setMassInfo(const bool& eggClutchFromEquation, const double& forClutchMassCoefficient,
					 const double& forClutchMassScale);




	void setPlasticityDueToConditionVor(const double& plasticityDueToConditionVor);
	void setPlasticityDueToConditionSearch(const double& plasticityDueToConditionSearch);
	void setPlasticityDueToConditionSpeed(const double& plasticityDueToConditionSpeed);

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
	void setEggsPerBatch(const nlohmann::json& info);
	void setMaleMaxReproductionEvents(const int& maleMaxReproductionEvents) { this->maleMaxReproductionEvents = maleMaxReproductionEvents; }

	void setExperienceInfluencePerDay(const float& daysToRememberAbundancesExperienced);
	inline void setMaleMobility(const double& maleMobility) { this->maleMobility = maleMobility; }
	inline void setSurviveWithoutFood(const bool surviveWithoutFood) { this->surviveWithoutFood = surviveWithoutFood; }
	inline void setActivityUnderPredationRisk(const float& activityUnderPredationRisk) { this->activityUnderPredationRisk = activityUnderPredationRisk; }
	void setDryProportionOfMass(double CONTROL_DRY_BODY_MASS);
	inline void setProbabilityDeathFromBackground(const double& probabilityDeathFromBackground) { this->probabilityDeathFromBackground = probabilityDeathFromBackground; }

	
	const bool hasCapitalBreeding() const;

	const unsigned int getTimeOfReproEventDuringCapitalBreeding() const;
	const unsigned int getNumberOfCapitalBreeds() const;






	void setInitialPopulation(const std::vector<unsigned int>& initialPopulation);
	inline void setStatisticsIndividualsPerInstar(const unsigned int& statisticsIndividualsPerInstar) { this->statisticsIndividualsPerInstar = statisticsIndividualsPerInstar; }


	void setTraits(const std::unordered_map<std::string,nlohmann::json>& traitsInfo);


	void setNumberOfLinks(const unsigned int numberOfLinksAsPredator, const unsigned int numberOfLinksAsPrey);
	const unsigned int getNumberOfLinksAsPredator() const;
	const unsigned int getNumberOfLinksAsPrey() const;


	virtual double getMaxPredationProbability();
	void setMaxPredationProbability(double maxPredationProbability);

	virtual double getMaxPredationIndividualWetMass();
	void setMaxPredationIndividualWetMass(double maxPredationIndividualWetMass);

	inline void setCellEvaluationBiomass(const float& cellEvaluationBiomass) { this->cellEvaluationBiomass = cellEvaluationBiomass; }
	inline void setCellEvaluationRisk(const float& cellEvaluationRisk) { this->cellEvaluationRisk = cellEvaluationRisk; }
	inline void setCellEvaluationProConspecific(const float& cellEvaluationProConspecific) { this->cellEvaluationProConspecific = cellEvaluationProConspecific; }
	inline void setCellEvaluationAntiConspecific(const float& cellEvaluationAntiConspecific) { this->cellEvaluationAntiConspecific = cellEvaluationAntiConspecific; }
	inline void setConspecificWeighing(const float& conspecificWeighing) { this->conspecificWeighing = conspecificWeighing; }

	inline virtual const CustomIndexedVector<Instar, double>& getInstarDevTimeVector() const { return instarDevTimeVector; }

	/**
     * @brief Serialize the Trait object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* ANIMAL_SPECIES_H_ */
