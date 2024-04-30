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
#include "FungusSpecies.h"
#include "AnimalSpecies.h"
#include "PlantSpecies.h"
#include "Maths/GaussianDistribution.h"
#include "Maths/Random.h"
#include "Edible.h"
#include "Animal.h"
#include "Types.h"

#include <sys/time.h>
#include <ctime>
/*
#include "tbb/task.h"
#include "tbb/task_group.h"
#include "tbb/tick_count.h"
#include "tbb/concurrent_vector.h"
*/

// Library nlohmann JSON
#include "nlohmann/json.h"

using json = nlohmann::json;

class TerrainCell;

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

class World
{
private:
	
	unsigned int runDays;
	unsigned int recordEach;
	unsigned int timeStepsPerDay;
	unsigned int numberOfCombinations;
	unsigned int simulType;
	

	void obtainInhabitableTerrainCells(vector<TerrainCell*> &inhabitableTerrainCells);
	void obtainSpeciesInhabitableTerrainCells(map<Species,vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells, const vector<TerrainCell*> &inhabitableTerrainCells);
	void printActualEcosystemSize(const vector<TerrainCell*> &inhabitableTerrainCells);

	map<Species, vector<Edible*>*>* generateStatisticsPopulation(vector<pair<Animal *, Instar> > &animalAndInstarAtInitialization, map<Species,vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells);
	void eraseStatisticsPopulation(map<Species, vector<Edible*>*> * animalsPopulation);
	int generatePopulation(Species* currentAnimalSpecies, const vector<TerrainCell*> &speciesInhabitableTerrainCells, ofstream &constitutiveTraitsFile);

	// Creates the terrain cells based on dimensions specified on this object. This should be called
	// right after constructor
	void initializeTerrainDimensions();
	void readObstaclePatchesFromJSONFiles();
	void readMoisturePatchesFromJSONFiles();
	void readAnimalSpeciesFromJSONFiles();
	void readFungusSpeciesFromJSONFiles();
	void initializeFungi();
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

	void setCellSize(float newCellSize);
	void setObstacleDirectoryName(fs::path newTerrainDirectoryName);
	void setMoistureDirectoryName(fs::path newMoistureDirectoryName);
	void setFungiDirectoryName(fs::path newFungiDirectoryName);
	void setSpeciesDirectoryName(fs::path newSpeciesDirectoryName);
	void setPeriodLength(double seconds); // In seconds
	// TODO Mario Initialize animals with initIndividualsPerDensities
	/*
	void setHeatingCodeTemperatureCycle(string temperatureFilename); // Celsius
	*/

	void setFungiToPreysCapacityTransference(double fungiToPreysCapacityTransference);
	void setPreysToPredatorsCapacityTransference(double preysToPredatorsCapacityTransference);

	// Returns a pointer to the indicated cell
	TerrainCell * getCell(unsigned int z, unsigned int y, unsigned int x);

	void addFungusSpecies(FungusSpecies * newSpecies );
	void addAnimalSpecies(AnimalSpecies * newSpecies );
	void addPlantSpecies(PlantSpecies * newSpecies, unsigned int numberOfInitialPopulation);
	Species * getFungusSpecies(string name);
	Species * getAnimalSpecies(string name);

	void printAnimalsAlongCells(int day, int simulationPoint);
	void printCellAlongCells(int day);
	void printAnimalsVoracitiesAlongCells(int day);
	ostream& printDailySummary(ostream& os, int day);
	ostream& printExtendedDailySummary(ostream& os, int day);
	void printGeneticsSummaries(int day);
	void saveAnimalSpeciesSnapshot(fs::path filenameRoot, int day, Species* species);
	void saveFungusSpeciesSnapshot(fs::path filenameRoot, int day, Species* species);
	void saveWaterSnapshot(fs::path filenameRoot, int day);
	void deleteExtinguishedReproducingAnimals();
	bool isExtinguished();
	
	void setHomogeneousFungus(Species* species, double value);
	void setCubicFungusPatch(Species* species, Coordinate3D<int> center, Coordinate3D<int> dimensions, double value);
	void setGaussianFungusPatch(Species* species, unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, float sigma, float amplitude);
	void setSphericalFungusPatch(Species* species, unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, double value);
	void setRandomGaussianFungusPatches(Species* species, unsigned int number, float radius, float newSigma, bool useRandomSigma, float newAmplitude, bool useRandomAmplitude);

	void setHomogeneousWater(float value, float relativeHumidityDecayOverTime);
	void setGaussianWaterPatch(unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, float sigma, float amplitude);
	void setCubicWaterPatch(string patchFilename, Coordinate3D<int> center, Coordinate3D<int> dimensions,  bool useRelativeHumidityCycle, const vector<float>& temperatureCycle, const vector<float>& relativeHumidityCycle, bool useRelativeHumidityDecayOverTime, float relativeHumidityOnRainEvent, float relativeHumidityDecayOverTime, int timeStepsBetweenRainEvents, int standardDeviationForRainEvent, float maximumFungiCapacity, bool inEnemyFreeSpace, bool inCompetitorFreeSpace);
	void setCubicObstaclePatch(string patchFilename, unsigned int depthStartPoint, unsigned int lengthStartPoint, unsigned int widthStartPoint, unsigned int patchDepth, unsigned int patchLength, unsigned int patchWidth);
	void setSphericalWaterPatch(string patchFilename, unsigned int radius, unsigned int xpos, unsigned int ypos, unsigned int zpos, bool useRelativeHumidityCycle, const vector<float>& temperatureCycle, const vector<float>& relativeHumidityCycle, bool useRelativeHumidityDecayOverTime, float relativeHumidityDecayInitialValue, float relativeHumidityDecayOverTime, int timeStepsBetweenRainEvents, int standardDeviationForRainEvent, float maximumFungiCapacity, bool inEnemyFreeSpace, bool inCompetitorFreeSpace);
	void setRandomGaussianWaterPatches(unsigned int number, float radius, float newSigma, bool useRandomSigma, float newAmplitude, bool useRandomAmplitude );

protected:

