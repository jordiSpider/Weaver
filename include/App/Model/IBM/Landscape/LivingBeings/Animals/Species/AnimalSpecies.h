/**
 * @file AnimalSpecies.h
 * @brief Defines the AnimalSpecies class, representing a species of animals with associated growth, genetics, decisions, and interactions.
 */

#ifndef ANIMAL_SPECIES_H_
#define ANIMAL_SPECIES_H_


#include <vector>
#include <deque>
#include <unordered_map>
#include <cmath>
#include <numeric>
#include <nlohmann/json.hpp>


#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/unordered_map.hpp>


#include "rapidcsv.h"


#include "App/Model/IBM/Landscape/LivingBeings/Species/Species.h"
#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "Misc/OntogeneticLink.h"
#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Maths/Random.h"
#include "App/Model/IBM/Maths/MathFunctions.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/AnimalSpeciesDecisions.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Growth/AnimalSpeciesGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/AnimalSpeciesGenetics.h"
#include "Misc/CustomIndexedVector.h"
#include "Misc/EnumClass.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Gender.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/HuntingMode.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/SexualType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/EdibleSearchParams.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/AnimalSearchParams.h"


class Map;

class Animal;


/**
 * @defgroup BehavioralParameters Behavioral Parameters
 * @brief Methods related to behavioral traits and conspecific search rules.
 *
 * This group contains all methods that compute or set parameters governing the
 * behavior of animals in the simulation. This includes mate search, population
 * interactions, life stage detection, and general conspecific interactions.
 */

/**
 * @defgroup PopulationParameters Population Parameters
 * @brief Methods related to population-level interactions and conspecific detection.
 *
 * This group contains all methods that compute or set parameters governing
 * interactions with conspecifics at the population level, including density
 * estimation, aggregation, dispersal decisions, and avoidance of overcrowding.
 */

/**
 * @defgroup LifeStageParameters Life Stage Parameters
 * @brief Methods related to detecting and interacting with individuals at different life stages.
 *
 * This group includes methods that compute search parameters or behavioral
 * rules depending on the life stage of the individuals. It influences interactions
 * such as predation, competition, cannibalism, facilitation, and kin recognition.
 */


/**
 * @class AnimalSpecies
 * @brief Represents a specific animal species in the ecosystem.
 * 
 * Contains information about genetics, growth, decisions, ontogenetic links, 
 * prey and predator search parameters, and reproductive strategies.
 */
class AnimalSpecies: public Species
{
public:
	/**
     * @brief Default constructor.
     */
	AnimalSpecies();

	/**
     * @brief Constructor to initialize an animal species.
     * 
     * @param speciesId Unique identifier within the Species system.
     * @param animalSpeciesId Unique identifier for this AnimalSpecies.
     * @param info JSON object containing species-specific parameters.
     * @param timeStepsPerDay Number of simulation time steps per day.
     * @param pdfThreshold Threshold for probability density functions.
     * @param numberOfExistingSpecies Number of existing species for initialization purposes.
     */
	AnimalSpecies(const AnimalSpecies::ID& speciesId, const AnimalSpeciesID& animalSpeciesId, const nlohmann::json &info, const PreciseDouble& timeStepsPerDay, const PreciseDouble& pdfThreshold, const size_t numberOfExistingSpecies);
	
	/**
     * @brief Destructor.
     */
	virtual ~AnimalSpecies();

	// Deleted copy constructor
	AnimalSpecies(const AnimalSpecies&) = delete;

	// Deleted assignment operator
	AnimalSpecies& operator=(const AnimalSpecies&) = delete;

	/**
	 * @defgroup GeneticsAccessors Genetics Accessors
	 * @brief Accessors for reading and modifying species-level genetic configuration.
	 * @{
	 */

	/**
	 * @brief Returns the immutable genetics block for this species.
	 */
	const AnimalSpeciesGenetics& getGenetics() const;

	/**
	 * @brief Returns a mutable reference to the genetics block of this species.
	 */
	AnimalSpeciesGenetics& getMutableGenetics();

	///@}

	/**
	 * @defgroup ResourceHandlingOptions Resource Handling Options
	 * @brief Configuration for resource consumption and leftover handling.
	 * @{
	 */

