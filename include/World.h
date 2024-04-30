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

#include "WorldInterface.h"

#include "Maths/GaussianDistribution.h"
#include "Maths/Random.h"
#include "Types.h"
#include "TerrainCell.h"
#include "Edible.h"
#include "LineInfoException.h"
#include "BasalResourceSpecies.h"
#include "NoBasalResourceSpecies.h"
#include "Resource.h"
#include "SimulType.h"
#include "Utilities.h"
#include "AnimalSpecies.h"
#include "Coordinate3D.h"
#include "Patch.h"

#include <sys/time.h>
#include <ctime>
/*
#include "tbb/task.h"
#include "tbb/task_group.h"
#include "tbb/tick_count.h"
#include "tbb/concurrent_vector.h"
*/

// Library nlohmann JSON
#include <nlohmann/json.hpp>

#include <magic_enum.hpp>

#include "Output.h"

#define FMT_HEADER_ONLY
#include <fmt/core.h>

using json = nlohmann::json;


class ResourceSpecies;

/*
#ifdef _TBB
typedef tbb::concurrent_vector<TerrainCell *> Terrain1D;
typedef tbb::concurrent_vector<Terrain1D> Terrain2D;
typedef tbb::concurrent_vector<Terrain2D> Terrain3D;
#else
*/
// Lets define a 3D world by means of smaller dimension items
typedef std::vector<TerrainCell *> Terrain1D;
typedef std::vector<Terrain1D> Terrain2D;
typedef std::vector<Terrain2D> Terrain3D;
/*
#endif
*/

class World : public WorldInterface
{
public:
	std::vector<ResourceSpecies*> existingResourceSpecies;
	std::vector<AnimalSpecies*> existingAnimalSpecies;

	fs::path WeaverFolder;

	// Constructors
	// Recover from previously stored world
	World(std::string filesNameRoot);

	// Create a new, empty world
    World(json * jsonTree, json &worldConfig, fs::path inputFile, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath);
	~World(); // Destructor

	void calculateAttackStatistics(std::map<AnimalSpecies*,std::vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells);
	void updateMaxSearchArea(double currentAnimalMaxSearchArea);

	void initializeAnimals();

	virtual void evolveWorld();

	inline bool getSaveAnimalConstitutiveTraits() const { return saveAnimalConstitutiveTraits; }
	inline bool getSaveEdibilitiesFile() const { return saveEdibilitiesFile; }
	inline std::ofstream& getConstitutiveTraitsFile() { return constitutiveTraitsFile; }

	// Removes all existing terrain cells and removes all terrain. This cannot be undone
	void eraseTerrain();

	// Empties existing world, erases and recreates a new one with new dimensions
	void restart(unsigned int newWidth, unsigned int newLength, unsigned int newDepth, float newCellSize);

	void setOutputFolder(fs::path outputFolder);
	void printPredationEventsOnOtherSpeciesMatrix(std::ostream& predationEventsOnOtherSpeciesFile);
	void printInteractionMatrices(std::ostream& encountersMatrixFile, std::ostream& predationsMatrixFile, std::ostream& nodesMatrixFile);

	// Getters
	int getWidth();
	int getLength();
	int getDepth();
	bool getCompetitionAmongResourceSpecies();
	float getExitTimeThreshold();
	float getMinLogMassRatio() {return minLogMassRatio;};
	float getMaxLogMassRatio() {return maxLogMassRatio;};
	float getSigmaForPDF() {return sigmaForPDF;};
	float getMuForPDF() {return muForPDF;};
	double getMaxSearchArea() {return maxSearchArea;};
	const std::vector<ResourceSpecies *> * getExistingResourceSpecies();
	const std::vector<AnimalSpecies *> * getExistingAnimalSpecies();
	float getCellSize();
	float getMinWater();
	float getMaxWater();
	int getNumberOfWaterPatches();
	int getWaterPatchWidth();
	float getWaterPatchesAmplitude();
	float getWaterPatchesSigma();
	double getPeriodLength();
	inline const SimulType& getSimulType() const { return simulType; }

