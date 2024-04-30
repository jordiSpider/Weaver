#ifndef TERRAINCELL_HPP_
#define TERRAINCELL_HPP_

#include <ios>
#include <iomanip>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <ctime>
#include <map>
#include <tuple>
#include "Animal.h"
#include "Resource.h"
#include "Types.h"
#include "SimulType.h"

#include "nlohmann/json.h"


using namespace std;

class ResourceSpecies;


class TerrainCell
{
protected:

	float size; // Dimensions of the squared voxel
	bool obstacle;

	float C; // Carbon
	float P; // Phosphorus
	float N; // Nitrogen
	float moisture; // Moisture content

	vector<float> temperatureCycle;
	vector<float> relativeHumidityCycle;
	float relativeHumidityOnRainEvent;
	float relativeHumidityDecayOverTime;
	int timeStepsBetweenRainEvents;
	int standardDeviationForRainEvent;

	float temperature; // Current cell temperature (Kelvin degrees)

	float totalMaximumResourceCapacity;
	float totalMinimumResourceCapacity;

	float maximumPreysCapacity;
	float maximumPredatorsCapacity;

	float auxInitialResourceBiomass;
	int auxInitialResourceSpeciesId;

	//arthro and for dinos
	bool inPatch = false;
	bool inEnemyFreeSpace;
	bool inCompetitorFreeSpace;

	uiPoint3D myCoordinates; // Where am I located in my reference framework? (world)

	vector<Resource *> resources; // Stores all the resource living within this cell

	// The maps are intended to store a list for each phenological stage so traversing animals
	// becomes more efficient. One unsigned int per phenological stage
	map<LifeStage, map<AnimalSpecies* , vector<Animal*> *> * > animals; // Stores all the animals living within this cell


	//MutexType animalsToMoveMutex;
	//MutexType::scoped_lock animalsToMoveLock;
	vector<Animal*> * animalsToMove;
	vector<Animal*> * animalsToReproduce;
	vector<Animal*> * animalsToMetabolizeAndGrowth;

	// This map is for speed-up purposes only. Once a given distance has been calculated, it is stored here
	// for a latter use
	map<unsigned int, vector<TerrainCell *> *> cellsWithinDistance; // Neighbor cells within a given distance
	map<unsigned int, vector<TerrainCell *> *> cellsAtDistance; // Neighbor cells at an exact given distance

	std::vector<AnimalSpecies*> *existingAnimalSpecies;
	std::vector<ResourceSpecies*> *existingResourceSpecies;


	void checkMaximumResourceCapacity();

public:
	TerrainCell(int x, int y, int z, float newSize, const nlohmann::json &moistureBasePatch, std::vector<AnimalSpecies*> *existingAnimalSpecies, std::vector<ResourceSpecies*> *existingResourceSpecies);
	virtual ~TerrainCell();
	float getSize();

	unsigned int getNumberOfMatureFemales(Animal* reproducingAnimal);
	/*
	 * Return the number of predatos in this cell that are in an ACTIVE state
	 */

	inline vector<Animal*> * getAnimalsBySpecies(const LifeStage &lifeStage, AnimalSpecies* species) { return animals.at(lifeStage)->at(species); }
	
	unsigned int getNumberOfEdibleAnimals(Animal* animalWhoIsEvaluating);
	tuple<double, double, double> getCellEvaluation(Animal* animalWhoIsEvaluating, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);

	void activateAndResumeAnimals(int timeStep, int timeStepsPerDay);
	//void activateAnimalsThreaded(int timeStep);
	void tuneTraits(int timeStep, int timeStepsPerDay, std::FILE* tuneTraitsFile, SimulType simulType, TerrainCell*(*getCell)(unsigned int, unsigned int, unsigned int));
	//void tuneTraitsThreaded(int timeStep);
	//bool searchAnimalsToEat(Animal* predator, int day, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile);


	map<Species *, double> * getRateOfResourcePredators(Animal* animalWhoIsEvaluating, bool discardZero = true);

	map<Species*, double> * getResourceStock(Animal* animalWhoIsEvaluating, bool discardZero = true);

	void migrateAnimalTo(Animal* animalToMigrate, TerrainCell* newPosition);

	double getTotalResourceBiomass();