	/**
	 * @brief Indicates whether the species forces presence of all resources involved.
	 */
	bool getForcePresenceAllResourcesInvolved() const;

	/**
	 * @brief Returns whether resource leftovers should be preserved after feeding.
	 */
	bool getPreserveLeftovers() const;

	///@}

	/**
	 * @defgroup GrowthAndInstarInformation Growth and Instar Information
	 * @brief Methods related to growth curves, instar transitions and instar-dependent parameters.
	 * @{
	 */

	/**
	 * @brief Computes cell depth per instar using the provided map.
	 */
	void calculateCellDepthPerInstar(const Map* const map);

	/**
	 * @brief Returns the mapping of instars to their cell depths.
	 */
	const CustomIndexedVector<Instar, unsigned int>& getCellDepthPerInstar() const;

	/**
	 * @brief Returns the cell depth corresponding to a given instar.
	 */
	unsigned int getInstarCellDepth(const Instar &instar) const;

	/**
	 * @brief Returns the immutable growth blueprint object for the species.
	 */
	const AnimalSpeciesGrowth& getGrowthBuildingBlock() const;

	/**
	 * @brief Returns a mutable reference to the growth blueprint object.
	 */
	AnimalSpeciesGrowth& getMutableGrowthBuildingBlock();

	///@}

	/**
	 * @defgroup OntogeneticLinks Ontogenetic Links
	 * @brief Species relationships across instars and prey–predator developmental stages.
	 * @{
	 */

	/**
	 * @brief Returns the ontogenetic links available to a predator instar.
	 */
	const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, OntogeneticLink>>& getOntogeneticLinksPerInstar(const Instar &predatorInstar) const;
	
	/**
	 * @brief Retrieves the ontogenetic link for a given predator–prey instar pair.
	 */
	const OntogeneticLink& getEdibleOntogeneticLink(const Species::ID &preySpeciesId, const Instar &predatorInstar, const Instar &preyInstar) const;
	
	/**
	 * @brief Initializes ontogenetic link structures using external tables of preferences and profitability.
	 */
	void setOntogeneticLinks(View* view, const std::vector<Species*>& existingSpecies, rapidcsv::Document& ontogeneticLinksPreference, rapidcsv::Document& ontogeneticLinksProfitability);
	
	///@}

	/**
	 * @defgroup SearchParameters Search Parameters
	 * @brief Parameters used to evaluate search ranges, prey availability and predator interactions.
	 * @{
	 */

	/**
	 * @brief Computes search parameters for prey, predators and conspecifics.
	 */
	void obtainPreyAndPredatorSearchParams(const std::vector<Species*>& existingSpecies, const std::vector<AnimalSpecies*>& existingAnimalSpecies, const std::vector<ResourceSpecies*>& existingResourceSpecies);
	
	/**
	 * @brief Returns search parameters for prey detection for the given instar.
	 */
	const EdibleSearchParams& getPreySearchParams(const Instar &instar) const;
	
	/**
	 * @brief Returns predation search parameters for the given instar.
	 */
	const AnimalSearchParams& getPredatorSearchParams(const Instar &instar) const;
	
	/**
	 * @brief Returns search parameters for conspecific detection.
	 */
	const AnimalSearchParams& getConspecificSearchParams() const;
	
	/**
	 * @brief Returns search parameters for locating mates of the given gender.
	 */
	const AnimalSearchParams& getBreedSearchParams(const Gender &gender) const;
	
	/**
	 * @brief Returns search parameters for detecting mature females.
	 */
	const AnimalSearchParams& getMatureFemalesSearchParams() const;
	
	/**
	 * @brief Returns search parameters related to population-level detection.
	 */
	const AnimalSearchParams& getPopulationSearchParams() const;
	
	/**
	 * @brief Returns search parameters for detecting individuals in a given life stage.
	 */
	const AnimalSearchParams& getLifeStageSearchParams(const LifeStage &lifeStage) const;

	///@}

	/**
	 * @defgroup ReproductionAndHabitatShift Reproduction and Habitat Shift
	 * @brief Controls habitat transitions linked to reproduction events.
	 * @{
	 */

