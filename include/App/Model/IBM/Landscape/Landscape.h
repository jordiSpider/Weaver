/**
* @file Landscape.h
* @brief Doxygen-documented declaration of the Landscape class and related serialization helpers.
*
* This header declares the abstract base class Landscape which encapsulates the
* simulation landscape, species, resources, and time-stepping logic used by the
* Weaver ecosystem simulator.
*/

#ifndef LANDSCAPE_H_
#define LANDSCAPE_H_

#include <vector>
#include <set>
#include <ios>
#include <iomanip>
#include <typeinfo>
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <memory>
#include <chrono>
#include <numeric>
#include <unordered_set>
#include <nlohmann/json.hpp>


#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>



#include <fstream>
#include <ostream>
#include <filesystem>
#include <sstream>
#include <algorithm>

#include "rapidcsv.h"


#include "App/Model/IBM/Maths/GaussianDistribution.h"
#include "App/Model/IBM/Maths/Random.h"
#include "Misc/Types.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"
#include "App/Model/IBM/Landscape/LivingBeings/Edible.h"
#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResource.h"
#include "Misc/Utilities.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "Misc/Coordinate3D.h"
#include "App/Model/IBM/Landscape/Map/Map.h"
#include "Misc/CustomIndexedVector.h"
#include "Misc/SerializablePath.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/MoistureSource.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/ResourceSource.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/ObstacleSource.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/HabitatDomainSource.h"
#include "Misc/EnumClass.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"
#include "App/View/View.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "App/Model/IBM/Landscape/LivingBeings/EdibleID.h"
#include "App/Model/IBM/Maths/Dynamics/NonMassBased/NonDynamics.h"
#include "App/Model/IBM/Maths/PreciseDouble.h"
#include "Misc/ProgressBar.h"
#include "App/Model/IBM/Landscape/Map/Patches/Shape/CubicPatch.h"




/**
* @brief Abstract base class that represents the simulation landscape.
*
* Landscape is responsible for managing the map, resources, species and animals,
* updating sources, advancing simulation time-steps, saving outputs and
* performing various calculations related to metabolism, predation and
* reproduction. Concrete subclasses implement taxon-specific behaviour
* (for example dinosaurs vs arthropods).
*/
class Landscape
{
public:
	/**
	* @brief Enumeration of Landscape subtypes used to select concrete implementations.
	*/
	enum class Type : unsigned int
	{
		Arthropods,	/**< Arthropod-oriented landscape behaviour. */
		Dinosaurs,	/**< Dinosaur-oriented landscape behaviour. */
		Aquatic		/**< Aquatic-oriented landscape behaviour. */
	};

	/**
	* @brief Factory: create an instance of a concrete Landscape for the given simulation type.
	*
	* @param simulationType A string identifying the simulation type (e.g. "Dinosaurs").
	* @return A pointer to a newly allocated Landscape instance; ownership follows caller convention.
	*/
	static Landscape* createInstance(const std::string& simulationType);
	
	/** @brief Default constructor. */
    Landscape();

	/** @brief Virtual destructor. */
	virtual ~Landscape(); 

	// non-copyable
	Landscape(const Landscape&) = delete;
    Landscape& operator=(const Landscape&) = delete;

	/**
	* @brief Attach a view object used for visualization or UI callbacks.
	*
	* @param newView Pointer to a View instance. Can be nullptr to detach.
	*/
	void setView(View* newView);

	/**
	* @brief Obtain a mutable pointer to the internal Map object.
	*
	* @return Pointer to the Map that can be modified by the caller.
	*/
	Map* getMutableMap();

	/**
	* @brief Obtain a const pointer to the internal Map object.
	*
	* @return Const pointer to the Map for read-only access.
	*/
	const Map* getMap() const;

	/**
	* @brief Initialize the landscape from configuration files and options.
	*
	* @param newView View pointer to attach during initialization.
	* @param configPath Filesystem path to configuration folder or file.
	* @param newOutputFolder Path where simulation outputs must be written.
	* @param fromCheckpoint True if initialization should restore state from a checkpoint.
	*/
	void init(View* newView, std::filesystem::path configPath, std::filesystem::path newOutputFolder, bool fromCheckpoint);