	inline bool isGrowthAndReproTest() const { return growthAndReproTest; }

	// Saves everything so simulation can be snapshoted and recovered later
	void save(std::string filesNameRoot);

	// Read water from stored volume
	void readWaterFromVolume(std::string fileName);
	void initializeOutputFiles(json * jsonTree, fs::path inputFile);

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

	#ifdef _DEBUG
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

	const double& getMaximumTemperature() const;

	void setPredationSpeedRatioSaw(float predationSpeedRatioSaw) {
		predationSpeedRatioSAW = predationSpeedRatioSaw;
	}
	double getPdfThreshold() const;

protected:
	class ResourceType {
	public:
		enum ResourceTypeValue {
			BasalResource, 
			NoBasalResource
		};

		static ResourceTypeValue stringToEnumValue(const std::string &str);

	private:
		static const std::unordered_map<std::string_view, const ResourceTypeValue> stringToEnum;
		static const std::string enumValues;

		static std::string_view printAvailableValues();
		static std::string_view to_string(const ResourceTypeValue& resourceType);
		static const std::unordered_map<std::string_view, const ResourceTypeValue> generateMap();
		static constexpr size_t size();
		static const std::string generateAvailableValues();
	};

	unsigned int runDays;
	unsigned int recordEach;
	unsigned int timeStepsPerDay;
	unsigned int numberOfCombinations;
	SimulType simulType;
	bool growthAndReproTest;
	double massRatio;

	void obtainInhabitableTerrainCells(std::vector<TerrainCell*> &inhabitableTerrainCells);
	void obtainSpeciesInhabitableTerrainCells(std::map<AnimalSpecies*,std::vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells, const std::vector<TerrainCell*> &inhabitableTerrainCells);
	void printActualEcosystemSize(const std::vector<TerrainCell*> &inhabitableTerrainCells);

	void calculateAnimalSpeciesK_Value() const;

	virtual std::map<AnimalSpecies*, std::vector<Animal*>*>* generateStatisticsPopulation(std::vector<std::pair<Animal *, Instar> > &animalAndInstarAtInitialization, std::map<AnimalSpecies*,std::vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells)=0;
	void eraseStatisticsPopulation(std::map<AnimalSpecies*, std::vector<Animal*>*> * animalsPopulation);
	virtual int generatePopulation(AnimalSpecies* currentAnimalSpecies, const std::vector<TerrainCell*> &speciesInhabitableTerrainCells)=0;

	// Creates the terrain cells based on dimensions specified on this object. This should be called
	// right after constructor
	void initializeTerrainDimensions(const nlohmann::json &moistureBasePatch);
	void readObstaclePatchesFromJSONFiles();
	void readMoisturePatchesFromJSONFiles();
	void readAnimalSpeciesFromJSONFiles();
	void readResourceSpeciesFromJSONFiles();
	void initializeResource();
	// Setters. Be aware that setting this values after calling createTerrain will lead to undesired
	// behavior and memory errors. This should always be used prior to create terrain
	void setWidth(unsigned int newWidth);
	void setLength(unsigned int newLength);
	void setDepth(unsigned int newDepth);
	void setInitialEcosystemSize(unsigned long newInitialEcosystemSize);
	void setMaxLogMassRatio(float newMaxLogMassRatio);
	void setExitTimeThreshold(float exitTimeThreshold);
	void setMinLogMassRatio(float newMinLogMassRatio);
	void setSigmaForPDF(float sigmaForPDF);
	void setMuForPDF(float muForPDF);

	void setMaximumTemperature(const double &newValue);

	void setCellSize(float newCellSize);
	void setObstacleFolderName(fs::path newTerrainFolderName);
	void setMoistureFolderName(fs::path newMoistureFolderName);
	void setResourceFolderName(fs::path newResourceFolderName);
	void setSpeciesFolderName(fs::path newSpeciesFolderName);
	void setPeriodLength(double seconds); // In seconds
	// TODO Mario Initialize animals with initIndividualsPerDensities
	/*
	void setHeatingCodeTemperatureCycle(string temperatureFilename); // Celsius
	*/