	/**
	 * @brief Returns true if the species shifts habitat before breeding.
	 */
	bool occursHabitatShiftBeforeBreeding() const;
	
	/**
	 * @brief Returns true if the species shifts habitat after breeding.
	 */
	bool occursHabitatShiftAfterBreeding() const;
	
	/**
	 * @brief Returns the habitat shift factor applied before breeding.
	 */
	const PreciseDouble& getHabitatShiftBeforeBreedingFactor() const;
	
	/**
	 * @brief Returns the habitat shift factor applied after breeding.
	 */
	const PreciseDouble& getHabitatShiftAfterBreedingFactor() const;
	
	/**
	 * @brief Sets whether habitat shift occurs before breeding and updates its factor.
	 */
	void setHabitatShiftBeforeBreeding(const bool newHabitatShiftBeforeBreedingValue, const nlohmann::json &newHabitatShiftBeforeBreedingFactor);
	
	/**
	 * @brief Sets whether habitat shift occurs after breeding and updates its factor.
	 */
	void setHabitatShiftAfterBreeding(const bool newHabitatShiftAfterBreedingValue, const nlohmann::json &newHabitatShiftAfterBreedingFactor);
	
	///@}

	/**
	 * @defgroup Predation Predation
	 * @brief Handling of predator–prey interactions and consumption constraints.
	 * @{
	 */

	/**
	 * @brief Returns true if individuals of the given instar are carnivorous.
	 */
	bool eatsAnimalSpecies(const Instar &instar) const;
	
	/**
	 * @brief Returns profitability of eating a prey instar by a predator instar.
	 */
	const PreciseDouble& getEdibleProfitability(const Species::ID &preySpeciesId, const Instar &predatorInstar, const Instar &preyInstar) const;
	
	/**
	 * @brief Determines if a predator instar can eat the given prey instar.
	 */
	bool canEatEdible(const Species::ID &preySpeciesId, const Instar &predatorInstar, const Instar &preyInstar) const;
	
	/**
	 * @brief Updates the maximum interaction area for individuals of a given instar.
	 */
	void updateMaximumInteractionArea(const Instar& instar, const PreciseDouble& newValue);
	
	/**
	 * @brief Returns the maximum interaction area for individuals of an instar.
	 */
	const PreciseDouble& getMaximumInteractionArea(const Instar& instar) const;
	
	/**
	 * @brief Registers a predation event on another species.
	 */
	void addPredationEventOnOtherSpecies(Species::ID predatedSpeciesId);
	
	/**
	 * @brief Returns total predation events on a specific species.
	 */
	const unsigned int& getPredationEventsOnOtherSpecies(Species::ID predatedSpeciesId) const;
	
	///@}

	/**
	 * @defgroup MobilityTraits Mobility and Behavioral Traits
	 * @brief Behavioral, physiological and movement-related parameters.
	 * @{
	 */

	/**
	 * @brief Returns whether this species is mobile.
	 */
	virtual bool isMobile() const { return true; };

	/**
	 * @brief Returns plasticity factor for search area influenced by condition.
	 */
	virtual PreciseDouble getPlasticityDueToConditionSearch() const { return plasticityDueToConditionSearch; };

	/**
	 * @brief Returns plasticity factor for speed influenced by condition.
	 */
	virtual PreciseDouble getPlasticityDueToConditionSpeed() const { return plasticityDueToConditionSpeed; };

	/**
	 * @brief Returns lab reference temperature.
	 */
	virtual const Temperature& getTempFromLab() const { return tempFromLab; };

	/**
	 * @brief Returns activity level under predation risk.
	 */
	virtual float getActivityUnderPredationRisk() const { return activityUnderPredationRisk; };

	/**
	 * @brief Returns male mobility parameter.
	 */
	virtual PreciseDouble getMaleMobility() const { return maleMobility; };

	/**
	 * @brief Returns true if the species can survive without food.
	 */
	virtual bool isSurviveWithoutFood() const { return surviveWithoutFood; };

	///@}

	/**
	 * @defgroup PopulationAndStatistics Population and Statistics
	 * @brief Methods for tracking populations, variability and stability metrics.
	 * @{
	 */