	/**
	* @brief Calculate statistics related to predation/attack for each species.
	*
	* The structure of the argument maps species/instars to the set of terrain
	* cells in which they can inhabit.
	*
	* @param mapSpeciesInhabitableTerrainCells Mapping of species/instars to iterators of terrain cells.
	*/
	void calculateAttackStatistics(std::vector<CustomIndexedVector<Instar, std::vector<std::vector<TerrainCell*>::iterator>>> &mapSpeciesInhabitableTerrainCells);
	
	/**
	* @brief Initialize animals at simulation start according to provided initial population.
	*
	* @param initialPopulation Species and instar counts used to seed the landscape.
	* @param mapSpeciesInhabitableTerrainCells Mapping of species/instars to inhabitible terrain cells.
	*/
	void initializeAnimals(const CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, unsigned int>>& initialPopulation, const CustomIndexedVector<AnimalSpeciesID, std::vector<Genome>>& initialGenomesPool, std::vector<CustomIndexedVector<Instar, std::vector<std::vector<TerrainCell*>::iterator>>>& mapSpeciesInhabitableTerrainCells);

	/**
	* @brief Evolve landscape.
	*
	* This method coordinates updates to resources, animals and environmental
	* sources across the whole map.
	*/
	void evolveLandscape();

	/**
	* @brief Generate a new unique identifier for an edible item.
	*
	* @return A newly generated EdibleID (project-specific type alias).
	*/
	EdibleID generateEdibleId();

	/** @brief Reset the internal edible identifier counter to its initial state. */
	void resetEdibleIdCounter();

	/**
	* @brief Generate a new unique identifier for a resource.
	*
	* @return Generated id_type for resources.
	*/
	id_type generateResourceId();

	/**
	* @brief Generate a new unique identifier for an animal.
	*
	* @return Generated id_type for animals.
	*/
	id_type generateAnimalId();

	/**
	* @brief Query whether this landscape implementation models dinosaurs.
	*
	* Default returns false; dinosaur-specific subclass should override.
	*
	* @return True if landscape represents dinosaurs; false otherwise.
	*/
	virtual bool isDinosaurs() const;

	/**
	* @brief Calculate the maximum voracity (dry mass) an animal can have per time-step.
	*
	* Concrete subclasses must implement taxon-specific metabolic/allometric
	* formulas.
	*
	* @param wetMass Animal wet mass.
	* @param conversionToWetMass Conversion factor between dry and wet mass.
	* @param hasCapitalBreeding Whether the animal uses capital-breeding strategy.
	* @param basalMetabolicDryMassLoss Basal dry mass lost per time-step.
	* @param YodzisA Allometric parameter A used in metabolic scaling.
	* @param YodzisB Allometric parameter B used in metabolic scaling.
	* @return Maximum dry mass the animal may eat during a single time-step.
	*/
	virtual DryMass calculateMaxVoracityTimeStep(const WetMass& wetMass, const PreciseDouble& conversionToWetMass, const bool hasCapitalBreeding, const DryMass& basalMetabolicDryMassLoss, const PreciseDouble& YodzisA, const PreciseDouble& YodzisB) const=0;

	/**
	* @brief Calculate a post-time-step adjusted speed based on allometry and mass.
	*
	* @param speedAfterAllometric Speed derived from allometric rules.
	* @param wetMass Animal wet mass.
	* @return Adjusted speed after applying post-allometric effects.
	*/
	virtual PreciseDouble calculatePostTSpeed(const PreciseDouble& speedAfterAllometric, const WetMass& wetMass) const=0;

	/**
	* @brief Whether the simulation should save per-animal constitutive trait files.
	*
	* @return True when saving of constitutive traits is enabled.
	*/
	bool getSaveAnimalConstitutiveTraits() const;

	/**
	* @brief Whether to write edibilities information to file.
	*
	* @return True when edibilities file output is enabled.
	*/
	bool getSaveEdibilitiesFile() const;

	/**
	* @brief Set the output folder and (optionally) generate result-folder names.
	*
	* @param newOutputFolder Destination folder for outputs.
	* @param configName Name of the configuration used (appended to results path).
	*/
	void setOutputFolder(std::filesystem::path newOutputFolder, const std::string& configName);

	/** @brief Print predation events matrix showing how species prey on others. */
	void printPredationEventsOnOtherSpeciesMatrix();

	/**
	* @brief Query whether resources species compete among themselves.
	*
	* @return True if competition among resource species is enabled.
	*/
	bool getCompetitionAmongResourceSpecies() const;