	void setResourceToPreysCapacityTransference(double resourceToPreysCapacityTransference);
	void setPreysToPredatorsCapacityTransference(double preysToPredatorsCapacityTransference);

	// Returns a pointer to the indicated cell
	TerrainCell* getCell(unsigned int z, unsigned int y, unsigned int x);

	void addResourceSpecies(ResourceSpecies * newSpecies );
	AnimalSpecies* addAnimalSpecies(const json &animalSpeciesInfo);
	ResourceSpecies * getResourceSpecies(std::string name);
	AnimalSpecies * getAnimalSpecies(const std::string& name);
	void addResourceSpeciesOntogeneticLinks(AnimalSpecies* const predatorSpecies, const json &resourceLinks);
	void addAnimalSpeciesOntogeneticLinks(AnimalSpecies* const predatorSpecies, const json &animalLinks);
	void addOntogeneticLinks(const map<AnimalSpecies*, json> &ontogeneticLinks);

	void printAnimalsAlongCells(int day, int simulationPoint);
	void printCellAlongCells(int day);
	void printAnimalsVoracitiesAlongCells(int day);
	std::ostream& printDailySummary(std::ostream& os, int day);
	std::ostream& printExtendedDailySummary(std::ostream& os, int day);
	void printGeneticsSummaries(int day);
	void saveAnimalSpeciesSnapshot(fs::path filenameRoot, std::string filename, int day, AnimalSpecies* species);
	void saveResourceSpeciesSnapshot(fs::path filenameRoot, std::string filename, int day, ResourceSpecies* species);
	void saveWaterSnapshot(fs::path filenameRoot, std::string filename, int day);
	void deleteExtinguishedReproducingAnimals();
	bool isExtinguished();

	void setResourcePatch(const json &patch, ResourceSpecies* const resourceSpecie);
	void setWaterPatch(const json &patch, const std::string &patchFilename);
	
	void setHomogeneousResource(ResourceSpecies* const species, double value, double resourceMaximumCapacity, bool patchSpread);
	void setCubicResourcePatch(ResourceSpecies* const species, Coordinate3D<int> center, Coordinate3D<int> dimensions, double value, double resourceMaximumCapacity, bool patchSpread);
	void setGaussianResourcePatch(ResourceSpecies* const species, unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, float sigma, float amplitude, double resourceMaximumCapacity, bool patchSpread);
	void setSphericalResourcePatch(ResourceSpecies* const species, unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, double value, double resourceMaximumCapacity, bool patchSpread);
	void setRandomGaussianResourcePatches(ResourceSpecies* const species, unsigned int number, float radius, float newSigma, bool useRandomSigma, float newAmplitude, bool useRandomAmplitude, double resourceMaximumCapacity, bool patchSpread);

	void setHomogeneousWater(float value, float relativeHumidityDecayOverTime);
	void setGaussianWaterPatch(unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, float sigma, float amplitude);
	void setCubicWaterPatch(std::string patchFilename, Coordinate3D<int> center, Coordinate3D<int> dimensions,  bool useRelativeHumidityCycle, const std::vector<float>& relativeHumidityCycle, const std::vector<float>& temperatureCycle, bool useRelativeHumidityDecayOverTime, float relativeHumidityDecayOverTime, float relativeHumidityOnRainEvent, int timeStepsBetweenRainEvents, int standardDeviationForRainEvent, float maximumResourceCapacity, bool inEnemyFreeSpace, bool inCompetitorFreeSpace);
	void setCubicObstaclePatch(std::string patchFilename, unsigned int depthStartPoint, unsigned int lengthStartPoint, unsigned int widthStartPoint, unsigned int patchDepth, unsigned int patchLength, unsigned int patchWidth);
	void setSphericalWaterPatch(std::string patchFilename, unsigned int radius, unsigned int xpos, unsigned int ypos, unsigned int zpos, bool useRelativeHumidityCycle, const std::vector<float>& relativeHumidityCycle, const std::vector<float>& temperatureCycle, bool useRelativeHumidityDecayOverTime, float relativeHumidityDecayOverTime, float relativeHumidityDecayInitialValue, int timeStepsBetweenRainEvents, int standardDeviationForRainEvent, float maximumResourceCapacity, bool inEnemyFreeSpace, bool inCompetitorFreeSpace);
	void setRandomGaussianWaterPatches(unsigned int number, float radius, float newSigma, bool useRandomSigma, float newAmplitude, bool useRandomAmplitude );

