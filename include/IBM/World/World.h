#ifndef WORLD_HPP_
#define WORLD_HPP_

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
#include <sys/time.h>
#include <numeric>
#include <ctime>
#include <nlohmann/json.hpp>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>
#include <boost/filesystem.hpp>


#include "IBM/Maths/GaussianDistribution.h"
#include "IBM/Maths/Random.h"
#include "Misc/Types.h"
#include "IBM/World/Map/TerrainCells/TerrainCell.h"
#include "IBM/World/LivingBeings/Edible.h"
#include "Exceptions/LineInfoException.h"
#include "IBM/World/LivingBeings/Resources/Species/Factory/ResourceSpeciesFactory.h"
#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "IBM/World/LivingBeings/Resources/Resource.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpeciesFactory.h"
#include "IBM/World/Map/Patches/Resource/ResourcePatchFactory.h"
#include "IBM/World/Map/Patches/Obstacle/ObstaclePatchFactory.h"
#include "IBM/World/Map/Patches/Moisture/MoisturePatchFactory.h"
#include "Misc/Utilities.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "Misc/Coordinate3D.h"
#include "IBM/World/Map/Map.h"
#include "Misc/Macros.h"
#include "IBM/World/Map/SpatialTree.h"
#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"
#include "Misc/EnumClass.h"
#include "IBM/World/LivingBeings/Animals/AnimalNonStatistical.h"





class World
{
private:
	const double printBarEach = 0.05;

	double currentMovePercentage;
	unsigned int currentNumberOfTerrainCellsMoved;

	const int burnIn;

	std::vector<std::pair<double, double>> speciesAmplitude;


	friend class boost::serialization::access;

	/**
     * @name Patches
     * @{
     */

    /**
     * @name Obstacle patches
     * @{
     */
	void applyObstaclePatch(const std::unique_ptr<const ObstaclePatch> &obstaclePatch);
    /** @} */

	/**
     * @name Moisture patches
     * @{
     */
	void applyMoisturePatch(MoisturePatch &moisturePatch);
	/** @} */
	
    /** @} */

	std::pair<AnimalStatistical*, Instar> getRandomPredator(
		const unsigned int numberOfTotalPotentialPredators, const InstarVector<std::vector<AnimalStatistical*>*> &potentialPredators
	) const;

	AnimalStatistical* getRandomPrey(
		const InstarVector<unsigned int> &numberOfPotentialPreysPerInstar, const Instar &predatorInstar, 
		const AnimalSpecies* const predatorSpecies, const std::vector<InstarVector<std::vector<AnimalStatistical*>>> &animalsPopulation
	) const;

	const int getBurnIn() const;

protected:
	void calculateTotalNumberOfOntogeneticLinks();

public:
	enum class Type : unsigned int
	{
		Arthropods,
		Dinosaurs,
		Aquatic
	};


	static std::unique_ptr<World> createInstance(nlohmann::json* jsonTree, nlohmann::json &worldConfig, boost::filesystem::path outputFolder, boost::filesystem::path configPath, int burnIn);
	
	
	std::vector<const Species*> existingSpeciesConstPointers;

	std::vector<std::unique_ptr<ResourceSpecies>> existingResourceSpecies;
	std::vector<ResourceSpecies*> existingResourceSpeciesPointers;
	std::vector<const ResourceSpecies*> existingResourceSpeciesConstPointers;

	std::vector<std::unique_ptr<AnimalSpecies>> existingAnimalSpecies;
	std::vector<AnimalSpecies*> existingAnimalSpeciesPointers;
	std::vector<const AnimalSpecies*> existingAnimalSpeciesConstPointers;

	
	// Constructors
	// Recover from previously stored world
	World(std::string filesNameRoot);

	// Create a new, empty world
    World(json * jsonTree, json &worldConfig, boost::filesystem::path outputFolder, boost::filesystem::path configPath, int burnIn, const double &massRatio);
	virtual ~World(); // Destructor

	void calculateAttackStatistics(std::vector<InstarVector<std::vector<std::vector<TerrainCellInterface*>::iterator>>> &mapSpeciesInhabitableTerrainCells);
	void updateMaxSearchArea(const double& currentAnimalMaxSearchArea);