	/**
	* @brief Register a non-statistical animal instance with the landscape.
	*
	* Registered animals are considered for actions such as movement, feeding
	* and saving.
	*
	* @param animal Pointer to AnimalNonStatistical to register.
	*/
	void registerAnimal(AnimalNonStatistical* animal);

	/**
	* @brief Unregister a non-statistical animal instance from the landscape.
	*
	* @param animal Pointer to AnimalNonStatistical to unregister.
	*/
	void unregisterAnimal(AnimalNonStatistical* animal);

	/**
	* @brief Compute basal metabolic dry mass loss per time-step.
	*
	* @param wetMass Animal wet mass.
	* @param actE_metValue Activity energy/metabolic parameter.
	* @param actE_metThermallyDependent Whether actE_metValue depends on temperature.
	* @param met_rateValue Metabolic rate parameter.
	* @param met_rateThermallyDependent Whether metabolic rate depends on temperature.
	* @param tempFromLab Laboratory reference temperature.
	* @param terrainCellTemperature Temperature of the terrain cell the animal occupies.
	* @param conversionToWetMass Conversion factor between dry and wet mass.
	* @return Dry mass lost to basal metabolism during one time-step.
	*/
	DryMass getBasalMetabolicDryMassLossPerTimeStep(WetMass wetMass, const PreciseDouble& actE_metValue, const bool actE_metThermallyDependent, const PreciseDouble& met_rateValue, const bool met_rateThermallyDependent, const Temperature& tempFromLab, const Temperature& terrainCellTemperature, const PreciseDouble& conversionToWetMass) const;
	
	/**
	* @brief Compute total metabolic dry mass loss per time-step including activity.
	*
	* @param wetMass Animal wet mass.
	* @param proportionOfTimeTheAnimalWasMoving Fraction of time spent moving.
	* @param actE_metValue Activity energy/metabolic parameter.
	* @param actE_metThermallyDependent Whether actE_metValue depends on temperature.
	* @param met_rateValue Metabolic rate parameter.
	* @param met_rateThermallyDependent Whether metabolic rate depends on temperature.
	* @param search_areaValue Search area parameter value.
	* @param tempFromLab Laboratory reference temperature.
	* @param terrainCellTemperature Terrain cell temperature.
	* @param conversionToWetMass Conversion factor between dry and wet mass.
	* @return Dry mass lost due to metabolism and activity during one time-step.
	*/
	DryMass getMetabolicDryMassLossPerTimeStep(WetMass wetMass, const PreciseDouble& proportionOfTimeTheAnimalWasMoving, const PreciseDouble& actE_metValue, const bool actE_metThermallyDependent, const PreciseDouble& met_rateValue, const bool met_rateThermallyDependent, const PreciseDouble& search_areaValue, const Temperature& tempFromLab, const Temperature& terrainCellTemperature, const PreciseDouble& conversionToWetMass) const;

	/** @brief Query whether the landscape is running in growth & reproduction test mode. */
	bool isGrowthAndReproTest() const;

	/**
	* @brief Initialize output files used during simulation runs.
	*
	* @param configPath Path used to locate output configuration and naming.
	*/
	void initializeOutputFiles(std::filesystem::path configPath);

	/**
	* @brief Access the configured number of time-steps that constitute one simulated day.
	*
	* @return Reference to the steps-per-day value stored in the Landscape.
	*/
	const PreciseDouble& getTimeStepsPerDay() const;

	/**
	* @brief Get the top-level result folder path where outputs were written.
	*
	* @return Const reference to the result folder path.
	*/
	const std::filesystem::path& getResultFolder() const;

	/**
	* @brief Get the threshold used for probability density function (PDF) filtering.
	*
	* @return Reference to PDF threshold value.
	*/
	const PreciseDouble& getPdfThreshold() const;

	/**
	* @brief Get scale factor for mass used by concrete landscape types.
	*
	* Concrete subclasses implement taxon-specific mass-scaling factor.
	*/
	virtual PreciseDouble getScaleMass() const=0;

	/**
	* @brief Calculate metabolic dry mass loss per day (taxon-specific).
	*
	* @note Implemented by concrete subclasses.
	*/
	virtual DryMass calculateMetabolicDryMassLossPerDay(const WetMass &wetMass, const PreciseDouble&proportionOfTimeTheAnimalWasMoving, const PreciseDouble& actE_metValue, const bool actE_metThermallyDependent, const PreciseDouble& met_rateValue, const bool met_rateThermallyDependent, const PreciseDouble& search_areaValue, const Temperature& tempFromLab, const Temperature& terrainCellTemperature, const PreciseDouble& conversionToWetMass) const=0;
	
