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
#include "Types.h"
#include "Fungus.h"
#include "Animal.h"
/*
#include "tbb/task.h"
*/
#include "World.h"

using namespace std;

// Forward declaration. Needed to avoid "#include loop"
class World;
class Edible;
class Animal;
class Fungus;

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

	float maximumFungiCapacity;
	float minimumFungiCapacity;

	float maximumPreysCapacity;
	float maximumPredatorsCapacity;

	float auxInitialFungiBiomass;
	int auxInitialFungusSpeciesId;

	//arthro and for dinos
	bool inPatch = false;
	bool inEnemyFreeSpace;
	bool inCompetitorFreeSpace;

	World * theWorld; // What world do I belong to?
	uiPoint3D myCoordinates; // Where am I located in my reference framework? (world)

	vector<Edible *> fungi; // Stores all the fungi living within this cell

	// The maps are intended to store a list for each phenological stage so traversing animals
	// becomes more efficient. One unsigned int per phenological stage
	map<unsigned int, map<Species* , vector<Edible*> *> * > animals; // Stores all the animals living within this cell


	//MutexType animalsToMoveMutex;
	//MutexType::scoped_lock animalsToMoveLock;
	vector<Edible*> * animalsToMove;
	vector<Edible*> * animalsToReproduce;
	vector<Edible*> * animalsToMetabolizeAndGrowth;

	// This map is for speed-up purposes only. Once a given distance has been calculated, it is stored here
	// for a latter use
	map<unsigned int, vector<TerrainCell *> *> cellsWithinDistance; // Neighbor cells within a given distance
	map<unsigned int, vector<TerrainCell *> *> cellsAtDistance; // Neighbor cells at an exact given distance