	void initializeAnimals();

	void evolveWorld();

	void saveOptimizationResult(boost::filesystem::path resultFolder);

	void increaseMovePrintBar();

	virtual const bool isDinosaurs() const;

	virtual double calculateMaxMassPredicted(const double& dryMass, const double& wetMass, const double& conversionToWetMass, const unsigned int numberOfTimeSteps, const bool hasCapitalBreeding, const double& minTotalMetabolicDryMassLoss, const double& newAAdult, const double& newB) const=0;

	virtual double calculatePostTSpeed(const double& speedValue, const double& wetMass, const double& scaleForSpeed) const=0;

	inline bool getSaveAnimalConstitutiveTraits() const { return saveAnimalConstitutiveTraits; }
	inline bool getSaveEdibilitiesFile() const { return saveEdibilitiesFile; }
	inline std::ofstream& getConstitutiveTraitsFile() { return constitutiveTraitsFile; }

	void setOutputFolder(boost::filesystem::path outputFolder);
	void printPredationEventsOnOtherSpeciesMatrix(std::ostream& predationEventsOnOtherSpeciesFile);
	//void printInteractionMatrices(std::ostream& encountersMatrixFile, std::ostream& predationsMatrixFile, std::ostream& nodesMatrixFile);

	// Getters
	bool getCompetitionAmongResourceSpecies() const;
	float getExitTimeThreshold();
	float getMinLogMassRatio() {return minLogMassRatio;};
	float getMaxLogMassRatio() {return maxLogMassRatio;};
	float getSigmaForPDF() {return sigmaForPDF;};
	float getMuForPDF() {return muForPDF;};
	double getMaxSearchArea() {return maxSearchArea;};
	const std::vector<const ResourceSpecies*>& getExistingResourceSpecies() const;
	const std::vector<ResourceSpecies*>& getMutableExistingResourceSpecies() const;
	const std::vector<AnimalSpecies*>& getMutableExistingAnimalSpecies() const;
	const std::vector<const AnimalSpecies*>& getExistingAnimalSpecies() const;
	const std::vector<const Species*>& getExistingSpecies() const;
	double getPeriodLength();

	inline bool isGrowthAndReproTest() const { return growthAndReproTest; }

	// Saves everything so simulation can be snapshoted and recovered later
	void save(std::string filesNameRoot);

	// Read water from stored volume
	void initializeOutputFiles(json * jsonTree);

	bool exitAtFirstExtinction;
	
#ifdef _PTHREAD
		struct ThreadRangerArgument
		{
				World* world;
				unsigned int x0, x1, y0, y1, z0, z1;
				int day;
		};
		static void* activateAnimalsThreadMaker(void* threadArgument);
		void* activateAnimalsTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, int day);
		static void* growResourceThreadMaker(void* threadArgument);
		void* growResourceTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1);
		static void* moveAnimalsThreadMaker(void* threadArgument);
		void* moveAnimalsTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, int day);
		static void* assimilateThreadMaker(void* threadArgument);
		void* assimilateTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, int day);