	/**
	* @brief Calculate a new voracity value (wet/dry conversion aware).
	*
	* @param wetMass Current wet mass.
	* @param conversionToWetMass Conversion factor used to convert between dry and wet mass.
	* @return New voracity expressed as a PreciseDouble.
	*/
	PreciseDouble calculateNewVoracity(const PreciseDouble&wetMass, const PreciseDouble&conversionToWetMass) const;

	/**
	* @brief Get the landscape type enum of this instance.
	*
	* @return A Type value identifying the concrete landscape kind.
	*/
	virtual Type getType() const=0;

	/**
	* @brief Boost.Serialization hook for Landscape.
	*
	* Serializes relevant fields of the Landscape so checkpoints can be written.
	*/
	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
	View* view;	/**< Visualization / UI view attached to the landscape (may be null). */

	unsigned int fixedSeedValue; /**< Fixed RNG seed value for deterministic runs (when used). */

	Day runDays;				/**< Number of days to simulate. */
	unsigned int recordEach;	/**< Frequency (in time-steps) to record checkpoints or outputs. */
	PreciseDouble timeStepsPerDay; /**< Number of discrete time-steps per simulation day. */
	unsigned int numberOfCombinations; /**< Number of simulated attacks from the species  */
	bool growthAndReproTest;	/**< Flag: run only growth & reproduction tests if true. */

	std::vector<std::pair<size_t, MoistureSource*>> appliedMoisture; /**< Ordered moisture sources with priority. */
	std::vector<std::vector<std::pair<size_t, ResourceSource*>>> appliedResource; /**< Resource sources grouped by priority. */

	std::vector<AnimalNonStatistical*> landscapeAnimals; /**< Non-statistical animals present in the landscape. */

	std::vector<std::vector<std::vector<CellResource*>>> landscapeResources; /**< Resources by cell and species. */

	std::filesystem::path outputFolder; /**< Base folder for output files during a run. */
	std::filesystem::path resultFolder; /**< Final result folder for the simulation run. */

	CustomIndexedVector<AnimalSpeciesID, std::ofstream*> animalConstitutiveTraitsFile; /**< Per-species files for constitutive traits. */
	std::ofstream dailySummaryFile; /**< Stream for daily summary outputs. */
	std::ofstream extendedDailySummaryFile; /**< Stream for extended daily summary outputs. */
	std::ofstream movementsFile; /**< Stream for recorded movements. */
	std::ofstream edibilitiesFile; /**< Stream describing edible links / edibilities. */
	bool saveEdibilitiesFile; /**< Whether to produce the edibilities file. */
	
	bool competitionAmongResourceSpecies; /**< Whether resource species compete for the same resource. */
	float exitTimeThreshold; /**< Threshold used to determine early simulation exit. */

	bool savePredationEventsOnOtherSpecies; /**< Whether predation events matrix should be saved. */

	bool saveAnimalConstitutiveTraits; /**< Whether to save animal constitutive trait files. */

	bool saveGenetics; /**< Whether genetic data should be saved. */

	bool saveDailySummary; /**< Whether daily summary should be written. */

	bool saveExtendedDailySummary; /**< Whether extended daily summary should be written. */

	bool saveMovements; /**< Whether movements file is activated. */

	bool saveAnimalsEachDayStart; /**< Save snapshot of animals at day start. */

	bool saveAnimalsEachDayEnd; /**< Save snapshot of animals at day end. */

	bool saveCellsEachDay; /**< Save cell-level snapshots each day. */

	bool saveAnimalsEachDayVoracities; /**< Save voracity per animal each day. */

	bool saveAnimalsEachDayPredationProbabilities; /**< Save predation probabilities per animal. */

	bool saveActivity; /**< Whether to save activity-related outputs. */

	bool saveMassInfo; /**< Whether to save mass/energetics information. */

	bool saveIntermidiateVolumes; /**< Whether intermediate volume snapshots are saved. */
	unsigned int saveIntermidiateVolumesPeriodicity; /**< Periodicity for intermediate volumes. */

	std::vector<float> heatingCodeTemperatureCycle;	/**< Optional heating code cycle defining temperatures through time. */