public:

	friend class Animal;
	friend class Fungus;

	TerrainCell(World * newWorld, int x, int y, int z, float newSize);
	virtual ~TerrainCell();
	float getSize();

	unsigned int getNumberOfMatureFemales(Animal* reproducingAnimal);
	/*
	 * Return the number of predatos in this cell that are in an ACTIVE state
	 */

	vector<Edible*> * getAnimalsBySpecies(unsigned int lifeStage, Species* species);
	
	unsigned int getNumberOfEdibleAnimals(Animal* animalWhoIsEvaluating);
	tuple<double, double, double> getCellEvaluation(Edible* animalWhoIsEvaluating);

	void activateAndResumeAnimals(int timeStep, int timeStepsPerDay);
	//void activateAnimalsThreaded(int timeStep);
	void tuneTraits(int timeStep, int timeStepsPerDay, ostream& tuneTraitsFile);
	//void tuneTraitsThreaded(int timeStep);
	//bool searchAnimalsToEat(Animal* predator, int day, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile);


	map<Species *, double> * getRateOfFungiPredators(Animal* animalWhoIsEvaluating, bool discardZero = true);

	map<Species*, double> * getFungiStock(Animal* animalWhoIsEvaluating, bool discardZero = true);

	void migrateAnimalTo(Animal* animalToMigrate, TerrainCell* newPosition);

	double getTotalFungusBiomass();

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
	std::vector<TerrainCell *> * getCellsWithinDistance(int maxDistance, double searchArea);

	/*
	 * Returns a vector with all those cells located on a cube shell (the boundaries of the cube)
	 * of side=2*distance from the current cell.
	 */
	std::vector<TerrainCell *> * getCellsAtDistance(int distance, double searchArea, bool isMobile);

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
	void setMaximumCapacities(float maximumFungiCapacity);
	void setInEnemyFreeSpace(bool inEnemyFreeSpace);
	void setInCompetitorFreeSpace(bool inCompetitorFreeSpace);

	void printStoichiometry(bool includeWater = false);
	void setAuxInitialFungiBiomass(float auxInitialFungiBiomass, int auxInitialFungusSpeciesId);
	void addFungus(Fungus* newFungus);
	void deleteFungus(Edible* edible);
	Edible* getFungus(Species* species);
	vector<Edible *>* getFungi();
	bool containsFungusSpecies(Species* species) { return getFungus(species) != NULL; };
	float getFungusBiomass(Species* species);
	double getDistanceToCell(TerrainCell* targetCell);

	void addAnimal(Edible* newAnimal);
	double turnEdibleIntoDryMassToBeEaten(Edible* targetAnimal, int day, Edible* predatorEdible, double leftovers);
	void changeAnimalToSenesced(Edible* targetAnimal, int day);
	void eraseAnimal(Edible* oldEdible, int oldStage);

	unsigned int getLifeStagePopulation(unsigned int stage, Species* species);
	unsigned int getLifeStagePopulation(unsigned int stage, unsigned int huntingMode);
	void updateWorldFungiBiomassAndAnimalsPopulation(map<Species*, double>* worldFungiBiomass, map<Species*, vector<unsigned int>* >* worldAnimalsPopulation);
	void updateAnimalsPopulationAndGeneticsFrequencies(map<Species*, vector<unsigned int>* >* worldAnimalsPopulation, map<Species*, vector<set<double>* >* >* worldGeneticsFrequencies);
	void metabolizeAnimals(int timeStep, int timeStepPerDay);
	void growAnimals(int timeStep, int timeStepsPerDay);
	void diePredatorsFromBackground(int day);
	void breedAnimals(int timeStep, int timeStepsPerDay, fs::path outputDirectory);
	void setFungiFromChemostatEffect(vector<Species *>* existingFungiSpecies, double increaseForChemostatEffect);
	void growFungi(int timeStep, bool competition, double maxCapacity, int size);
	void commitFungiSpread();
	void moveAnimals(int day, int timeStepsPerDay, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile, float exitTimeThreshold);
	void purgeDeadAnimals();
	void deleteExtinguishedReproducingAnimals(Species* extinguishedAnimalSpecies);
	void assimilateFoodMass(int timeStep);

	ostream& printAnimalsVoracities(int timeStep, int timeStepsPerDay, ostream& os);
	ostream& printAnimals(ostream& os);
	ostream& printCell(ostream& os);
    int getX() {return myCoordinates.X();};
    int getY() {return myCoordinates.Y();};
    int getZ() {return myCoordinates.Z();};
    World* getTheWorld() {return theWorld;};
    vector<Edible*> * getAnimalsToMove() { return animalsToMove; };

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

	float getMaximumFungiCapacity()  {  //const   //Dinosaurs debug remove constant
		return maximumFungiCapacity;
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
class GrowFungiTaskContinuation: public task {

public:
	//Continuation task is only useful to gather the results
	GrowFungiTaskContinuation() {
	}

	task* execute() {
		return NULL;
	}
};

class GrowFungiTask: public task {
public:
	unsigned int x0, x1, y0, y1, z0, z1;
	Terrain3D& terrain;
	int timeStepsPerDay;
    GrowFungiTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, Terrain3D& terrain, int timeStepsPerDay) :
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
						currentTerrainCell->growFungi(timeStepsPerDay);
					}
				}
			}
		}
    	return NULL;
    }

};

class GrowFungiRangerTask: public task {
public:
	unsigned int xWidth, yLength, zDepth;
	Terrain3D& terrain;
	int timeStepsPerDay;
    GrowFungiRangerTask(unsigned int xWidth, unsigned int yLength, unsigned int zDepth, Terrain3D& terrain, int timeStepsPerDay) :
    	xWidth(xWidth), yLength(yLength), zDepth(zDepth), terrain(terrain), timeStepsPerDay(timeStepsPerDay){}

    task* execute() {     // Overrides virtual function task::execute
    	unsigned int xSliceSize = xWidth / PARTITIONS_PER_DIMENSION;
    	unsigned int ySliceSize = yLength / PARTITIONS_PER_DIMENSION;
    	unsigned int zSliceSize = zDepth / PARTITIONS_PER_DIMENSION;
    	unsigned int x0, x1, y0, y1;

    	//MoveAnimalsTaskContinuation& continuation = *new (allocate_continuation()) MoveAnimalsTaskContinuation();
    	task_list growFungiTaskList;

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
					growFungiTaskList.push_back(*new (allocate_child()) GrowFungiTask(x0, x1, y0, y1, k*zSliceSize, (k!=PARTITIONS_PER_DIMENSION-1)?((k+1)*zSliceSize):zDepth, terrain, timeStepsPerDay));
				}
			}
    	}

    	set_ref_count(PARTITIONS_PER_DIMENSION*PARTITIONS_PER_DIMENSION*PARTITIONS_PER_DIMENSION+1);
    	spawn_and_wait_for_all(growFungiTaskList);
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