	float getTemperature();
	const vector<float>& getTemperatureCycle() const {return temperatureCycle;};

	float getWater();
	float getC();
	float getP();
	float getN();

	void printInteractionMatrices(ostream& encountersMatrixFile, ostream& predationsMatrixFile,
			ostream& nodesMatrixFile, unsigned int totalInitialPopulation);

	/*
	 * Returns a vector with all those cells within the searchArea distance from the
	 * current cell.
	 */
	std::vector<TerrainCell *> * getCellsWithinDistance(int maxDistance, double searchArea, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, TerrainCell*(*getCell)(unsigned int, unsigned int, unsigned int));

	/*
	 * Returns a vector with all those cells located on a cube shell (the boundaries of the cube)
	 * of side=2*distance from the current cell.
	 */
	std::vector<TerrainCell *> * getCellsAtDistance(int distance, double searchArea, bool isMobile, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, TerrainCell*(*getCell)(unsigned int, unsigned int, unsigned int));

	void setSize(float newSize);

	void updateTemperature(int timeStep);
	void updateRelativeHumidity(int timeStep);

	void chemostatEffect();
	void setTemperatureCycle(const vector<float>& temperatureCycle);
	void setRelativeHumidityCycle(const vector<float>& relativeHumidityCycle);
	void setRelativeHumidityDecayOverTime(float relativeHumidityDecayOverTime);
	void setRelativeHumidityOnRainEvent(float newWater);
	void setTimeStepsBetweenRainEvents(int timeStepsBetweenRainEvents);
	void setStandardDeviationForRainEvent(float standardDeviationForRainEvent);

	void setC(float newC);
	void setP(float newP);
	void setN(float newN);
	void setElements(float newC, float newN, float newP);
	void setMaximumCapacities(float totalMaximumResourceCapacity);
	void setInEnemyFreeSpace(bool inEnemyFreeSpace);
	void setInCompetitorFreeSpace(bool inCompetitorFreeSpace);

	void printStoichiometry(bool includeWater = false);
	void setAuxInitialResourceBiomass(float auxInitialResourceBiomass, int auxInitialResourceSpeciesId);
	void addResource(Resource* newResource);
	void deleteResource(Resource* edible);
	Resource* getResource(ResourceSpecies* species);
	vector<Resource *>* getResources();
	bool containsResourceSpecies(ResourceSpecies* species) { return getResource(species) != NULL; };
	float getResourceBiomass(ResourceSpecies* species);
	double getDistanceToCell(TerrainCell* targetCell);

	void addAnimal(Animal* newAnimal);
	double turnEdibleIntoDryMassToBeEaten(Edible* targetAnimal, int day, Animal* predatorEdible, double leftovers);
	void changeAnimalToSenesced(Animal* targetAnimal, int day);
	void eraseAnimal(Animal* oldEdible, const LifeStage &oldStage);