	bool exitAtFirstExtinction; /**< Stop simulation when first species becomes extinct. */

	bool stabilityEnabled; /**< Enable stability detection logic. */
	PreciseDouble coefficientOfVariationThreshold; /**< CV threshold to determine stability. */
	TimeStep stabilityCalculationInterval; /**< Interval used for stability calculations. */
	
	std::vector<Species*> existingSpecies; /**< All species currently present in the landscape. */
	std::vector<ResourceSpecies*> existingResourceSpecies; /**< Existing resource species. */
	std::vector<AnimalSpecies*> existingAnimalSpecies; /**< Existing animal species. */

	Map* landscapeMap; /**< Pointer to the landscape Map instance. */

	PreciseDouble multiplierForFieldMetabolicRate; /**< Multiplier applied to field metabolic rate calculations. */

	/**
	* @brief Add an external moisture source with a priority.
	*
	* @param source Pointer to the MoistureSource to add.
	* @param priority Priority value used when combining multiple sources.
	*/
	void addAppliedMoisture(MoistureSource* const source, const size_t priority);

	/**
	* @brief Add an external resource source with a priority.
	*
	* @param source Pointer to the ResourceSource to add.
	* @param priority Priority for the given source.
	*/
	void addAppliedResource(ResourceSource* const source, const size_t priority);

	/**
	* @brief Obtain the configured number of simulation run days.
	*
	* @return The Day value representing the run length.
	*/
	const Day getRunDays() const;

	/**
	* @brief Erase statistical records for a population container.
	*
	* @param population Population data structure to clear.
	*/
	void eraseStatisticsPopulation(std::vector<CustomIndexedVector<Instar, std::vector<AnimalStatistical *>>>& population);
	
	/** @brief Remove dead animals from internal containers and free resources. */
	void purgeDeadAnimals();

	/**
	* @brief Validate the headers of ontogenetic links CSVs read via rapidcsv.
	*
	* @param ontogeneticLinksPreference rapidcsv document representing preference links.
	* @param ontogeneticLinksProfitability rapidcsv document representing profitability links.
	*/
	void checkOntogeneticLinksHeaders(const rapidcsv::Document& ontogeneticLinksPreference, const rapidcsv::Document& ontogeneticLinksProfitability) const;

	/**
	* @brief Allometric scaling formula by Yodzis (helper).
	*
	* @param wetMass Animal wet mass used in the scaling.
	* @param newA Optional exponent parameter A.
	* @param newB Optional exponent parameter B.
	* @return Computed scaling value according to the Yodzis formula.
	*/
	PreciseDouble Yodzis(const PreciseDouble& wetMass, const PreciseDouble& newA = 0.1, const PreciseDouble& newB = 0.75) const;
	
	/**
	* @brief Allometric scaling helper based on Garland (1983).
	*
	* @param wetMass Animal wet mass.
	* @return Scaling factor from Garland1983 formula.
	*/
	PreciseDouble Garland1983(const PreciseDouble& wetMass) const;

	/**
	* @brief Calculate wet food requirement from animal wet mass (taxon-specific).
	*
	* @return Wet food amount for the given wetMass.
	*/
	virtual PreciseDouble calculateWetFood(const PreciseDouble& wetMass) const=0;

	/**
	* @brief Set simulation-wide parameters from configuration files.
	*
	* @param configPath Path to simulation configuration.
	*/
	void setSimulationParams(const std::filesystem::path& configPath);

	/**
	* @brief Set landscape parameters such as sources, patches and species definitions.
	*
	* @param configPath Path to configuration folder.
	* @param fromCheckpoint True if parameters should be loaded in checkpoint-restore mode.
	*/
	void setLandscapeParams(const std::filesystem::path& configPath, bool fromCheckpoint);

	/**
	* @brief Update the map state for a number of time-steps (e.g. applied sources, resources regeneration).
	*
	* @param numberOfTimeSteps Number of time-steps to advance the map.
	*/
	void updateMap(const TimeStep numberOfTimeSteps);

	/** @brief Update all external sources (moisture, resource supply, etc.). */
	void updateAllSources();

	/** @brief Update per-species global maxima used in functional responses or other calculations. */
	void updateAnimalSpeciesGlobalMaximum();

	/**
	* @brief Initialize the internal Map object using JSON configuration.
	*
	* @param mapConfig JSON object describing map layout and parameters.
	*/
	void initializeMap(const nlohmann::json &mapConfig);