	friend class TerrainCell;

	fs::path outputDirectory;
	fs::path inputDirectory;
	string encountersMatrixFilename;
	string predationsMatrixFilename;
	string nodesMatrixFilename;
	string predationEventsOnOtherSpeciesFilename;

	ofstream dailySummaryFile;
	ofstream extendedDailySummaryFile;
	ofstream geneticsSummaryFile;
	ofstream edibilitiesFile;

	unsigned int width; // Number of terrain cells
	unsigned int length; // Number of terrain cells
	unsigned int depth; // Number of terrain cells
	float cellSize;
	fs::path obstacleDirectoryName;
	fs::path moistureDirectoryName;
	fs::path fungiDirectoryName;
	fs::path speciesDirectoryName;
	bool initIndividualsPerDensities;
	bool competitionAmongFungiSpecies;
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

	float fungiToPreysCapacityTransference;
	float preysToPredatorsCapacityTransference;

	bool saveGeneticsSummaries;
	bool saveIntermidiateVolumes;
	unsigned int saveIntermidiateVolumesPeriodicity;

	Terrain3D terrain; // "Voxels" contained in 3D terrain

	/***
	 * Stores species and the number of initial patches/individuals associated
	 */

	//double periodLength;
	vector<float> heatingCodeTemperatureCycle;

public:
	vector<Species*> existingFungiSpecies;
	vector<Species*> existingAnimalSpecies;
	map<PlantSpecies *, unsigned int> existingPlantSpecies;

	// Constructors
	// Recover from previously stored world
	World(std::string filesNameRoot);

	// Create a new, empty world
	World(json * jsonTree, fs::path outputDirectory);
	~World(); // Destructor

	void calculateAttackStatistics(map<Species,vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells);
	void updateMaxSearchArea(double currentAnimalMaxSearchArea);

	void initializeAnimals();

	void evolveWorld();

	// Removes all existing terrain cells and removes all terrain. This cannot be undone
	void eraseTerrain();

	// Empties existing world, erases and recreates a new one with new dimensions
	void restart(unsigned int newWidth, unsigned int newLength, unsigned int newDepth, float newCellSize);

	void setOutputDirectory(fs::path outputDirectory);
	void printPredationEventsOnOtherSpeciesMatrix(ostream& predationEventsOnOtherSpeciesFile);
	void printInteractionMatrices(ostream& encountersMatrixFile, ostream& predationsMatrixFile, ostream& nodesMatrixFile);

	// Getters
	int getWidth();
	int getLength();
	int getDepth();
	bool getCompetitionAmongFungiSpecies();
	float getExitTimeThreshold();
	TerrainCell * getCellByBearing(TerrainCell* position, TerrainCell* bestCell);
	float getMinLogMassRatio() {return minLogMassRatio;};
	float getMaxLogMassRatio() {return maxLogMassRatio;};
	float getSigmaForPDF() {return sigmaForPDF;};
	float getMuForPDF() {return muForPDF;};
	double getMaxSearchArea() {return maxSearchArea;};
	const vector<Species *> * getExistingAnimalSpecies();
	float getCellSize();
	float getMinWater();
	float getMaxWater();
	int getNumberOfWaterPatches();
	int getWaterPatchWidth();
	float getWaterPatchesAmplitude();
	float getWaterPatchesSigma();
	double getPeriodLength();
	unsigned int getSimulType() { return simulType;};

	// Saves everything so simulation can be snapshoted and recovered later
	void save(std::string filesNameRoot);

	// Read water from stored volume
	void readWaterFromVolume(std::string fileName);
	string translateLifeStage(unsigned int lifeStage);
	void initializeOutputFiles();

		// Returns a pointer to the indicated cell
	TerrainCell * getCell2(unsigned int z, unsigned int y, unsigned int x);

	bool exitAtFirstExtinction;
	//string simulTypeString;
	
#ifdef _PTHREAD
		struct ThreadRangerArgument
		{
				World* world;
				unsigned int x0, x1, y0, y1, z0, z1;
				int day;
		};
		static void* activateAnimalsThreadMaker(void* threadArgument);
		void* activateAnimalsTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, int day);
		static void* growFungiThreadMaker(void* threadArgument);
		void* growFungiTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1);
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

	float getPredationSpeedRatioAh() const {
		return predationSpeedRatioAH;
	}

	void setPredationSpeedRatioAh(float predationSpeedRatioAh) {
		predationSpeedRatioAH = predationSpeedRatioAh;
	}

	float getPredationSpeedRatioSaw() const {
		return predationSpeedRatioSAW;
	}

	void setPredationSpeedRatioSaw(float predationSpeedRatioSaw) {
		predationSpeedRatioSAW = predationSpeedRatioSaw;
	}

		
			
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