	unsigned int getLifeStagePopulation(const LifeStage &stage, AnimalSpecies* species);
	unsigned int getLifeStagePopulation(const LifeStage &stage, const HuntingMode::HuntingModeValue &huntingMode);
	void updateWorldResourceBiomassAndAnimalsPopulation(map<ResourceSpecies*, double>* worldResourceBiomass, map<AnimalSpecies*, vector<unsigned int>* >* worldAnimalsPopulation);
	void updateAnimalsPopulationAndGeneticsFrequencies(map<AnimalSpecies*, vector<unsigned int>* >* worldAnimalsPopulation, map<AnimalSpecies*, vector<set<double>* >* >* worldGeneticsFrequencies);
	void metabolizeAnimals(int timeStep, int timeStepPerDay, SimulType simulType);
	void growAnimals(int timeStep, int timeStepsPerDay);
	void diePredatorsFromBackground(int day, bool growthAndReproTest);
	void breedAnimals(int timeStep, int timeStepsPerDay, fs::path outputDirectory, bool saveAnimalConstitutiveTraits, FILE* constitutiveTraitsFile);
	void setResourceFromChemostatEffect(vector<ResourceSpecies *>* existingResourceSpecies, double increaseForChemostatEffect, bool competitionAmongResourceSpecies, double massRatio);
	void growResource(int timeStep, bool competition, double maxCapacity, int size, SimulType simulType, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, TerrainCell*(*getCell)(unsigned int, unsigned int, unsigned int), bool competitionAmongResourceSpecies, double massRatio);
	void commitResourceSpread();
	void moveAnimals(int day, int timeStepsPerDay, FILE* encounterProbabilitiesFile, FILE* predationProbabilitiesFile, bool saveEdibilitiesFile, FILE* edibilitiesFile, float exitTimeThreshold, TerrainCell*(*getCellByBearing)(TerrainCell*, TerrainCell*), unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, TerrainCell*(*getCell)(unsigned int, unsigned int, unsigned int), double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
	void purgeDeadAnimals();
	void deleteExtinguishedReproducingAnimals(AnimalSpecies* extinguishedAnimalSpecies);
	void assimilateFoodMass(int timeStep);

	void printAnimalsVoracities(int timeStep, int timeStepsPerDay, FILE* voracitiesFile, SimulType simulType);
	void printAnimals(FILE* file);
	void printCell(FILE* file);
    int getX() {return myCoordinates.getX();};
    int getY() {return myCoordinates.getY();};
    int getZ() {return myCoordinates.getZ();};
    inline vector<Animal*> * getAnimalsToMove() { return animalsToMove; };

    //void lockAnimalsToMove() { animalsToMoveLock.acquire(animalsToMoveMutex); };
    //void unlockAnimalsToMove() { animalsToMoveLock.release(); };

/*
//#ifdef _PTHREAD
	struct ThreadArgument
	{
			TerrainCell* terrainCell;
			int timeStep;
			int timeStepsPerDay;
			void (TerrainCell::*function)();
	};
	static void* activateAnimalsThreadMaker(void* threadArgument);
	static void* tuneTraitsThreadMaker(void* threadArgument);
//#endif
*/

#ifdef _DEBUG
	void increaseMultipleSameSearchedAnimalToday();
	void increaseMultipleSameEncounteredAnimalToday();
	void increaseMultipleSameHuntedAnimalToday();
	void increaseMultipleSamePredatedAnimalToday();
#endif

	float getTotalMaximumResourceCapacity()  {  //const   //Dinosaurs debug remove constant
		return totalMaximumResourceCapacity;
	}

	float getMaximumPreysCapacity() const {
		return maximumPreysCapacity;
	}

	bool isObstacle() const {
		return obstacle;
	}

	void setObstacle(bool obstacle) {
		this->obstacle = obstacle;
	}

	
	//arthro also for dino
	bool isInPatch() {
		return inPatch;
	}

	void setInPatch(bool inPatch) {
		this->inPatch = inPatch;
	}

	//arthro also for dino
	bool isInEnemyFreeSpace() {
		return inEnemyFreeSpace;
	}

	bool isInCompetitorFreeSpace() {
		return inCompetitorFreeSpace;
	}
	//end arthro
};

/*
#ifdef _TBB
class GrowResourceTaskContinuation: public task {

public:
	//Continuation task is only useful to gather the results
	GrowResourceTaskContinuation() {
	}

	task* execute() {
		return NULL;
	}
};

class GrowResourceTask: public task {
public:
	unsigned int x0, x1, y0, y1, z0, z1;
	Terrain3D& terrain;
	int timeStepsPerDay;
    GrowResourceTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, Terrain3D& terrain, int timeStepsPerDay) :
    	x0(x0), x1(x1), y0(y0), y1(y1), z0(z0), z1(z1), terrain(terrain), timeStepsPerDay(timeStepsPerDay){}

    task* execute() {     // Overrides virtual function task::execute

    	TerrainCell* currentTerrainCell = NULL;
		for (unsigned int z = z0; z < z1; z++)
		{
			for (unsigned int y = y0; y < y1; y++)
			{
				for (unsigned int x = x0; x < x1; x++)
				{
					currentTerrainCell = terrain[z][y][x];
					if(currentTerrainCell != NULL)
					{
						currentTerrainCell->growResource(timeStepsPerDay);
					}
				}
			}
		}
    	return NULL;
    }

};

class GrowResourceRangerTask: public task {
public:
	unsigned int xWidth, yLength, zDepth;
	Terrain3D& terrain;
	int timeStepsPerDay;
    GrowResourceRangerTask(unsigned int xWidth, unsigned int yLength, unsigned int zDepth, Terrain3D& terrain, int timeStepsPerDay) :
    	xWidth(xWidth), yLength(yLength), zDepth(zDepth), terrain(terrain), timeStepsPerDay(timeStepsPerDay){}

    task* execute() {     // Overrides virtual function task::execute
    	unsigned int xSliceSize = xWidth / PARTITIONS_PER_DIMENSION;
    	unsigned int ySliceSize = yLength / PARTITIONS_PER_DIMENSION;
    	unsigned int zSliceSize = zDepth / PARTITIONS_PER_DIMENSION;
    	unsigned int x0, x1, y0, y1;

    	//MoveAnimalsTaskContinuation& continuation = *new (allocate_continuation()) MoveAnimalsTaskContinuation();
    	task_list growResourceTaskList;

    	for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
    	{
    		x0 = i*xSliceSize;
    		x1 = (i!=PARTITIONS_PER_DIMENSION-1)?((i+1)*xSliceSize):xWidth;
			for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
			{
				y0 = j*ySliceSize;
				y1 = (j!=PARTITIONS_PER_DIMENSION-1)?((j+1)*ySliceSize):yLength;
				for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
				{
					growResourceTaskList.push_back(*new (allocate_child()) GrowResourceTask(x0, x1, y0, y1, k*zSliceSize, (k!=PARTITIONS_PER_DIMENSION-1)?((k+1)*zSliceSize):zDepth, terrain, timeStepsPerDay));
				}
			}
    	}

    	set_ref_count(PARTITIONS_PER_DIMENSION*PARTITIONS_PER_DIMENSION*PARTITIONS_PER_DIMENSION+1);
    	spawn_and_wait_for_all(growResourceTaskList);
    	return NULL;
    }
};

class MoveAnimalsTaskContinuation: public task {

public:
	//Continuation task is only useful to gather the results
	MoveAnimalsTaskContinuation() {
	}

	task* execute() {
		return NULL;
	}
};

class ActivateAnimalsTask: public task {
public:
	unsigned int x0, x1, y0, y1, z0, z1;
	Terrain3D& terrain;
	int timeStep;
	int timeStepsPerDay;

	ActivateAnimalsTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, Terrain3D& terrain, int timeStep, int timeStepsPerDay) :
    	x0(x0), x1(x1), y0(y0), y1(y1), z0(z0), z1(z1), terrain(terrain), timeStep(timeStep), timeStepsPerDay(timeStepsPerDay) {}

    task* execute() {     // Overrides virtual function task::execute

    	TerrainCell* currentTerrainCell = NULL;
		for (unsigned int z = z0; z < z1; z++)
		{
			for (unsigned int y = y0; y < y1; y++)
			{
				for (unsigned int x = x0; x < x1; x++)
				{
					currentTerrainCell = terrain[z][y][x];
					if(currentTerrainCell != NULL)
					{
						currentTerrainCell->activateAndResumeAnimals(timeStep, timeStepsPerDay);
						currentTerrainCell->tuneTraits(timeStep, timeStepsPerDay, cout);
					}
				}
			}
		}
    	return NULL;
    }

};

class MoveAnimalsTask: public task {
public:
	unsigned int x0, x1, y0, y1, z0, z1;
	Terrain3D& terrain;
	int timeStep;
	int timeStepsPerDay;

	MoveAnimalsTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, Terrain3D& terrain, int timeStep, int timeStepsPerDay) :
    	x0(x0), x1(x1), y0(y0), y1(y1), z0(z0), z1(z1), terrain(terrain), timeStep(timeStep), timeStepsPerDay(timeStepsPerDay) {}

    task* execute() {     // Overrides virtual function task::execute

    	TerrainCell* currentTerrainCell = NULL;
		for (unsigned int z = z0; z < z1; z++)
		{
			for (unsigned int y = y0; y < y1; y++)
			{
				for (unsigned int x = x0; x < x1; x++)
				{
					currentTerrainCell = terrain[z][y][x];
					if(currentTerrainCell != NULL)
					{
						currentTerrainCell->moveAnimals(timeStep, timeStepsPerDay, cout, cout);
					}
				}
			}
		}
    	return NULL;
    }

};

class ActivateAnimalsRangerTask: public task {
public:
	unsigned int xWidth, yLength, zDepth;
	Terrain3D& terrain;
	int day;
	int timeStepsPerDay;

	ActivateAnimalsRangerTask(unsigned int xWidth, unsigned int yLength, unsigned int zDepth, Terrain3D& terrain, int day, int timeStepsPerDay) :
    	xWidth(xWidth), yLength(yLength), zDepth(zDepth), terrain(terrain), day(day), timeStepsPerDay(timeStepsPerDay) {}

    task* execute() {     // Overrides virtual function task::execute
    	unsigned int xSliceSize = xWidth / PARTITIONS_PER_DIMENSION;
    	unsigned int ySliceSize = yLength / PARTITIONS_PER_DIMENSION;
    	unsigned int zSliceSize = zDepth / PARTITIONS_PER_DIMENSION;
    	unsigned int x0, x1, y0, y1;

    	//MoveAnimalsTaskContinuation& continuation = *new (allocate_continuation()) MoveAnimalsTaskContinuation();
    	task_list activateAnimalsTaskList;

    	for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
    	{
    		x0 = i*xSliceSize;
    		x1 = (i!=PARTITIONS_PER_DIMENSION-1)?((i+1)*xSliceSize):xWidth;
			for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
			{
				y0 = j*ySliceSize;
				y1 = (j!=PARTITIONS_PER_DIMENSION-1)?((j+1)*ySliceSize):yLength;
				for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
				{
					activateAnimalsTaskList.push_back(*new (allocate_child()) ActivateAnimalsTask(x0, x1, y0, y1, k*zSliceSize, (k!=PARTITIONS_PER_DIMENSION-1)?((k+1)*zSliceSize):zDepth, terrain, day, timeStepsPerDay));
				}
			}
    	}

    	set_ref_count(PARTITIONS_PER_DIMENSION*PARTITIONS_PER_DIMENSION*PARTITIONS_PER_DIMENSION+1);
    	spawn_and_wait_for_all(activateAnimalsTaskList);
    	return NULL;
    }

};

class MoveAnimalsRangerTask: public task {
public:
	unsigned int xWidth, yLength, zDepth;
	Terrain3D& terrain;
	int timeStep;
	int timeStepsPerDay;

	MoveAnimalsRangerTask(unsigned int xWidth, unsigned int yLength, unsigned int zDepth, Terrain3D& terrain, int timeStep, int timeStepsPerDay) :
    	xWidth(xWidth), yLength(yLength), zDepth(zDepth), terrain(terrain), timeStep(timeStep), timeStepsPerDay(timeStepsPerDay) {}

    task* execute() {     // Overrides virtual function task::execute
    	unsigned int xSliceSize = xWidth / PARTITIONS_PER_DIMENSION;
    	unsigned int ySliceSize = yLength / PARTITIONS_PER_DIMENSION;
    	unsigned int zSliceSize = zDepth / PARTITIONS_PER_DIMENSION;
    	unsigned int x0, x1, y0, y1;

    	//MoveAnimalsTaskContinuation& continuation = *new (allocate_continuation()) MoveAnimalsTaskContinuation();
    	task_list moveAnimalsTaskList;

    	for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
    	{
    		x0 = i*xSliceSize;
    		x1 = (i!=PARTITIONS_PER_DIMENSION-1)?((i+1)*xSliceSize):xWidth;
			for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
			{
				y0 = j*ySliceSize;
				y1 = (j!=PARTITIONS_PER_DIMENSION-1)?((j+1)*ySliceSize):yLength;
				for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
				{
					moveAnimalsTaskList.push_back(*new (allocate_child()) MoveAnimalsTask(x0, x1, y0, y1, k*zSliceSize, (k!=PARTITIONS_PER_DIMENSION-1)?((k+1)*zSliceSize):zDepth, terrain, timeStep, timeStepsPerDay));
				}
			}
    	}

    	set_ref_count(PARTITIONS_PER_DIMENSION*PARTITIONS_PER_DIMENSION*PARTITIONS_PER_DIMENSION+1);
    	spawn_and_wait_for_all(moveAnimalsTaskList);
    	return NULL;
    }
};
#endif //_TBB
*/

#endif