	/** @brief Read obstacle patch definitions from JSON files in configPath. */
	void readObstaclePatchesFromJSONFiles(const std::filesystem::path& configPath);

	/** @brief Read habitat domain patch definitions from JSON files in configPath. */
	void readHabitatDomainPatchesFromJSONFiles(const std::filesystem::path& configPath);

	/** @brief Read moisture patch definitions from JSON files in configPath. */
	void readMoisturePatchesFromJSONFiles(const std::filesystem::path& configPath);

	/**
	* @brief Read resource patch definitions from JSON files in configPath.
	*
	* @return True if resource patches were found and loaded successfully.
	*/
	bool readResourcePatchesFromJSONFiles(const std::filesystem::path& configPath);

	/**
	* @brief Read animal species definitions from JSON files and optionally fill the initial population.
	*
	* @param configPath Path to species configuration files.
	* @param initialPopulation Output argument filled with initial per-species instar counts.
	* @return True if animal species were loaded successfully.
	*/
	bool readAnimalSpeciesFromJSONFiles(const std::filesystem::path& configPath, CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, unsigned int>>& initialPopulation);

	/** @brief Read resource species definitions from JSON files. */
	bool readResourceSpeciesFromJSONFiles(const std::filesystem::path& configPath);
	
	/** @brief Configure early-exit threshold for the simulation. */
	void setExitTimeThreshold(float exitTimeThresholdValue);

	/** @brief Add a resource species from parsed JSON description. */
	void addResourceSpecies(const nlohmann::json &resourceSpeciesInfo);

	/**
	* @brief Add an animal species from parsed JSON description and update initialPopulation.
	*
	* @param animalSpeciesInfo JSON document describing the animal species.
	* @param initialPopulation Container to update with initial counts for the new species.
	*/
	void addAnimalSpecies(const nlohmann::json &animalSpeciesInfo, CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, unsigned int>>& initialPopulation);
	
	/**
	* @brief Find a loaded resource species by name.
	*
	* @param name Name of the resource species to find.
	* @return Pointer to ResourceSpecies if found, otherwise nullptr.
	*/
	ResourceSpecies * getResourceSpecies(std::string name);

	/**
	* @brief Find a loaded animal species by name.
	*
	* @param name Name of the animal species to find.
	* @return Pointer to AnimalSpecies if found, otherwise nullptr.
	*/
	AnimalSpecies * getAnimalSpecies(const std::string& name);

	/**
	* @brief Configure ontogenetic links between life stages from config files.
	*
	* @param configPath Path with ontogenetic link definitions.
	*/
	void setOntogeneticLinks(const std::filesystem::path& configPath);

	/**
	* @brief Register a newly created generic Species in the landscape.
	*
	* @param newSpecies Pointer to the Species instance to add.
	*/
	void addSpecies(Species* newSpecies);

	/**
	* @brief Compute the default results folder name from an output root and config name.
	*
	* @param outputFolder Base output folder path.
	* @param configName Configuration name used to create a subfolder.
	* @return Computed result folder path.
	*/
	static std::filesystem::path getResultFolderName(const std::filesystem::path& outputFolder, const std::string& configName);

	/**
	* @brief Debugging helper: print animals present along cells at a given time-step.
	*
	* @param numberOfTimeSteps Time-step index.
	* @param simulationPoint Simulation point index if required by multi-point runs.
	*/
	void printAnimalsAlongCells(const TimeStep numberOfTimeSteps, const int simulationPoint) const;
	
	/** @brief Print cell values along cells at the provided time-step (debug output). */
	void printCellAlongCells(const TimeStep numberOfTimeSteps) const;

	/**
	* @brief Append extended daily summary information to a provided stream.
	*
	* @param os Output stream to append to.
	* @param numberOfTimeSteps Current time-step index for timestamping.
	*/
	void printExtendedDailySummary(std::ostream& os, const TimeStep numberOfTimeSteps) const;
	
	/**
	* @brief Save a snapshot of a specific animal species to disk.
	*
	* @param filenameRoot Root path where the snapshot will be stored.
	* @param filename Chosen file name for the snapshot.
	* @param numberOfTimeSteps Time-step at which the snapshot is taken.
	* @param species Pointer to the species to snapshot.
	*/
	void saveAnimalSpeciesSnapshot(std::filesystem::path filenameRoot, std::string filename, const TimeStep numberOfTimeSteps, AnimalSpecies* species);
	