	/**
	 * @brief Returns number of individuals per instar.
	 */
	inline virtual const CustomIndexedVector<Instar, unsigned int>& getIndividualsPerInstar() const { return individualsPerInstar; };
	
	/**
	 * @brief Returns the statistic representing individuals per instar.
	 */
	virtual const unsigned int& getStatisticsIndividualsPerInstar() const { return statisticsIndividualsPerInstar; };
	
	/**
	 * @brief Returns the total initial population.
	 */
	virtual unsigned int getTotalInitialPopulation() const;
	
	/**
	 * @brief Returns the initial population size used for statistics.
	 */
	virtual unsigned int getTotalInitialStatisticsPopulation();
	
	/**
	 * @brief Returns current population size.
	 */
	unsigned int getPopulation() const;
	
	/**
	 * @brief Increases population by one.
	 */
	void increasePopulation();
	
	/**
	 * @brief Decreases population by one.
	 */
	void decreasePopulation();
	
	/**
	 * @brief Updates history of population values for stability analysis.
	 */
	void updatePopulationHistory(const TimeStep& stabilityCalculationInterval);
	
	/**
	 * @brief Computes the coefficient of variation of population history.
	 */
	void calculateCoefficientOfVariation();
	
	/**
	 * @brief Returns the coefficient of variation.
	 */
	const PreciseDouble& getCoefficientOfVariation() const;

	///@}

	/**
	 * @defgroup ReproductiveTraits Reproductive Traits
	 * @brief Parameters related to fecundity, sex ratio and sexual system.
	 * @{
	 */

	/**
	 * @brief Returns the sexual type of the species (diploid, haplodiploid, asexual).
	 */
	virtual const SexualType& getSexualType() const { return sexualType; };

	/**
	 * @brief Generates a random gender according to species' sex ratio.
	 */
	virtual Gender getRandomGender() const;
	
	/**
	 * @brief Returns the sex ratio of the species.
	 */
	float getSexRatio();

	/**
	 * @brief Sets the sex ratio of the species.
	 */
	void setSexRatio(const float& sexRatioValue);
	
	/**
	 * @brief Returns maximum reproductive events for females.
	 */
	virtual unsigned int getFemaleMaxReproductionEvents() const { return femaleMaxReproductionEvents; };
	
	/**
	 * @brief Returns maximum reproductive events for males.
	 */
	virtual unsigned int getMaleMaxReproductionEvents() const { return maleMaxReproductionEvents; };
	
	/**
	 * @brief Sets maximum reproductive events for females.
	 */
	void setFemaleMaxReproductionEvents(const unsigned int& femaleMaxReproductionEventsValue);
	
	/**
	 * @brief Sets maximum reproductive events for males.
	 */
	void setMaleMaxReproductionEvents(const unsigned int& maleMaxReproductionEventsValue);
	
	/**
	 * @brief Returns the total fecundity factor.
	 */
	const PreciseDouble& getTotFec() const;

	///@}

	/**
	 * @defgroup MiscSpeciesParameters Miscellaneous Species Parameters
	 * @brief Additional ecological or physiological parameters not covered by other groups.
	 * @{
	 */

	/**
	 * @brief Returns Yodzis parameter A for metabolic scaling.
	 */
	const PreciseDouble& getYodzisA() const;

	/**
	 * @brief Returns Yodzis parameter B for metabolic scaling.
	 */
	const PreciseDouble& getYodzisB() const;

	/**
	 * @brief Returns the default hunting mode for the species.
	 */
	virtual const HuntingMode& getDefaultHuntingMode() const { return defaultHuntingMode; };

	/**
	 * @brief Returns true if handling time should be activated.
	 */
	bool getActivatedHandling() const;

	///@}

	/** @defgroup SpeciesResourceGeneration Resource Generation and Involvement
	 *  @brief Methods that compute which resources or species affect each instar.
	 *  @{
	 */

	/// Generates involved resource species for the whole species.
	void generateInvolvedResourceSpecies(const std::vector<Species*> &existingSpecies, const std::vector<AnimalSpecies*>& existingAnimalSpecies, CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, std::vector<ResourceSpecies::ResourceID>>>& involvedResourceSpecies) const;