	friend class TerrainCell;

	fs::path outputFolder;
	fs::path inputFolder;
	std::string encountersMatrixFilename;
	std::string predationsMatrixFilename;
	std::string nodesMatrixFilename;
	std::string predationEventsOnOtherSpeciesFilename;

	ofstream dailySummaryFile;
	ofstream extendedDailySummaryFile;
	ofstream edibilitiesFile;
	bool saveEdibilitiesFile;

	double maximumTemperature;

	unsigned int width; // Number of terrain cells
	unsigned int length; // Number of terrain cells
	unsigned int depth; // Number of terrain cells
	float cellSize;
	fs::path obstacleFolderName;
	fs::path moistureFolderName;
	fs::path resourceFolderName;
	fs::path speciesFolderName;
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
	std::unordered_map<AnimalSpecies*, std::ofstream> geneticsSummaryFile;

	bool saveIntermidiateVolumes;
	unsigned int saveIntermidiateVolumesPeriodicity;

	Terrain3D terrain; // "Voxels" contained in 3D terrain

	/***
	 * Stores species and the number of initial patches/individuals associated
	 */

	//double periodLength;
	std::vector<float> heatingCodeTemperatureCycle;		
};

class OldWorld : public World
{
public:
	OldWorld(json * jsonTree, json &worldConfig, fs::path inputFile, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath)
		: World(jsonTree, worldConfig, inputFile, outputFolder, WeaverFolder, configPath)
	{
		
	}
	~OldWorld() {}

	TerrainCellInterface * getCellByBearing(TerrainCellInterface* position, const TerrainCellInterface* const bestCell);
	std::map<AnimalSpecies*, std::vector<Animal*>*>* generateStatisticsPopulation(std::vector<std::pair<Animal *, Instar> > &animalAndInstarAtInitialization, std::map<AnimalSpecies*,std::vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells);
	int generatePopulation(AnimalSpecies* currentAnimalSpecies, const std::vector<TerrainCell*> &speciesInhabitableTerrainCells);

protected:

};

class Matrix3DWorld : public World
{
public:
	Matrix3DWorld(json * jsonTree, json &worldConfig, fs::path inputFile, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath)
		: World(jsonTree, worldConfig, inputFile, outputFolder, WeaverFolder, configPath)
	{

	}
	~Matrix3DWorld() {}

	TerrainCellInterface * getCellByBearing(TerrainCellInterface* position, const TerrainCellInterface* const bestCell);
	std::map<AnimalSpecies*, std::vector<Animal*>*>* generateStatisticsPopulation(std::vector<std::pair<Animal *, Instar> > &animalAndInstarAtInitialization, std::map<AnimalSpecies*,std::vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells);
	int generatePopulation(AnimalSpecies* currentAnimalSpecies, const std::vector<TerrainCell*> &speciesInhabitableTerrainCells);
	void evolveWorld();
	void finishWorld();

protected:
	
};




// Manage exceptions due to wrong sizes used for world creation
class WorldSizeException
{
private:

	std::string exceptionDescription; // Descriptive exception string
	int errorCode; // Error code used externally to manage exceptions

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
};

#endif // WORLD_HPP_