	/**
	* @brief Save a snapshot of a specific resource species to disk.
	*/
	void saveResourceSpeciesSnapshot(std::filesystem::path filenameRoot, std::string filename, const TimeStep numberOfTimeSteps, ResourceSpecies* species);
	
	/** @brief Save a water distribution snapshot to disk. */
	void saveWaterSnapshot(std::filesystem::path filenameRoot, std::string filename, const TimeStep numberOfTimeSteps);
	
	/** @brief Test if all species are extinct on the landscape. */
	bool isExtinguished() const;

	/**
	* @brief Test if the simulation has stabilised using internal stability metrics.
	*
	* @param numberOfTimeSteps Time-step used as reference for stabilisation check.
	*/
	bool isSimulationStabilised(const TimeStep& numberOfTimeSteps);

	/**
	* @brief Get a const reference to the list of resource species currently loaded.
	*
	* @return Vector of pointers to ResourceSpecies objects.
	*/
	const std::vector<ResourceSpecies*>& getExistingResourceSpecies() const;
	
	/** @brief Get a mutable reference to the vector of resource species. */
	std::vector<ResourceSpecies*>& getMutableExistingResourceSpecies();

	/** @brief Get a mutable reference to the vector of animal species. */
	std::vector<AnimalSpecies*>& getMutableExistingAnimalSpecies();
	
	/** @brief Get a const reference to the vector of loaded animal species. */
	const std::vector<AnimalSpecies*>& getExistingAnimalSpecies() const;

	/** @brief Get a mutable reference to the vector of all species. */
	std::vector<Species*>& getMutableExistingSpecies();

	/** @brief Get a const reference to the vector of all species. */
	const std::vector<Species*>& getExistingSpecies() const;

private:
	const std::filesystem::path RESOURCE_FOLDER_NAME = "resource"; /**< Name of the resource folder. */
	const std::filesystem::path SPECIES_FOLDER_NAME = "species"; /**< Name of the species folder. */
	const std::filesystem::path OBSTACLE_FOLDER_NAME = "obstacle"; /**< Name of the obstacle folder. */
	const std::filesystem::path HABITAT_DOMAIN_FOLDER_NAME = "habitat_domain"; /**< Habitat domain folder name. */
	const std::filesystem::path MOISTURE_FOLDER_NAME = "moisture"; /**< Moisture folder name. */

	std::string serializationVersion; /**< Serialization version used for checkpoints. */

	id_type edibleIdCounter;	/**< Counter used to generate unique edible IDs. */
	id_type resourceIdCounter;	/**< Counter used to generate unique resource IDs. */
	id_type animalIdCounter;	/**< Counter used to generate unique animal IDs. */

	bool checkpointsEnabled; /**< Whether checkpointing is enabled. */
	bool binaryCheckpointEnabled; /**< Whether binary checkpoints are used. */
	unsigned int checkpointsRecordEach; /**< Frequency for writing checkpoints. */

	PreciseDouble pdfThreshold; /**< Threshold used for PDF-based filtering. */

	/**
	* @brief Checks whether simulation checkpoints are enabled.
	* @return True if checkpoints are activated.
	*/
	bool isCheckpointsEnabled() const;

	/**
	* @brief Checks whether checkpoint files should be written in binary format.
	* @return True if binary checkpointing is enabled.
	*/
	bool isBinaryCheckpointEnabled() const;

	/**
	* @brief Returns the frequency (in timesteps) at which checkpoints are recorded.
	* @return Number of timesteps between two recorded checkpoints.
	*/
	unsigned int getCheckpointsRecordEach() const;

	/**
	* @brief Saves a checkpoint of the entire landscape state.
	*
	* @param numberOfTimeSteps Current simulation timestep.
	*/
	void saveCheckpoint(const TimeStep& numberOfTimeSteps);

	/**
	* @brief Extracts landscape resource biomass and animal population counts.
	*
	* @param landscapeResourceBiomass Output vector with biomass per cell.
	* @param landscapeAnimalsPopulation Output nested structure with counts of animals per species and instar.
	*/
	void obtainLandscapeResourceBiomassAndAnimalsPopulation(std::vector<WetMass> &landscapeResourceBiomass, CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<LifeStage, unsigned int>> &landscapeAnimalsPopulation) const;
	