	/// Returns species identifier.
	const AnimalSpeciesID& getAnimalSpeciesId() const;

	/// Initializes genetic files.
	void initializeGeneticFiles(const std::filesystem::path& geneticsFolder);

	/// Generates involved-resource list for a single instar.
	void generateInstarInvolvedResourceSpecies(
		const std::vector<Species*>& existingSpecies, const Instar &instar, 
		std::vector<ResourceSpecies::ResourceID>& involvedResourceSpecies, 
		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, bool>> &alreadyCheckedSpecies
	) const;

	/// Returns species type.
	SpeciesType getType() const;

	/// @}

	/** @defgroup SpeciesDecisionBlocks Decision Modules
	 *  @brief Decision-making building blocks.
	 *  @{
	 */

	/// Returns immutable decisions block.
	const AnimalSpeciesDecisions* getDecisionsBuildingBlock() const;

	/// Returns mutable decisions block.
	AnimalSpeciesDecisions* getMutableDecisionsBuildingBlock();

	/// @}

	/** @defgroup SpeciesModifiers Setters and Change Operations
	 *  @brief Setters for mobility, metabolism, mortality and population.
	 *  @{
	 */

	/// Adds an animal species record.
	void addAnimalSpecies();

	/// Adds a resource species record.
	void addResourceSpecies();

	/**
	 * @brief Generates the initial genomes pool for this species.
	 * 
	 * @param initialPopulation Initial population per instar.
	 * @param initialGenomesPool Output vector to be filled with initial genomes.
	 */
	void generateInitialGenomesPool(const CustomIndexedVector<Instar, unsigned int>& population, std::vector<Genome>& initialGenomesPool);
	
	/// Returns humidity threshold per instar.
	virtual PreciseDouble getMinRelativeHumidityThreshold(const Instar &instar) const { return minRelativeHumidityThreshold[instar]; };
	
	/// Sets humidity thresholds.
	virtual void setMinRelativeHumidityThreshold(const std::vector<double>& newMinRelativeHumidityThreshold);
	
	/// Returns time without food before downregulation.
	TimeStep getTimeStepsWithoutFoodForMetabolicDownregulation() const { return timeStepsWithoutFoodForMetabolicDownregulation; };
	
	/// Sets time without food before downregulation.
	virtual void setTimeStepsWithoutFoodForMetabolicDownregulation(const PreciseDouble& daysWithoutFoodForMetabolicDownregulation, const PreciseDouble& timeStepsPerDay);
	
	/// Returns percentage metabolic downregulation.
	virtual float getPercentageMetabolicDownregulation() const { return percentageMetabolicDownregulation; };
	
	/// Sets percentage metabolic downregulation.
	inline virtual void setPercentageMetabolicDownregulation(const float& percentageMetabolicDownregulationValue) { percentageMetabolicDownregulation = percentageMetabolicDownregulationValue; }

	/// Returns instar-edible species structure.
	const CustomIndexedVector<SpeciesType, std::unordered_map<Species::ID, std::vector<Instar>>>& getInstarEdibleSpecies(const Instar &instar) const;
	
	/// Returns instar-edible animal species.
	const std::unordered_map<Species::ID, std::vector<Instar>>& getInstarEdibleAnimalSpecies(const Instar &instar) const;
	
	/// Returns instar-edible resource species.
	const std::unordered_map<Species::ID, std::vector<Instar>>& getInstarEdibleResourceSpecies(const Instar &instar) const;

	/// Returns cost for metabolic downregulation affecting voracity.
	virtual float getPercentageCostForMetabolicDownregulationVoracity() const { return percentageCostForMetabolicDownregulationVoracity; };
	
	/// Returns cost for downregulation affecting search area.
	virtual float getPercentageCostForMetabolicDownregulationSearchArea() const { return percentageCostForMetabolicDownregulationSearchArea; };
	
	/// Returns cost for downregulation affecting speed.
	virtual float getPercentageCostForMetabolicDownregulationSpeed() const { return percentageCostForMetabolicDownregulationSpeed; };