#endif

	#ifdef DEBUG
	int multipleSameSearchedAnimalToday;
	int multipleSameEncounteredAnimalToday;
	int multipleSameHuntedAnimalToday;
	int multipleSamePredatedAnimalToday;
	#endif

	float getEncounterHuntedSizeAH() const {
		return encounterHuntedSizeAH;
	}

	void setEncounterHuntedSizeAH(float encounterHuntedSizeAH) {
		this->encounterHuntedSizeAH = encounterHuntedSizeAH;
	}

	float getEncounterHuntedSizeSAW() const {
		return encounterHuntedSizeSAW;
	}

	void setEncounterHuntedSizeSAW(float encounterHuntedSizeSAW) {
		this->encounterHuntedSizeSAW = encounterHuntedSizeSAW;
	}

	float getEncounterHuntedVoracityAH() const {
		return encounterHuntedVoracityAH;
	}

	void setEncounterHuntedVoracityAH(float encounterHuntedVoracityAH) {
		this->encounterHuntedVoracityAH = encounterHuntedVoracityAH;
	}

	float getEncounterHuntedVoracitySAW() const {
		return encounterHuntedVoracitySAW;
	}

	void setEncounterHuntedVoracitySAW(float encounterHuntedVoracitySAW) {
		this->encounterHuntedVoracitySAW = encounterHuntedVoracitySAW;
	}

	const double& getTimeStepsPerDay() const;

	float getEncounterHunterSizeAH() const {
		return encounterHunterSizeAH;
	}

	void setEncounterHunterSizeAH(float encounterHunterSizeAH) {
		this->encounterHunterSizeAH = encounterHunterSizeAH;
	}

	float getEncounterHunterSizeSAW() const {
		return encounterHunterSizeSAW;
	}

	void setEncounterHunterSizeSAW(float encounterHunterSizeSAW) {
		this->encounterHunterSizeSAW = encounterHunterSizeSAW;
	}

	float getEncounterHunterVoracityAH() const {
		return encounterHunterVoracityAH;
	}

	void setEncounterHunterVoracityAH(float encounterHunterVoracityAH) {
		this->encounterHunterVoracityAH = encounterHunterVoracityAH;
	}

	float getEncounterHunterVoracitySAW() const {
		return encounterHunterVoracitySAW;
	}

	void setEncounterHunterVoracitySAW(float encounterHunterVoracitySAW) {
		this->encounterHunterVoracitySAW = encounterHunterVoracitySAW;
	}

	float getEncounterProbabilityDensityFunctionAH() const {
		return encounterProbabilityDensityFunctionAH;
	}

	void setEncounterProbabilityDensityFunctionAH(
			float encounterProbabilityDensityFunctionAH) {
		this->encounterProbabilityDensityFunctionAH =
				encounterProbabilityDensityFunctionAH;
	}

	float getEncounterProbabilityDensityFunctionSAW() const {
		return encounterProbabilityDensityFunctionSAW;
	}

	void setEncounterProbabilityDensityFunctionSAW(
			float encounterProbabilityDensityFunctionSAW) {
		this->encounterProbabilityDensityFunctionSAW =
				encounterProbabilityDensityFunctionSAW;
	}

	float getEncounterVoracitiesProductAH() const {
		return encounterVoracitiesProductAH;
	}

	void setEncounterVoracitiesProductAH(float encounterVoracitiesRateAH) {
		this->encounterVoracitiesProductAH = encounterVoracitiesRateAH;
	}

	const unsigned int getActualTimeStep() const 
	{
		return numberOfTimeSteps;
	}

	float getEncounterVoracitiesProductSAW() const {
		return encounterVoracitiesProductSAW;
	}

	void setEncounterVoracitiesProductSAW(float encounterVoracitiesRateSAW) {
		this->encounterVoracitiesProductSAW = encounterVoracitiesRateSAW;
	}

	float getPredationHunterVoracityAH() const {
		return predationHunterVoracityAH;
	}

	void setPredationHunterVoracityAH(float predationHunterVoracityAH) {
		this->predationHunterVoracityAH = predationHunterVoracityAH;
	}

	float getPredationHunterVoracitySAW() const {
		return predationHunterVoracitySAW;
	}

	void setPredationHunterVoracitySAW(float predationHunterVoracitySAW) {
		this->predationHunterVoracitySAW = predationHunterVoracitySAW;
	}

	float getPredationProbabilityDensityFunctionAH() const {
		return predationProbabilityDensityFunctionAH;
	}

	void setPredationProbabilityDensityFunctionAH(
			float predationProbabilityDensityFunctionAH) {
		this->predationProbabilityDensityFunctionAH =
				predationProbabilityDensityFunctionAH;
	}

	float getPredationProbabilityDensityFunctionSAW() const {
		return predationProbabilityDensityFunctionSAW;
	}

	void setPredationProbabilityDensityFunctionSAW(
			float predationProbabilityDensityFunctionSAW) {
		this->predationProbabilityDensityFunctionSAW =
				predationProbabilityDensityFunctionSAW;
	}

	float getPredationSpeedRatioAH() const {
		return predationSpeedRatioAH;
	}

	void setPredationSpeedRatioAh(float predationSpeedRatioAh) {
		predationSpeedRatioAH = predationSpeedRatioAh;
	}

	float getPredationSpeedRatioSAW() const {
		return predationSpeedRatioSAW;
	}

	double getMassRatio() const {
		return massRatio;
	}

	void setPredationSpeedRatioSaw(float predationSpeedRatioSaw) {
		predationSpeedRatioSAW = predationSpeedRatioSaw;
	}
	virtual const double getPdfThreshold() const=0;

	virtual double calculateTotalMetabolicDryMassLossPerDay(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const AnimalNonStatistical* const animal) const=0;
	virtual double calculateNewBiomassPerDay(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const=0;
    double calculateNewVoracity(const double &wetMass, const double &conversionToWetMass) const;

	/**
     * @brief Serialize the World object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
	unsigned int runDays;
	unsigned int recordEach;
	double timeStepsPerDay;
	unsigned int numberOfCombinations;
	bool growthAndReproTest;
	double massRatio;

	unsigned int numberOfTimeSteps;

	std::vector<std::unique_ptr<ExtendedMoisture>> appliedMoisture;

	
	void addAppliedMoisture(MoisturePatch &moisturePatch);

	const unsigned int getRunDays() const;

	void applyResourcePatch(const ResourcePatch &resourcePatch);

	void printActualEcosystemSize();

	void calculateAnimalSpeciesK_Density() const;

	const double Yodzis(const double& wetMass, const double& newA = 0.1, const double& newB = 0.75) const;
	const double Garland1983(const double& wetMass) const;

	virtual const double calculateWetFood(const double& wetMass) const=0;

	void updateInitialMapMoisture();
	void updateMap(const unsigned int numberOfTimeSteps, std::ofstream& tuneTraitsFile);
	void updateAllMoistureSource(const unsigned int numberOfTimeSteps);

	// Creates the terrain cells based on dimensions specified on this object. This should be called
	// right after constructor
	void initializeMap(const nlohmann::json &mapConfig);
	void readPatchesFromJSONFiles(const nlohmann::json &moistureBasePatchInfo, const bool initialPatches = false);
	void readObstaclePatchesFromJSONFiles();
	void readMoisturePatchesFromJSONFiles(const nlohmann::json &moistureBasePatchInfo);
	void readResourcePatchesFromJSONFiles();
	void readAnimalSpeciesFromJSONFiles();
	void readResourceSpeciesFromJSONFiles();
	// Setters. Be aware that setting this values after calling createTerrain will lead to undesired
	// behavior and memory errors. This should always be used prior to create terrain
	void setInitialEcosystemSize(unsigned long newInitialEcosystemSize);
	void setMaxLogMassRatio(float newMaxLogMassRatio);
	void setExitTimeThreshold(float exitTimeThreshold);
	void setMinLogMassRatio(float newMinLogMassRatio);
	void setSigmaForPDF(float sigmaForPDF);
	void setMuForPDF(float muForPDF);

	void setObstacleFolderName(boost::filesystem::path newTerrainFolderName);
	void setMoistureFolderName(boost::filesystem::path newMoistureFolderName);
	void setResourceFolderName(boost::filesystem::path newResourceFolderName);
	void setSpeciesFolderName(boost::filesystem::path newSpeciesFolderName);
	void setPeriodLength(double seconds); // In seconds
	// TODO Mario Initialize animals with initIndividualsPerDensities
	/*
	void setHeatingCodeTemperatureCycle(string temperatureFilename); // Celsius
	*/

	void setResourceToPreysCapacityTransference(double resourceToPreysCapacityTransference);
	void setPreysToPredatorsCapacityTransference(double preysToPredatorsCapacityTransference);

	void addResourceSpecies(std::unique_ptr<ResourceSpecies> newSpecies);
	AnimalSpecies* addAnimalSpecies(std::unique_ptr<AnimalSpecies> newSpecies);
	ResourceSpecies * getResourceSpecies(std::string name);
	AnimalSpecies * getAnimalSpecies(const std::string& name);
	void addSpeciesOntogeneticLinks(AnimalSpecies* const predatorSpecies, const json &resourceLinks, const json &animalLinks);
	void addOntogeneticLinks(const std::map<AnimalSpecies*, nlohmann::json> &ontogeneticLinks);

	void addSpecies(const Species* newSpecies);

	const bool getInitIndividualsPerDensities() const;

	void printAnimalsAlongCells(const unsigned int numberOfTimeSteps, const int simulationPoint) const;
	void printCellAlongCells(const unsigned int numberOfTimeSteps) const;
	void printAnimalsVoracitiesAlongCells(int day);
	std::ostream& printExtendedDailySummary(std::ostream& os, const unsigned int numberOfTimeSteps);
	void printGeneticsSummaries(const unsigned int numberOfTimeSteps);
	void saveAnimalSpeciesSnapshot(boost::filesystem::path filenameRoot, std::string filename, const unsigned int numberOfTimeSteps, AnimalSpecies* species);
	void saveResourceSpeciesSnapshot(boost::filesystem::path filenameRoot, std::string filename, const unsigned int numberOfTimeSteps, ResourceSpecies* species);
	void saveWaterSnapshot(boost::filesystem::path filenameRoot, std::string filename, const unsigned int numberOfTimeSteps);
	//void deleteExtinguishedReproducingAnimals();
	bool isExtinguished(const unsigned int numberOfTimeSteps);

	boost::filesystem::path outputFolder;
	boost::filesystem::path inputFolder;
	std::string encountersMatrixFilename;
	std::string predationsMatrixFilename;
	std::string nodesMatrixFilename;
	std::string predationEventsOnOtherSpeciesFilename;

	std::ofstream dailySummaryFile;
	std::ofstream extendedDailySummaryFile;
	std::ofstream edibilitiesFile;
	bool saveEdibilitiesFile;

	boost::filesystem::path obstacleFolderName;
	boost::filesystem::path moistureFolderName;
	boost::filesystem::path resourceFolderName;
	boost::filesystem::path speciesFolderName;
	bool initIndividualsPerDensities;
	bool competitionAmongResourceSpecies;
	float exitTimeThreshold;
	unsigned long initialEcosystemSize;
	float minLogMassRatio;
	float maxLogMassRatio;
	float sigmaForPDF;
	float muForPDF;

	float encounterHuntedVoracitySAW;
	float encounterHunterVoracitySAW;
	float encounterVoracitiesProductSAW;
	float encounterHunterSizeSAW;
	float encounterHuntedSizeSAW;
	float encounterProbabilityDensityFunctionSAW;

	float encounterHuntedVoracityAH;
	float encounterHunterVoracityAH;
	float encounterVoracitiesProductAH;
	float encounterHunterSizeAH;
	float encounterHuntedSizeAH;
	float encounterProbabilityDensityFunctionAH;

	float predationSpeedRatioSAW;
	float predationHunterVoracitySAW;
	float predationProbabilityDensityFunctionSAW;

	float predationSpeedRatioAH;
	float predationHunterVoracityAH;
	float predationProbabilityDensityFunctionAH;

	double maxSearchArea;
	double minK;

	float resourceToPreysCapacityTransference;
	float preysToPredatorsCapacityTransference;

	bool saveAnimalConstitutiveTraits;
	std::ofstream constitutiveTraitsFile;

	bool saveGeneticsSummaries;
	std::vector<std::ofstream> geneticsSummaryFile;

	bool saveIntermidiateVolumes;
	unsigned int saveIntermidiateVolumesPeriodicity;

	std::unique_ptr<Map> worldMap; // "Voxels" contained in 3D terrain

	/***
	 * Stores species and the number of initial patches/individuals associated
	 */

	//double periodLength;
	std::vector<float> heatingCodeTemperatureCycle;		
};


// Manage exceptions due to wrong sizes used for world creation
class WorldSizeException
{
private:

	std::string exceptionDescription; // Descriptive exception string
	int errorCode; // Error code used externally to manage exceptions

	friend class boost::serialization::access;

public:

	// Default constructor
	WorldSizeException() :
		exceptionDescription("Error")
	{
	}

	// Alternative constructor with personalized message
	WorldSizeException(std::string newMessage)
	{
		this->exceptionDescription = newMessage;
	}

	std::string exceptionDetail()
	{
		return this->exceptionDescription;
	}

	/**
     * @brief Serialize the WorldSizeException object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & exceptionDescription;
        ar & errorCode;
    }
};

#endif // WORLD_HPP_