	/**
	* @brief Moves all animals according to their movement rules.
	* @param numberOfTimeSteps Current timestep number.
	*/
	void moveAnimals(const TimeStep& numberOfTimeSteps);

	/**
	* @brief Executes all behavioural actions (feeding, interactions, etc.) of animals.
	* @param numberOfTimeSteps Current timestep number.
	*/
	void performAnimalsActions(const TimeStep numberOfTimeSteps);

	/**
	* @brief Prints (or logs) voracity values for all animals in the landscape.
	* @param numberOfTimeSteps Current timestep number.
	*/
	void printAnimalsVoracities(const TimeStep& numberOfTimeSteps);

	/**
	* @brief Removes animals that die from background mortality.
	* @param numberOfTimeSteps Current timestep.
	*/
    void dieFromBackground(const TimeStep& numberOfTimeSteps);

	/**
	* @brief Transfers assimilated food into the internal energy tanks of animals.
	* @param numberOfTimeSteps Current timestep.
	*/
    void transferAssimilatedFoodToEnergyTank(const TimeStep& numberOfTimeSteps);
    
	/**
	* @brief Applies metabolic costs to animals for the current timestep.
	* @param numberOfTimeSteps Current timestep.
	*/
	void metabolizeAnimals(const TimeStep numberOfTimeSteps);

	/**
	* @brief Applies growth processes to animals.
	* @param numberOfTimeSteps Current timestep.
	*/
    void growAnimals(const TimeStep& numberOfTimeSteps);

	/**
	* @brief Performs breeding and reproduction actions.
	* @param numberOfTimeSteps Current timestep.
	*/
    void breedAnimals(const TimeStep& numberOfTimeSteps);

	/**
	* @brief Evaluates simulation break conditions.
	* @param numberOfTimeSteps Current timestep.
	* @return True if break conditions are met.
	*/
	bool checkBreakConditions(const TimeStep& numberOfTimeSteps);

	/**
	* @brief Saves detailed information explaining why break conditions were reached.
	* @param numberOfTimeSteps Current timestep.
	*/
	void saveBreakConditionsInfo(const TimeStep& numberOfTimeSteps);

	/**
	* @brief Applies a patch to the landscape (resource modification, terrain update, etc.).
	* @param patch Patch object representing the modification.
	* @return True if the patch was successfully applied.
	*/
	bool applyPatch(Patch& patch);

	/**
	* @brief Selects a random predator from a set of potential predators.
	*
	* @param numberOfTotalPotentialPredators Total count of possible predators.
	* @param potentialPredators Mapping from instar to lists of predator individuals.
	* @return Pair containing the chosen predator and its instar.
	*/
	std::pair<AnimalStatistical*, Instar> getRandomPredator(
		const size_t numberOfTotalPotentialPredators, const CustomIndexedVector<Instar, std::vector<AnimalStatistical*>*> &potentialPredators
	) const;

	/**
	* @brief Selects a random prey for a predator, considering instar and species.
	*
	* @param numberOfPotentialPreysPerInstar Counts of preys per instar.
	* @param predatorInstar Instar of the predator.
	* @param predatorSpecies Pointer to predator species.
	* @param animalsPopulation Population of animals grouped by species and instar.
	* @return Pointer to the chosen prey, or nullptr if no prey is available.
	*/
	AnimalStatistical* getRandomPrey(
		const CustomIndexedVector<Instar, size_t> &numberOfPotentialPreysPerInstar, const Instar &predatorInstar, 
		const AnimalSpecies* const predatorSpecies, const std::vector<CustomIndexedVector<Instar, std::vector<AnimalStatistical*>>> &animalsPopulation
	) const;

	/**
	* @brief Computes statistics for all animal species (population size, biomass, etc.).
	*/
	void calculateAnimalSpeciesStatistics();
};


/**
 * @brief Boost library namespace.
 */
namespace boost {
	/**
     * @brief Boost.Serialization library namespace.
     */
    namespace serialization {
		/**
		* @brief Boost.Serialization free function overload to (de)serialize Landscape pointers.
		*
		* Specialization to assist in (de)serializing polymorphic Landscape* pointers
		* while providing a View pointer during restoration.
		*/
        template<class Archive>
        void serialize(Archive &ar, Landscape* &landscapePtr, const unsigned int version, View* view);
    }
}

#endif // LANDSCAPE_H_