 	/// Returns probability of background mortality.
	virtual const PreciseDouble& getProbabilityDeathFromBackground(const Instar& instar) const { return probabilityDeathFromBackground[instar]; };
	
	/// Computes interaction ranges between predator and prey.
	virtual void interactionRanges(Animal& predator, Animal& prey);

	/// Updates max predation encounters per day.
	void updateMaximumPredationEncountersPerDay(const size_t& predationEncountersPerDay);

	/// Returns max predation encounters per day.
	const size_t& getMaximumPredationEncountersPerDay() const;

	/// Sets search plasticity.
	void setPlasticityDueToConditionSearch(const PreciseDouble& plasticityDueToConditionSearchValue);

	/// Sets speed plasticity.
	void setPlasticityDueToConditionSpeed(const PreciseDouble& plasticityDueToConditionSpeedValue);

	/// Sets cost for metabolic downregulation (voracity).
	void setPercentageCostForMetabolicDownregulationVoracity(const float& percentageCostForMetabolicDownregulationVoracityValue);
	
	/// Sets cost for metabolic downregulation (search area).
	void setPercentageCostForMetabolicDownregulationSearchArea(const float& percentageCostForMetabolicDownregulationSearchAreaValue);
	
	/// Sets cost for metabolic downregulation (speed).
	void setPercentageCostForMetabolicDownregulationSpeed(const float& percentageCostForMetabolicDownregulationSpeedValue);

	/// Sets male mobility parameter.
	void setMaleMobility(const PreciseDouble& maleMobilityValue);
	
	/// Sets survival-without-food option.
	void setSurviveWithoutFood(const bool surviveWithoutFoodValue);
	
	/// Sets activity under predation risk.
	void setActivityUnderPredationRisk(const float& activityUnderPredationRiskValue);
	
	/// Sets background mortality probability.
	void setProbabilityDeathFromBackground(const std::vector<double>& newProbabilityDeathFromBackground);

	/// Sets individuals per instar.
	void setIndividualsPerInstar(const std::vector<unsigned int>& newIndividualsPerInstar);
	
	/// Sets statistics-individuals-per-instar.
	void setStatisticsIndividualsPerInstar(const unsigned int& statisticsIndividualsPerInstarValue);

	/// @}

	/**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
	/** @defgroup SpeciesGeneticsAttributes Genetic and Growth Attributes
	 *  @brief Attributes storing genetic configuration and growth parameters.
	 *  @{
	 */

	/// Genetic traits and inheritance rules for the species.
	AnimalSpeciesGenetics genetics;

	/// Growth-related parameters, including instar-duration functions and growth models.
	AnimalSpeciesGrowth animalSpeciesGrowth;

	/// @}

	/** @defgroup SpeciesDecisionAttributes Decision-Making Attributes
	 *  @brief Decision blocks defining behavioural rules and action-selection criteria.
	 *  @{
	 */

	/// Behavioural decision module controlling movement, feeding and risk response.
	AnimalSpeciesDecisions decisions;

	/// @}

	/** @defgroup SpeciesIdentityAttributes Species Identity Attributes
	 *  @brief Attributes identifying the species and its ecological strategy.
	 *  @{
	 */

	/// Unique identifier for this animal species.
	AnimalSpeciesID animalSpeciesId;

	/// Default hunting mode (e.g., sit-and-wait, grazer).
	HuntingMode defaultHuntingMode;

	/// @}

	/** @defgroup SpeciesEdibilityAttributes Edibility and Ontogeny Attributes
	 *  @brief Structures defining what each instar can eat and ontogenetic edibility links.
	 *  @{
	 */

	/// Species that are edible for each instar, categorized by prey type and instar.
	CustomIndexedVector<Instar, CustomIndexedVector<SpeciesType, std::unordered_map<ID, std::vector<Instar>>>> edibleSpecies;

	/// Ontogenetic edibility links for each predator instar and prey species/instar pair.
	CustomIndexedVector<Instar, CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, OntogeneticLink>>> edibleOntogeneticLink;

	/// Number of predation events performed on other species.
	CustomIndexedVector<Species::ID, unsigned int> predationEventsOnOtherSpecies;

	/// @}

	/** @defgroup SpeciesSearchAttributes Search Parameters
	 *  @brief Search radii and probabilities for prey, predators, conspecifics and reproduction.
	 *  @{
	 */
	
	/// Search parameters when looking for prey at every instar.
	CustomIndexedVector<Instar, EdibleSearchParams> preySearchParams;

	/// Search parameters when detecting predators at every instar.
	CustomIndexedVector<Instar, AnimalSearchParams> predatorSearchParams;

	/// Search parameters for conspecific encounters.
	AnimalSearchParams conspecificSearchParams;

	/// Search parameters associated to each gender during mating.
	CustomIndexedVector<Gender, AnimalSearchParams> breedSearchParams;

	/// Search parameters for locating mature females.
	AnimalSearchParams* matureFemalesSearchParams;

	/// Search parameters for population-level interactions.
	AnimalSearchParams* populationSearchParams;

	/// Search parameters depending on the individual's life stage.
	CustomIndexedVector<LifeStage, AnimalSearchParams> lifeStageSearchParams;

	/// @}

	/** @defgroup SpeciesPopulationAttributes Population Attributes
	 *  @brief Population counts, history and statistical indicators.
	 *  @{
	 */

	/// Historical record of population size across time steps.
	std::deque<unsigned int> populationHistory;

	/// Current number of individuals in the population.
	unsigned int population;

	/// Population variability metric (coefficient of variation).
	PreciseDouble coefficientOfVariation;

	/// Whether handling-time processes are activated.
	bool activatedHandling;

	/// Number of individuals per instar.
	CustomIndexedVector<Instar, unsigned int> individualsPerInstar;

	/// Number of individuals per instar used for statistics.
	unsigned int statisticsIndividualsPerInstar;

	/// @}

	/** @defgroup SpeciesMetabolismAttributes Metabolism, Plasticity and Environmental Tolerance
	 *  @brief Physiological parameters controlling metabolism, humidity tolerance and plasticity.
	 *  @{
	 */

	/// Minimum humidity threshold per instar for survival.
	CustomIndexedVector<Instar, PreciseDouble> minRelativeHumidityThreshold;
	
	/// Plasticity of search behavior depending on internal condition.
	PreciseDouble plasticityDueToConditionSearch;

	/// Plasticity of movement speed depending on internal condition.
	PreciseDouble plasticityDueToConditionSpeed;

	/// Time without food before metabolic downregulation begins.
	TimeStep timeStepsWithoutFoodForMetabolicDownregulation;

	/// Percentage of metabolic downregulation under starvation.
	float percentageMetabolicDownregulation;

	/// Cost of metabolic downregulation affecting voracity.
	float percentageCostForMetabolicDownregulationVoracity;

	/// Cost of metabolic downregulation reducing search area.
	float percentageCostForMetabolicDownregulationSearchArea;

	/// Cost of metabolic downregulation reducing movement speed.
	float percentageCostForMetabolicDownregulationSpeed;

	/// @}

	/** @defgroup SpeciesReproductionAttributes Reproduction Attributes
	 *  @brief Sexual structure, fecundity, reproduction limits and gender distribution.
	 *  @{
	 */

	/// Sexual reproduction type (diploid, haplodiploid, asexual).
	SexualType sexualType;

	/// Fraction of females at birth or emergence.
	float sexRatio;

	/// Maximum number of reproduction events for females.
	unsigned int femaleMaxReproductionEvents;

	/// Total lifetime fecundity.
	PreciseDouble totFec;

	/// Maximum number of reproduction events for males.
	unsigned int maleMaxReproductionEvents;

	/// Mobility coefficient of males.
	PreciseDouble maleMobility;

	/// Whether individuals can survive without food.
	bool surviveWithoutFood;

	/// Activity multiplier when under predation risk.
	float activityUnderPredationRisk;

	/// @}

	/** @defgroup SpeciesHabitatAttributes Habitat Use and Spatial Behaviour
	 *  @brief Factors influencing habitat shift, depth usage, and instar-specific pore use.
	 *  @{
	 */

	/// Probability of death from background causes for each instar.
	CustomIndexedVector<Instar, PreciseDouble> probabilityDeathFromBackground;

	/// Whether habitat shift occurs prior to breeding.
	bool habitatShiftBeforeBreeding;

	/// Whether habitat shift occurs after breeding.
	bool habitatShiftAfterBreeding;

	/// Factor applied during habitat shift before breeding.
	PreciseDouble habitatShiftBeforeBreedingFactor;

	/// Factor applied during habitat shift after breeding.
	PreciseDouble habitatShiftAfterBreedingFactor;

	/// Pore usage per instar (soil or substrate penetration).
	CustomIndexedVector<Instar, PreciseDouble> poreUsePerInstar;

	/// Depth per instar inside the spatial cell.
	CustomIndexedVector<Instar, unsigned int> cellDepthPerInstar;

	/// @}

	/** @defgroup SpeciesResourceHandlingAttributes Resource Handling Behaviour
	 *  @brief Attributes controlling resource presence requirements and leftover behaviour.
	 *  @{
	 */

	/// Whether all required resources must be present simultaneously.
	bool forcePresenceAllResourcesInvolved;

	/// Whether unused resource leftovers are preserved.
	bool preserveLeftovers;

	/// @}

	/** @defgroup SpeciesMiscAttributes Miscellaneous Ecophysiological Attributes
	 *  @brief Additional functional parameters not belonging to previous groups.
	 *  @{
	 */

	/// Yodzis–Innes metabolic parameter A.
	PreciseDouble YodzisA;

	/// Yodzis–Innes metabolic parameter B.
	PreciseDouble YodzisB;

	/// Maximum number of predation encounters allowed per da
	size_t maximumPredationEncountersPerDay;

	/// Maximum interaction area per instar.
	CustomIndexedVector<Instar, PreciseDouble> maximumInteractionArea;

	/// Laboratory-reference temperature.
	Temperature tempFromLab;

	/// @}


	/**
	 * @ingroup BehavioralParameters
	 * @brief Computes and assigns the search parameters used for locating potential mates.
	 *
	 * This method derives the behavioral rules for mate searching based on the species'
	 * sexual system, mobility, detection capabilities, and ecological constraints.
	 * The parameters are stored internally and later used during reproductive decision making.
	 */
	void obtainBreedSearchParams();

	/**
	 * @ingroup BehavioralParameters
	 * @brief Computes the search parameters for locating mature females.
	 *
	 * This method determines how individuals—typically males—locate receptive females.
	 * It integrates species-specific sensory capabilities, ecological context, and
	 * behavioral rules encoded in the model.
	 */
	void obtainMatureFemalesSearchParams();

	/**
	 * @ingroup PopulationParameters
	 * @brief Computes and stores the search parameters for detecting conspecific individuals.
	 *
	 * These parameters are used in processes involving population density estimation,
	 * aggregation, dispersal decisions, and avoidance of overcrowding.
	 */
	void obtainPopulationSearchParams();

	/**
	 * @ingroup LifeStageParameters
	 * @brief Computes the search parameters associated with detecting individuals in different life stages.
	 *
	 * These parameters influence interactions such as predation, competition,
	 * cannibalism, facilitation, and kin recognition, depending on the species’ ecology.
	 */
	void obtainLifeStageSearchParams();

	/**
	 * @ingroup BehavioralParameters
	 * @brief Computes overall conspecific interaction search parameters.
	 *
	 * These parameters are used for interactions that do not fit into breeding or population
	 * estimation, such as cooperative behaviors, territoriality, or social signaling.
	 */
	void obtainConspecificSearchParams();

	/**
	 * @ingroup MorphologyAndPhysiology
	 * @brief Sets the pore usage fraction for each instar.
	 *
	 * @param newPoreUsePerInstar Vector containing the new pore use value for each instar.
	 *
	 * Pore use affects how individuals interact physically with the environment,
	 * influencing traits such as diffusion-based respiration, water balance,
	 * and habitat occupancy within porous substrates.
	 *
	 * The size of the vector must match the number of instars defined for the species.
	 */
	void setPoreUsePerInstar(const std::vector<double>& newPoreUsePerInstar);
};

#endif /* ANIMAL_SPECIES_H_ */
