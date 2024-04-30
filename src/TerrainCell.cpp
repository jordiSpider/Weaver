#include "TerrainCell.h"
#include "ResourceSpecies.h"
#include "Maths/Random.h"
#include "Correlosome.h"
#include <thread>
#include <cmath>
#include "Utilities.h"
#include "LineInfoException.h"

#include "magic_enum/magic_enum.h"

/*
#include "tbb/tbb.h"

using namespace tbb;
*/
using namespace std;
using json = nlohmann::json;

TerrainCell::TerrainCell(int x, int y, int z, float newSize, const json &moistureBasePatch, vector<AnimalSpecies*> *existingAnimalSpecies, vector<ResourceSpecies*> *existingResourceSpecies)
	: myCoordinates(x, y, z), existingAnimalSpecies(existingAnimalSpecies), existingResourceSpecies(existingResourceSpecies)
{
	setSize(newSize);
	obstacle = false;
	temperature = 0.0;
	moisture = 0.0;
	relativeHumidityOnRainEvent = 0.0;

	timeStepsBetweenRainEvents = 1;
	auxInitialResourceBiomass = 0.0;
	auxInitialResourceSpeciesId = -1;
	for(const auto &lifeStage : LifeStage::getEnumValues())
	{
		animals[lifeStage] = new map<AnimalSpecies*, vector<Animal*> *>();
		for (auto speciesIt = existingAnimalSpecies->begin(); speciesIt != existingAnimalSpecies->end(); speciesIt++)
		{
			(*animals[lifeStage])[*speciesIt] = new vector<Animal*>();
		}
	}
	animalsToMove = new vector<Animal*>();
	animalsToReproduce = new vector<Animal*>();
	animalsToMetabolizeAndGrowth = new vector<Animal*>();

	setTemperatureCycle(moistureBasePatch["temperatureCycle"]);
	setRelativeHumidityCycle(moistureBasePatch["relativeHumidityCycle"]);
	setRelativeHumidityOnRainEvent(moistureBasePatch["relativeHumidityOnRainEvent"]);
	setRelativeHumidityDecayOverTime(moistureBasePatch["relativeHumidityDecayOverTime"]);
	setTimeStepsBetweenRainEvents(moistureBasePatch["timeStepsBetweenRainEvents"]);
	setStandardDeviationForRainEvent(moistureBasePatch["standardDeviationForRainEvent"]);
	setMaximumCapacities(moistureBasePatch["totalMaximumResourceCapacity"]);
	setInEnemyFreeSpace(moistureBasePatch["inEnemyFreeSpace"]);
	setInCompetitorFreeSpace(moistureBasePatch["inCompetitorFreeSpace"]);
}

TerrainCell::~TerrainCell()
{
	for(auto it = animals.begin(); it != animals.end(); it++)
	{
		map<AnimalSpecies* , vector<Animal*> *> * animalsInThisLifeStage = (*it).second;
		for(auto lifeStageIt = animalsInThisLifeStage->begin(); lifeStageIt != animalsInThisLifeStage->end(); lifeStageIt++)
		{
			vector<Animal*> * animalsInThisSpecies = (*lifeStageIt).second;
			for(auto animalsIt = animalsInThisSpecies->begin(); animalsIt != animalsInThisSpecies->end(); animalsIt++)
			{
				delete (*animalsIt);
			}
			animalsInThisSpecies->clear();
			delete animalsInThisSpecies;
		}
		animalsInThisLifeStage->clear();
		delete animalsInThisLifeStage;
	}
	animals.clear();

	for(auto it = resources.begin(); it != resources.end(); it++)
	{
		delete (*it);
	}
	resources.clear();

	for(auto it = cellsWithinDistance.begin(); it != cellsWithinDistance.end(); it++)
	{
		(*it).second->clear();
		delete (*it).second;
	}

	for(auto it = cellsAtDistance.begin(); it != cellsAtDistance.end(); it++)
	{
		(*it).second->clear();
		delete (*it).second;
	}

	delete animalsToMove;
	delete animalsToReproduce;
	delete animalsToMetabolizeAndGrowth;
}

void TerrainCell::setAuxInitialResourceBiomass(float auxInitialResourceBiomass, int auxInitialResourceSpeciesId)
{
	this->auxInitialResourceBiomass = auxInitialResourceBiomass;
	this->auxInitialResourceSpeciesId = auxInitialResourceSpeciesId;
}

void TerrainCell::checkMaximumResourceCapacity()
{
	double maximumResourceCapacity = 0.0;

	for(auto resource : resources) {
		maximumResourceCapacity += resource->getResourceMaximumCapacity();
	}

	if(maximumResourceCapacity > getTotalMaximumResourceCapacity()) {
		throwLineInfoException("The maximum resource capacity has been exceeded");
	}
}

void TerrainCell::addResource(Resource* newResource)
{
	//newResource->setBiomass(0);
	resources.push_back(newResource);

	checkMaximumResourceCapacity();
}

void TerrainCell::deleteResource(Resource* edible)
{
	resources.erase(std::remove(resources.begin(), resources.end(), edible), resources.end());
}

void TerrainCell::addAnimal(Animal* newAnimal)
{
	vector<Animal* > * animalsVector = animals[newAnimal->getLifeStage()]->at(newAnimal->getSpecies());
	animalsVector->push_back(newAnimal);
}

double TerrainCell::turnEdibleIntoDryMassToBeEaten(Edible* targetEdible, int day, Animal* predatorEdible, double leftovers)
{
	#ifdef _DEBUG
	Output::coutFlush("{} predated {}\n", predatorId, targetEdible->getId());
	#endif

	float targetEdibleProfitability = predatorEdible->getSpecies()->getEdibleProfitability(targetEdible->getSpecies());

	//double dryMassToBeEaten = 0;

	double dryMassToBeEaten = targetEdible->turnIntoDryMassToBeEaten(predatorEdible->getRemainingVoracity(), targetEdibleProfitability + predatorEdible->getAssim(), leftovers);
	

	if(targetEdible->getSpecies()->isMobile())
	{
		targetEdible->setNewLifeStage(LifeStage::PREDATED, day, predatorEdible->getId());

		animalsToMove->erase(std::remove(animalsToMove->begin(), animalsToMove->end(), targetEdible), animalsToMove->end());
	}
	else
	{
		
		//dryMassToBeEaten = targetEdible->turnIntoDryMassToBeEaten(predatorEdible->getRemainingVoracity(), targetEdibleProfitability + predatorEdible->getAssim());
		//arthros and for dinos, very big bug in feeding on resource and updating ungi fixed
		/* cout << "BEFOREEEEEEEEEE AND AFTEEEEEEEERRRRRRRRRRRRRRRRRR" << endl;
		cout << targetEdible->calculateDryMass() << endl;
		cout << dryMassToBeEaten << endl; */
		targetEdible->substractBiomass(dryMassToBeEaten);
		//cout << targetEdible->calculateDryMass() << endl;
	
	}

	if(dryMassToBeEaten < 0)
	{
		Output::cout("Edible id: {} ({}) - The dry mass to be eaten after considering profitability resulted in a negative value.\n", predatorEdible->getId(), predatorEdible->getSpecies()->getScientificName());
	}

	return dryMassToBeEaten;
}

void TerrainCell::changeAnimalToSenesced(Animal* targetAnimal, int day)
{
	#ifdef _DEBUG
	Output::coutFlush("{} predated {}\n", predatorId, targetAnimal->getId());
	#endif

	targetAnimal->setNewLifeStage(LifeStage::SENESCED, day);

	animalsToMove->erase(std::remove(animalsToMove->begin(), animalsToMove->end(), targetAnimal), animalsToMove->end());
}

void TerrainCell::eraseAnimal(Animal* oldEdible, const LifeStage &oldStage)
{
	vector<Animal*> * listOfAnimals = animals[oldStage]->at(oldEdible->getSpecies());
	listOfAnimals->erase(std::remove(listOfAnimals->begin(), listOfAnimals->end(), oldEdible), listOfAnimals->end());
}

void TerrainCell::setMaximumCapacities(float totalMaximumResourceCapacity)
{
	this->totalMaximumResourceCapacity = totalMaximumResourceCapacity;
	maximumPreysCapacity = 0.08*pow(totalMaximumResourceCapacity,0.75);
	maximumPredatorsCapacity  = 0.08*pow(maximumPreysCapacity,0.75);
}

void TerrainCell::setInEnemyFreeSpace(bool inEnemyFreeSpace)
{
	this->inEnemyFreeSpace = inEnemyFreeSpace;
}

void TerrainCell::setInCompetitorFreeSpace(bool inCompetitorFreeSpace)
{
	this->inCompetitorFreeSpace = inCompetitorFreeSpace;
}

float TerrainCell::getSize()
{
	return this->size;
}

float TerrainCell::getTemperature()
{
	return this->temperature;
}

float TerrainCell::getC()
{
	return this->C;
}

float TerrainCell::getP()
{
	return this->P;
}

float TerrainCell::getN()
{
	return this->N;
}

void TerrainCell::setSize(float newSize)
{
	if (newSize > 0)
	{
		this->size = newSize;
	}
	else
	{
		std::cerr << "Cell size must be positive. You entered " << newSize << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void TerrainCell::updateTemperature(int timeStep)
{
	if(!temperatureCycle.empty())
	{
		this->temperature = temperatureCycle.at(timeStep%temperatureCycle.size());
	}
}

void TerrainCell::updateRelativeHumidity(int timeStep)
{
	if(!relativeHumidityCycle.empty())
	{
		moisture = relativeHumidityCycle.at(timeStep%relativeHumidityCycle.size());
	}
	else
	{
		if(timeStep % timeStepsBetweenRainEvents == 0)
		{
			moisture = relativeHumidityOnRainEvent;
		}
		else
		{
			moisture -= relativeHumidityDecayOverTime;
			if(moisture < 0)
			{
				moisture = 0;
			}
		}
	}
}

void TerrainCell::setC(float newC)
{
	if (newC >= 0)
	{
		this->C = newC;
	}
	else
	{
		std::cerr << "Carbon Stoichiometry must be >=0. You entered " << newC << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void TerrainCell::setP(float newP)
{
	if (newP >= 0)
	{
		this->P = newP;
	}
	else
	{
		std::cerr << "Phosporous Stoichiometry must be >=0. You entered " << newP << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void TerrainCell::setN(float newN)
{
	if (newN >= 0)
	{
		this->N = newN;
	}
	else
	{
		std::cerr << "Nitrogen Stoichiometry must be >=0. You entered " << newN << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void TerrainCell::setElements(float newC, float newN, float newP)
{
	setC(newC);
	setN(newN);
	setP(newP);
}

float TerrainCell::getWater()
{
	return this->moisture;
}

void TerrainCell::setRelativeHumidityOnRainEvent(float relativeHumidityOnRainEvent)
{
	if(relativeHumidityOnRainEvent < 0) {
		throwLineInfoException("Water resource cannot be negative on a terrain cell");
	}

	this->moisture = max(this->moisture, relativeHumidityOnRainEvent);
	this->relativeHumidityOnRainEvent = max(this->relativeHumidityOnRainEvent, relativeHumidityOnRainEvent);
}

void TerrainCell::setRelativeHumidityDecayOverTime(float relativeHumidityDecayOverTime)
{
	this->relativeHumidityDecayOverTime = relativeHumidityDecayOverTime;
}


void TerrainCell::setTimeStepsBetweenRainEvents(int timeStepsBetweenRainEvents)
{
	if (timeStepsBetweenRainEvents >= 0)
	{
		this->timeStepsBetweenRainEvents = timeStepsBetweenRainEvents;
	}
	else
	{
		std::cerr << "Error, timeStepsBetweenRainEvents must be 0 or a positive value. You entered "
				<< timeStepsBetweenRainEvents << " ... EXITING" << std::endl;
		exit(-1);
	}
}

void TerrainCell::setStandardDeviationForRainEvent(float standardDeviationForRainEvent)
{
	if (standardDeviationForRainEvent >= 0)
	{
		this->standardDeviationForRainEvent = standardDeviationForRainEvent;
	}
	else
	{
		std::cerr << "Error, standardDeviationForRainEvent must be 0 or a positive value. You entered "
				<< standardDeviationForRainEvent << " ... EXITING" << std::endl;
		exit(-1);
	}
}

void TerrainCell::chemostatEffect()
{
	moisture = max(moisture, relativeHumidityOnRainEvent);
}

void TerrainCell::setTemperatureCycle(const vector<float>& temperatureCycle)
{
	this->temperatureCycle = temperatureCycle;
}

void TerrainCell::setRelativeHumidityCycle(const vector<float>& relativeHumidityCycle)
{
	if(!this->relativeHumidityCycle.empty() && !relativeHumidityCycle.empty()) {
		float previousAverage = accumulate(this->relativeHumidityCycle.begin(), this->relativeHumidityCycle.end(), 0.0)/this->relativeHumidityCycle.size();
		float average = accumulate(relativeHumidityCycle.begin(), relativeHumidityCycle.end(), 0.0) / relativeHumidityCycle.size();
		if(average >= previousAverage) {
			this->relativeHumidityCycle = relativeHumidityCycle;
		}
	} else {
		this->relativeHumidityCycle = relativeHumidityCycle;
	}
}

void TerrainCell::printStoichiometry(bool includeWater)
{
	float min = this->C;

	if (this->moisture < min)
		min = this->moisture;
	if (this->N < min)
		min = this->N;
	if (this->P < min)
		min = this->P;

	if (includeWater)
		Output::cout("Cell Stoichiometry (Water:C:N:P): {:.{0}f}:{:.{0}f}:{:.{0}f}:{:.{0}f}\n", STOICHIOMETRY_PRECISION, this->moisture / min, this->C / min, this->N / min, this->P / min);
	else
		Output::cout("Cell Stoichiometry (C:N:P): {:.{0}f}:{:.{0}f}:{:.{0}f}\n", STOICHIOMETRY_PRECISION, this->C / min, this->N / min, this->P / min);

}

std::vector<TerrainCell *> * TerrainCell::getCellsAtDistance(int distance, double searchArea, bool isMobile, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, TerrainCell*(*getCell)(unsigned int, unsigned int, unsigned int))
{
	//TODO cellsWithinDistance vector might be useful in order to improve performance, although it uses so much memory
	/*
	map<unsigned int, std::vector<TerrainCell *> *>::iterator it;
	TerrainCell* cellToBeAdded;

	it = cellsAtDistance.find(distance);

	if (it == cellsAtDistance.end())
	{
		std::vector<TerrainCell *> * newNeighbors = new std::vector<TerrainCell *>();
		if(distance == 1)
		{
			newNeighbors->push_back(this);
		}

		int myZ, myY, myX;
		int worldDepth, worldLength, worldWidth;

		myZ = myCoordinates.Z();
		myY = myCoordinates.Y();
		myX = myCoordinates.X();

		worldDepth = theWorld->getDepth();
		worldLength = theWorld->getLength();
		worldWidth = theWorld->getWidth();

		int minX = myX - min(distance, myX);
		int maxX = myX + min(distance, (worldWidth-1)-myX);
		int minY = myY - min(distance, myY);
		int maxY = myY + min(distance, (worldLength-1)-myY);
		int minZ = myZ - min(distance, myZ);
		int maxZ = myZ + min(distance, (worldDepth-1)-myZ);

		for (int z = minZ; z <= maxZ; z++)
		{
			if (z >= 0 && z < worldDepth)
			{
				for (int y = minY; y <= maxY; y++)
				{
					if (y >= 0 && y < worldLength)
					{
						for (int x = minX; x <= maxX; x++)
						{
							if (x >= 0 && x < worldWidth)
							{
									if (x == minX || y == minY || x == maxX || y == maxY || z == minZ || z == maxZ)
									{
										cellToBeAdded = theWorld->getCell(z, y, x);
										if(cellToBeAdded != NULL && cellToBeAdded != this && !cellToBeAdded->isObstacle())
										{
											newNeighbors->push_back(cellToBeAdded);
										}
									}
							}
						}
					}
				}
			}
		}

		cellsAtDistance[distance] = newNeighbors;
	}

	std::vector<TerrainCell *> * searchAreaNeighbors = new std::vector<TerrainCell *>();
	std::vector<TerrainCell *>::iterator newNeighborsIt;
	for(newNeighborsIt = cellsAtDistance[distance]->begin(); newNeighborsIt != cellsAtDistance[distance]->end(); newNeighborsIt++)
	{
		cellToBeAdded = (*newNeighborsIt);
		double distanceToTheCellToBeAdded = getDistanceToCell(cellToBeAdded);
		if(distanceToTheCellToBeAdded <= searchArea)
		{
			searchAreaNeighbors->push_back(cellToBeAdded);
		}
	}
	return searchAreaNeighbors;
	*/

	TerrainCell* cellToBeAdded;

	std::vector<TerrainCell *> * newNeighbors = new std::vector<TerrainCell *>();
	if(distance == 1)
	{
		newNeighbors->push_back(this);
	}

	int myZ, myY, myX;

	myZ = myCoordinates.getZ();
	myY = myCoordinates.getY();
	myX = myCoordinates.getX();

	int minX = myX - min(distance, myX);
	int maxX = myX + min(distance, (int)(worldWidth-1)-myX);
	int minY = myY - min(distance, myY);
	int maxY = myY + min(distance, (int)(worldLength-1)-myY);
	int minZ = myZ - min(distance, myZ);
	int maxZ = myZ + min(distance, (int)(worldDepth-1)-myZ);

/* 	if(myX == 48 && myY == 21){
	cout << "the coordinates are    " << endl;
	cout << myX << endl;
	cout << myY << endl;
	cout << myZ << endl;
	cout << minX << endl;
	cout << maxX << endl;
	cout << minY << endl;
	cout << maxY << endl;
	cout << minZ << endl;
	cout << maxZ << endl;

	minY = 21;

	}  */
	

	for (int z = minZ; z <= maxZ; z++)
	{
		if (z >= 0 && z < worldDepth)
		{
			for (int y = minY; y <= maxY; y++)
			{
				if (y >= 0 && y < worldLength)
				{
					for (int x = minX; x <= maxX; x++)
					{
						if (x >= 0 && x < worldWidth)
						{
								if (x == minX || y == minY || x == maxX || y == maxY || z == minZ || z == maxZ)
								{
									//arthro and for dinos
									if(!isMobile && (*getCell)(z, y, x)->isInPatch()){ //so resource spreads only within the original patches
									//TODO Jordi check: does this optimize animal search behavior?
									cellToBeAdded = (*getCell)(z, y, x);
									if(cellToBeAdded != NULL && cellToBeAdded != this && !cellToBeAdded->isObstacle())
									{
										newNeighbors->push_back(cellToBeAdded);
									}
									}else{//isMobile, an animal
									cellToBeAdded = (*getCell)(z, y, x);
									if(cellToBeAdded != NULL && cellToBeAdded != this && !cellToBeAdded->isObstacle())
									{
										newNeighbors->push_back(cellToBeAdded);
									}	
									}//arthro and for dinos
								}
						}
					}
				}
			}
		}
	}

	std::vector<TerrainCell *> * searchAreaNeighbors = new std::vector<TerrainCell *>();
	std::vector<TerrainCell *>::iterator newNeighborsIt;
	for(newNeighborsIt = newNeighbors->begin(); newNeighborsIt != newNeighbors->end(); newNeighborsIt++)
	{
		cellToBeAdded = (*newNeighborsIt);
		double distanceToTheCellToBeAdded = getDistanceToCell(cellToBeAdded);
		if(distanceToTheCellToBeAdded <= searchArea)
		{
			searchAreaNeighbors->push_back(cellToBeAdded);
		}
	}

	newNeighbors->clear();
	delete newNeighbors;
	return searchAreaNeighbors;
}

std::vector<TerrainCell *> * TerrainCell::getCellsWithinDistance(int maxDistance, double searchArea, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, TerrainCell*(*getCell)(unsigned int, unsigned int, unsigned int))
{
	//TODO cellsWithinDistance vector might be useful in order to improve performance, although it uses so much memory
	//map<unsigned int, std::vector<TerrainCell *> *>::iterator it = cellsWithinDistance.find(maxDistance);
	//it = cellsWithinDistance.find(maxDistance);
	//if (it == cellsWithinDistance.end())
	//{
		std::vector<TerrainCell *> * newNeighbors = new std::vector<TerrainCell *>();
		int myZ, myY, myX;

		myZ = myCoordinates.getZ();
		myY = myCoordinates.getY();
		myX = myCoordinates.getX();

		int minX = myX - min(maxDistance, myX);
		int maxX = myX + min(maxDistance, (int)(worldWidth-1)-myX);
		int minY = myY - min(maxDistance, myY);
		int maxY = myY + min(maxDistance, (int)(worldLength-1)-myY);
		int minZ = myZ - min(maxDistance, myZ);
		int maxZ = myZ + min(maxDistance, (int)(worldDepth-1)-myZ);

		TerrainCell* cellToBeAdded;
		for (int z = minZ; z <= maxZ; z++)
		{
			if (z >= 0 && z < worldDepth)
			{
				for (int y = minY; y <= maxY; y++)
				{
					if (y >= 0 && y < worldLength)
					{
						for (int x = minX; x <= maxX; x++)
						{
							if (x >= 0 && x < worldWidth)
							{
								cellToBeAdded = (*getCell)(z, y, x);
								if(cellToBeAdded != NULL && !cellToBeAdded->isObstacle())
								{
									double distanceToTheCellToBeAdded = getDistanceToCell(cellToBeAdded);
									if(distanceToTheCellToBeAdded <= searchArea)
									{
										newNeighbors->push_back(cellToBeAdded);
									}
								}
							}
						}
					}
				}
			}
		}

		//cellsWithinDistance[maxDistance] = newNeighbors;
	//}

	//return cellsWithinDistance[maxDistance];
	return newNeighbors;
}

double TerrainCell::getDistanceToCell(TerrainCell* targetCell)
{
	return sqrt(pow(abs(getX() - targetCell->getX()) * size, 2) + pow(abs(getY() - targetCell->getY()) * size, 2) + pow(abs(getZ() - targetCell->getZ()) * size, 2));
}

void TerrainCell::diePredatorsFromBackground(int timeStep, bool growthAndReproTest)
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		vector<Animal*> activeAnimals = *(getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies));

		if(!currentAnimalSpecies->getEdibleAnimalSpecies()->empty())
		{
			for (auto activeAnimalsIt = activeAnimals.begin(); activeAnimalsIt != activeAnimals.end(); activeAnimalsIt++)
			{
				(*activeAnimalsIt)->dieFromBackground(timeStep, growthAndReproTest);
			}
		}
	}
}

void TerrainCell::breedAnimals(int timeStep, int timeStepsPerDay, fs::path outputDirectory, bool saveAnimalConstitutiveTraits, FILE* constitutiveTraitsFile)
{
	//shuffleList(&(animals[LifeStage::REPRODUCING]));

	for (auto animalsToReproduceIt = animalsToReproduce->begin(); animalsToReproduceIt != animalsToReproduce->end(); animalsToReproduceIt++)
	{
		Animal* female = (*animalsToReproduceIt);

		list<Animal*> * offSprings = female->breed(timeStep, timeStepsPerDay, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);

		FILE* geneticsFile;
		createOutputFile(&geneticsFile, outputDirectory, "animal_genetics", "txt");
		if(!isFileOpen(geneticsFile)) {
			Output::cerr("Error opening the file.\n");
		} else {
			for(auto offSpringsIt = offSprings->begin(); offSpringsIt != offSprings->end(); offSpringsIt++)
			{
				//(*offSpringsIt)->printGenetics(geneticsFile);
			}

			fclose(geneticsFile);
		}

		vector<Animal*> * unbornAnimals = getAnimalsBySpecies(LifeStage::UNBORN, female->getSpecies());
		unbornAnimals->insert(unbornAnimals->end(), offSprings->begin(), offSprings->end());
	
		offSprings->clear();
		delete offSprings;
	}
	animalsToReproduce->clear();
}

void TerrainCell::metabolizeAnimals(int timeStep, int timeStepPerDay, SimulType simulType)
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
		for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
		{
			(*activeAnimalsIt)->metabolize(timeStep, timeStepPerDay, simulType);
		}
	}

	/*
	for (vector<Animal *>::iterator animalsToMetabolizeIt = animalsToMetabolizeAndGrowth->begin(); animalsToMetabolizeIt != animalsToMetabolizeAndGrowth->end(); animalsToMetabolizeIt++)
	{
		(*animalsToMetabolizeIt)->metabolize(timeStep, timeStepPerDay);
	}
	*/
}

void TerrainCell::growAnimals(int timeStep, int timeStepsPerDay)
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		vector<Animal*> activeAnimals = *(getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies));
		for (auto activeAnimalsIt = activeAnimals.begin(); activeAnimalsIt != activeAnimals.end(); activeAnimalsIt++)
		{
			(*activeAnimalsIt)->grow(timeStep, timeStepsPerDay);
		}
	}


	/*
	for (vector<Animal *>::iterator animalsToGrowIt = animalsToMetabolizeAndGrowth->begin(); animalsToGrowIt != animalsToMetabolizeAndGrowth->end(); animalsToGrowIt++)
	{
		unsigned int oldStage = (*animalsToGrowIt)->getLifeStage();
		unsigned int newStage = (*animalsToGrowIt)->grow(timeStep, timeStepsPerDay);

		if (newStage != oldStage)
		{
			// If the animal is to reproduce, move it from ACTIVE to REPRODUCING
			// If the animal is to DIAPAUSE, move it from ACTIVE to DIAPAUSE
			addAnimal(*animalsToGrowIt);
			eraseAnimal(*animalsToGrowIt, oldStage);
		}
	}
	*/
	animalsToMetabolizeAndGrowth->clear();

	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		vector<Animal*> * reproducingAnimals = getAnimalsBySpecies(LifeStage::REPRODUCING, currentAnimalSpecies);

		//Prepare the animals to be reproduced today
		animalsToReproduce->insert(animalsToReproduce->end(), reproducingAnimals->begin(), reproducingAnimals->end());
	}
}

void TerrainCell::tuneTraits(int day, int timeStepsPerDay, std::FILE* tuneTraitsFile, SimulType simulType, TerrainCell*(*getCell)(unsigned int, unsigned int, unsigned int))
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
		vector<Animal*> * pupaAnimals = getAnimalsBySpecies(LifeStage::PUPA, currentAnimalSpecies);
		vector<Animal*> * satiatedAnimals = getAnimalsBySpecies(LifeStage::SATIATED, currentAnimalSpecies);
		vector<Animal*> * handlingAnimals = getAnimalsBySpecies(LifeStage::HANDLING, currentAnimalSpecies);
		vector<Animal*> * diapauseAnimals = getAnimalsBySpecies(LifeStage::DIAPAUSE, currentAnimalSpecies);
		vector<Animal*> * tuneTraitsAnimals = new vector<Animal*>();
		tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), activeAnimals->begin(), activeAnimals->end());
		tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), pupaAnimals->begin(), pupaAnimals->end());
		tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), satiatedAnimals->begin(), satiatedAnimals->end());
		tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), handlingAnimals->begin(), handlingAnimals->end());
		tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), diapauseAnimals->begin(), diapauseAnimals->end());

		for (auto tuneTraitsAnimalsIt = tuneTraitsAnimals->begin(); tuneTraitsAnimalsIt != tuneTraitsAnimals->end(); tuneTraitsAnimalsIt++)
		{
			//Growth Curves calculus added here so it will be computed every step with the new temperature
			(*tuneTraitsAnimalsIt)->calculateGrowthCurves(temperature, tuneTraitsFile, true, -1); //Dinosaurs
			(*tuneTraitsAnimalsIt)->increaseAge(1);
			(*tuneTraitsAnimalsIt)->tuneTraits(day, timeStepsPerDay, temperature, moisture, tuneTraitsFile, true, false, simulType, getCell);
		}
		delete tuneTraitsAnimals;

		activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
		//Prepare the animals to be moved today
		animalsToMove->insert(animalsToMove->end(), activeAnimals->begin(), activeAnimals->end());
		//animalsToMove->insert(animalsToMove->end(), reproducingAnimals->begin(), reproducingAnimals->end());
	}
}

void TerrainCell::commitResourceSpread()
{
	//TODO Añadir las contribuciones de cada especie EN EL MISMO ORDEN
	//que se añadieron los crecimientos en la celda ORIGEN.
	//std::random_shuffle(resource.begin(), resource.end());

	vector<Resource*> markedForRemoval;
	for (auto it = resources.begin(); it != resources.end(); it++)
	{
		(*it)->processExternalContributions(getTotalMaximumResourceCapacity(), getTotalResourceBiomass());
		if ((*it)->isExtinct())
		{
			markedForRemoval.push_back((*it));
		}
	}

	for (auto it = markedForRemoval.begin(); it != markedForRemoval.end(); it++)
	{
		deleteResource((*it));
		delete(*it);
	}
}

void TerrainCell::setResourceFromChemostatEffect(vector<ResourceSpecies *>* existingResourceSpecies, double increaseForChemostatEffect, bool competitionAmongResourceSpecies, double massRatio) {
	if(moisture > 0)
	{
		//LO QUITAMOS PROVISIONALMENTE!
		//double fractionPerSpecies = 1.0/(double)existingResourceSpecies->size();
		for(auto it = existingResourceSpecies->begin(); it != existingResourceSpecies->end(); it++)
		{
			Resource* currentResource = getResource(*it);
			if(currentResource != NULL && (int)(*it)->getId() == auxInitialResourceSpeciesId)
			{
				//TODO PROVISIONAL! CAMBIAR POR MOISTURE
				currentResource->setBiomass(increaseForChemostatEffect*auxInitialResourceBiomass); // Adds biomass to this resource species living within this cell

			}
			else if((int)(*it)->getId() == auxInitialResourceSpeciesId)
			{
				//TODO PROVISIONAL! CAMBIAR POR MOISTURE
				addResource(new Resource((*it), increaseForChemostatEffect*auxInitialResourceBiomass, 0.0, competitionAmongResourceSpecies, massRatio));
			}
		}
	}
}

void TerrainCell::growResource(int timeStep, bool competition, double maxCapacity, int size, SimulType simulType, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, TerrainCell*(*getCell)(unsigned int, unsigned int, unsigned int), bool competitionAmongResourceSpecies, double massRatio)
{
	//unsigned int myZ, myY, myX;
	//unsigned int worldDepth, worldLength, worldWidth;

	//myZ = myCoordinates.Z();
	//myY = myCoordinates.Y();
	//myX = myCoordinates.X();

	//worldDepth = theWorld->getDepth();
	//worldLength = theWorld->getLength();
	//worldWidth = theWorld->getWidth();

	// Grow in random order if more than one species exist
	std::random_shuffle(resources.begin(), resources.end());
	vector<Resource*> markedForRemoval;
	for (auto it = resources.begin(); it != resources.end(); it++)
	{

		//cout << "wetMass:  " << (*it)->calculateWetMass() << endl;
		double excess = (*it)->grow(timeStep, getTotalMaximumResourceCapacity(), getWater(), getTemperature(), getTotalResourceBiomass(), simulType); // Grows this resource species living within this cell
		
		/* if((*it)->calculateWetMass() > 2.5){
		cout << "after excess 2:   " << endl;
		cout << excess << endl;
		cout << (*it)->calculateWetMass() <<  endl;
		}    */

		/* if(excess == 0){
		cout << "after excess 2:   " << endl;
		cout << excess << endl;
		cout << (*it)->calculateWetMass() <<  endl;
		}    */
		
		// DO NOT COMMENT OR REMOVE THIS LINE: it validates the output 
		// TODO investigate the reason...
		//cout << "TerrainCell: (" << myCoordinates.X() << "," << myCoordinates.Y() << "," << myCoordinates.Z() << ")" << " excess " << (*it)->getSpecies()->getScientificName() <<  ": " << excess << endl;		if ((*it)->isExtinct())
		if ((*it)->isExtinct())
 		{
			markedForRemoval.push_back((*it));
			//cout << "Are we here?????????????????????????????????????????????????????" << endl;
		}
			//> 100000000 to stop spreading functionality until further debugging
			if (excess > 0) //spread only if excess > cell size - otherwise is not biologically possible
			{              //(*it)->getSpecies()->getCellMass()                                  
				bool isMobile = (*it)->getSpecies()->isMobile();
				std::vector<TerrainCell *> * neighbors = getCellsAtDistance(1, DBL_MAX, isMobile, worldDepth, worldLength, worldWidth, getCell);

				ResourceSpecies* currentSpecies = (*it)->getSpecies();
				double massPerNeighbor = excess / neighbors->size(); // Distribute equally across neighbors
				
				Resource* auxResource;

				for (std::vector<TerrainCell *>::iterator neighborIt = neighbors->begin(); neighborIt != neighbors->end(); neighborIt++)
				{
					auxResource = (*neighborIt)->getResource(currentSpecies);

					double totBiomass = (*neighborIt)->getTotalResourceBiomass();

					if (auxResource != NULL)
					{
						auxResource->addBiomass(massPerNeighbor, competition, maxCapacity, size, totBiomass);
					}
					else
					{   //spread to a cell where that resource species was extinct
						if((*neighborIt)->isInPatch()){
							(*neighborIt)->addResource(new Resource(currentSpecies, massPerNeighbor, (*it)->getResourceMaximumCapacity(), competitionAmongResourceSpecies, massRatio)); //, massPerNeighbor, this
							auxResource = (*neighborIt)->getResource(currentSpecies);
							auxResource->addBiomass(massPerNeighbor, competition, maxCapacity, size, totBiomass);
						}
					
					} 
				}

			//Jordi attempt to free memory, it does not work
			/* for (std::vector<TerrainCell *>::iterator neighborIt = neighbors->begin(); neighborIt != neighbors->end(); neighborIt++)
		    {
		
		    delete (*neighborIt); 
			
		    }*/ 
		    neighbors->clear();
		    delete neighbors; 
			}

	}

	for (auto it = markedForRemoval.begin(); it != markedForRemoval.end(); it++)
	{
		resources.erase(std::remove(resources.begin(), resources.end(), (*it)), resources.end());
		delete (*it);
	}
}

/*
 * Returns, for each requested resource species, their stocks in this cell.
 * This is used to take decisions on preys movement but not taking into account the
 * potentially existing of predators on the cell.
 */
map<Species *, double> * TerrainCell::getResourceStock(Animal* animalWhoIsEvaluating, bool discardZero)
{
	const vector<ResourceSpecies *> * edibleResourceSpecies = animalWhoIsEvaluating->getSpecies()->getEdibleResourceSpecies();
	map<Species *, double> * result = new map<Species *, double> ();

	float mass;
	for (auto it = edibleResourceSpecies->begin(); it != edibleResourceSpecies->end(); it++)
	{
		mass = getResourceBiomass(*it);

		if ((mass > 0) || (!discardZero))
		{
			(*result)[(*it)] = mass;
		}
	}

	return result;
}

/*
 * Returns for each requested resource species, the ratio between their stocks and the number of
 * predator in this cell. This is used to take decisions on preys movement.
 */
map<Species *, double> * TerrainCell::getRateOfResourcePredators(Animal* animalWhoIsEvaluating, bool discardZero)
{
	// TODO ROMAN A JORDI: Sigo sin ver esto claro (todos los ratios)
	map<Species *, double> * result = new map<Species *, double> ();
	unsigned int predatorsCounter = 1; // +1: Avoid division by zero
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies->canEatAnimalSpecies(animalWhoIsEvaluating->getSpecies()))
		{
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
			for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				if ((*activeAnimalsIt)->canEatEdible(animalWhoIsEvaluating))
				{
					predatorsCounter++;
				}
			}
		}
	}

	double inversePredatorsCounter = 1.0 / predatorsCounter; // Avoid dividing many times;

	float mass;
	const vector<ResourceSpecies *> * edibleResourceSpecies = animalWhoIsEvaluating->getSpecies()->getEdibleResourceSpecies();
	for (auto it = edibleResourceSpecies->begin(); it != edibleResourceSpecies->end(); it++)
	{
		mass = getResourceBiomass(*it);

		if ((mass > 0) || (!discardZero))
		{
			(*result)[(*it)] = mass * inversePredatorsCounter; // This is like dividing as I have inverted predatorsCounter first
		}
	}

	return result;
}



tuple<double, double, double> TerrainCell::getCellEvaluation(Animal* animalWhoIsEvaluating, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
//arthros and for dinos to force searching for preferred food items be animals or resource, prompts mobility 
	double preferenceThresholdForEvaluation = 0.0;//defaults to 0 but it is a parameter to include in run_params.json
	
	double totalPredatoryRiskEdibilityValue = 0.0;
	double totalEdibilityValue = 0.0;
	double totalConspecificBiomass = 0.0;
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies->canEatAnimalSpecies(animalWhoIsEvaluating->getSpecies()))
		{
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
			for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				Animal* predatoryRiskAnimal = (*activeAnimalsIt);
				if (predatoryRiskAnimal->canEatEdible(animalWhoIsEvaluating))
				{
					double predatoryRiskEdibilityValue = predatoryRiskAnimal->calculatePredatoryRiskEdibilityValue(animalWhoIsEvaluating, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
					totalPredatoryRiskEdibilityValue += predatoryRiskEdibilityValue;
				}
			}
		}
		if(animalWhoIsEvaluating->getSpecies()->canEatAnimalSpecies(currentAnimalSpecies))
		{
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
			for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				Animal* edibleAnimal = (*activeAnimalsIt);
				if (!animalWhoIsEvaluating->hasTriedToHunt(edibleAnimal))
				{
					
					if(animalWhoIsEvaluating->getSpecies()->getEdiblePreference(edibleAnimal->getSpecies()) > preferenceThresholdForEvaluation){	
					double edibilityValue = animalWhoIsEvaluating->calculateEdibilityValueWithMass(edibleAnimal, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
					totalEdibilityValue += edibilityValue;
					}
				
				}
			}
		}
		if(animalWhoIsEvaluating->getSpecies() == currentAnimalSpecies)
		{
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
			for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				Animal* conspecificAnimal = (*activeAnimalsIt);
				totalConspecificBiomass += conspecificAnimal->calculateDryMass();
			}
		}
	}

	const vector<ResourceSpecies *> * edibleResourceSpecies = animalWhoIsEvaluating->getSpecies()->getEdibleResourceSpecies();
	for (auto it = edibleResourceSpecies->begin(); it != edibleResourceSpecies->end(); it++)
	{
		ResourceSpecies* currentResourceSpecies = *it;
		Edible* resource = getResource(currentResourceSpecies);
		//arthros to force search for preferred food
			if(resource != NULL)
			{
				if(animalWhoIsEvaluating->getSpecies()->getEdiblePreference(resource->getSpecies()) > preferenceThresholdForEvaluation){	
					totalEdibilityValue += animalWhoIsEvaluating->calculateEdibilityValueWithMass(resource, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
				}
			}
		
	}

	return make_tuple(totalEdibilityValue, totalPredatoryRiskEdibilityValue, totalConspecificBiomass);
}

unsigned int TerrainCell::getNumberOfMatureFemales(Animal* reproducingAnimal)
{
	vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, reproducingAnimal->getSpecies());
	vector<Animal*> * satiatedAnimals = getAnimalsBySpecies(LifeStage::SATIATED, reproducingAnimal->getSpecies());
	vector<Animal*> * handlingAnimals = getAnimalsBySpecies(LifeStage::HANDLING, reproducingAnimal->getSpecies());
	vector<Animal*> * diapauseAnimals = getAnimalsBySpecies(LifeStage::DIAPAUSE, reproducingAnimal->getSpecies());
	vector<Animal*> * searchableAnimals = new vector<Animal*>();

	searchableAnimals->insert(searchableAnimals->end(), activeAnimals->begin(), activeAnimals->end());
	searchableAnimals->insert(searchableAnimals->end(), satiatedAnimals->begin(), satiatedAnimals->end());
	searchableAnimals->insert(searchableAnimals->end(), handlingAnimals->begin(), handlingAnimals->end());
	searchableAnimals->insert(searchableAnimals->end(), diapauseAnimals->begin(), diapauseAnimals->end());

	unsigned int counter = 0;
	for (auto searchableAnimalsIt = searchableAnimals->begin(); searchableAnimalsIt != searchableAnimals->end(); searchableAnimalsIt++)
	{
		if ((*searchableAnimalsIt)->getGender() == AnimalSpecies::GENDERS::FEMALE && (*searchableAnimalsIt)->isMature() && !(*searchableAnimalsIt)->isMated())
		{
			counter++;
		}
	}
	delete searchableAnimals;
	return counter;
}

unsigned int TerrainCell::getNumberOfEdibleAnimals(Animal* animalWhoIsEvaluating)
{
	unsigned int preysCounter = 0;

	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		if(animalWhoIsEvaluating->getSpecies()->canEatAnimalSpecies(currentAnimalSpecies))
		{
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
			for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				if (animalWhoIsEvaluating->canEatEdible((Edible*)(*activeAnimalsIt)))
				{
					preysCounter++;
				}
			}
		}
	}
	return preysCounter;
}

void TerrainCell::printInteractionMatrices(ostream& encountersMatrixFile, ostream& predationsMatrixFile,
		ostream& nodesMatrixFile, unsigned int totalInitialPopulation)
{
	unsigned int j;
	for(const auto &lifeStage : LifeStage::getEnumValues())
	{
		j = 0;
		for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
		{
			AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);

			for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				if(((unsigned int)((*activeAnimalsIt)->getId())) < totalInitialPopulation )
				{
					list<int> * encounterEvents = (*activeAnimalsIt)->getEncounterEvents();
					int predatedByID = (*activeAnimalsIt)->getPredatedByID();

					encountersMatrixFile << endl;
					encountersMatrixFile << j;
					string scientificName = string(currentAnimalSpecies->getScientificName());
					scientificName.replace( scientificName.begin(), scientificName.end(), ' ', '_' );

					nodesMatrixFile << "X" << (*activeAnimalsIt)->getId() << "\t" << scientificName << endl;

					for (unsigned int k = 0; k < totalInitialPopulation ; k++)
					{
						if( std::find(encounterEvents->begin(), encounterEvents->end(), k) != encounterEvents->end() )
						{
							encountersMatrixFile << "\t" << 1;
						}
						else
						{
							encountersMatrixFile << "\t" << 0;
						}

						if( (unsigned int)predatedByID == k )
						{
							predationsMatrixFile << "\t" << 1;
						}
						else
						{
							predationsMatrixFile << "\t" << 0;
						}
					}
				}
				j++;
			}
		}
	}
}

void TerrainCell::printAnimalsVoracities(int timeStep, int timeStepsPerDay, FILE* voracitiesFile, SimulType simulType)
{
	//for (unsigned int i = 0; i < animals.size(); ++i)
	//{
		for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
		{
			AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
			//activeAnimals = getAnimalsBySpecies(i, currentAnimalSpecies);
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

			for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				(*activeAnimalsIt)->printVoracities(timeStep, timeStepsPerDay, voracitiesFile, simulType);
			}
		}
	//}
}

void TerrainCell::printAnimals(FILE* file)
{
	for(const auto &lifeStage : LifeStage::getEnumValues())
	{
		for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
		{
			AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);

			for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				Output::print(file, "{}\n", *(*activeAnimalsIt));
			}
		}
	}
}

void TerrainCell::printCell(FILE* file)
{
	Output::print(file, "{}\t{}\t{}\t", getX(), getY(), getZ());

	for (auto itResourceSpecies = existingResourceSpecies->begin(); itResourceSpecies != existingResourceSpecies->end(); itResourceSpecies++)
	{
		Output::print(file, "{}\t", getResourceBiomass(*itResourceSpecies));
	}

	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		int currentNumberOfAnimals = 0;
		for(const auto &lifeStage : LifeStage::getEnumValues())
		{
			AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);
			currentNumberOfAnimals += activeAnimals->size();
		}
		Output::print(file, "{}\t", currentNumberOfAnimals);
	}
	Output::print(file, "\n");
}

void TerrainCell::activateAndResumeAnimals(int timeStep, int timeStepsPerDay)
{
	#ifdef _DEBUG
	Output::coutFlush("what is happening: {}\n", theWorld->getExistingAnimalSpecies()->at(0)->getScientificName());
	#endif

	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		vector<Animal*> unbornAnimals = *(getAnimalsBySpecies(LifeStage::UNBORN, currentAnimalSpecies));
		vector<Animal*> pupaAnimals = *(getAnimalsBySpecies(LifeStage::PUPA, currentAnimalSpecies));
		vector<Animal*> * handlingAnimals = getAnimalsBySpecies(LifeStage::HANDLING, currentAnimalSpecies);
		vector<Animal*> diapauseAnimals = *(getAnimalsBySpecies(LifeStage::DIAPAUSE, currentAnimalSpecies));

		#ifdef _DEBUG
		Output::cout("but am I actually doing anything?: unborn/active: {}/{} -> ", unbornAnimals->size(), activeAnimals->size());
		#endif

		for (auto diapauseAnimalsIt = diapauseAnimals.begin(); diapauseAnimalsIt != diapauseAnimals.end(); diapauseAnimalsIt++)
		{
			(*diapauseAnimalsIt)->isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(moisture);
		}

		for (auto unbornAnimalsIt = unbornAnimals.begin(); unbornAnimalsIt != unbornAnimals.end(); unbornAnimalsIt++)
		{
			(*unbornAnimalsIt)->isReadyToBeBorn(timeStep, timeStepsPerDay);
		}

		for (auto pupaAnimalsIt = pupaAnimals.begin(); pupaAnimalsIt != pupaAnimals.end(); pupaAnimalsIt++)
		{
			(*pupaAnimalsIt)->isReadyToResumeFromPupaOrDecreasePupaTimer();
		}

		/*
		for (vector<Edible*>::iterator handlingAnimalsIt = handlingAnimals->begin(); handlingAnimalsIt != handlingAnimals->end();)
		{
			if ((*handlingAnimalsIt)->isReadyToResumeFromHandlingOrDecreaseHandlingTimer())
			{
				activeAnimals->push_back((*handlingAnimalsIt));
				handlingAnimalsIt = handlingAnimals->erase(handlingAnimalsIt);
			}
			else
			{
				handlingAnimalsIt++;
			}
		}
		*/

		vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

		#ifdef _DEBUG
		Output::coutFlush("{}/{}\n", unbornAnimals->size(), activeAnimals->size());
		#endif

	}

	#ifdef _DEBUG
	Output::coutFlush("activation finished for terrain field {}\n", this);
	#endif
}

/*
void TerrainCell::activateAnimalsThreaded(int day)
{
	//TODO JOIN THE THREADS PROPERLY
	pthread_t functionThread;
	TerrainCell::ThreadArgument* threadArgument;
	int errorCreatingThread;

	threadArgument = new TerrainCell::ThreadArgument();
	threadArgument->terrainCell = this;
	threadArgument->timeStep = day;

	errorCreatingThread = pthread_create(&functionThread, NULL, &TerrainCell::activateAnimalsThreadMaker, static_cast<void*>(threadArgument));
	if (errorCreatingThread){
	 cout << "Error:unable to create thread," << errorCreatingThread << endl;
	 exit(-1);
	}
	pthread_join(functionThread,NULL);
}
*/

/*
void TerrainCell::tuneTraitsThreaded(int day)
{
	//TODO JOIN THE THREADS PROPERLY
	pthread_t functionThread;
	TerrainCell::ThreadArgument* threadArgument;
	int errorCreatingThread;

	threadArgument = new TerrainCell::ThreadArgument();
	threadArgument->terrainCell = this;
	threadArgument->timeStep = day;

	errorCreatingThread = pthread_create(&functionThread, NULL, &TerrainCell::tuneTraitsThreadMaker, static_cast<void*>(threadArgument));
	if (errorCreatingThread){
	 cout << "Error:unable to create thread," << errorCreatingThread << endl;
	 exit(-1);
	}
	pthread_join(functionThread,NULL);
}
*/

Resource* TerrainCell::getResource(ResourceSpecies* species)
{
	for (auto it = resources.begin(); it != resources.end(); it++)
	{
		if ((*it)->getSpecies() == species)
		{
			return (*it);
		}
	}
	return NULL;
}

vector<Resource *>* TerrainCell::getResources()
{
	return &resources;
}

float TerrainCell::getResourceBiomass(ResourceSpecies* species)
{
	Resource* resource = getResource(species);

	if (resource != NULL)
	{
		return resource->calculateWetMass();
	}
	else
	{
		return 0;
	}
}

//TODO - generalize this independently of alphabetical order
double TerrainCell::getTotalResourceBiomass()
{
	double mass = 0;
	for (auto it = resources.begin(); it != (resources.end()); it++)
	{
		mass = mass + (*it)->calculateWetMass();
	}
	return mass;
}

unsigned int TerrainCell::getLifeStagePopulation(const LifeStage &stage, const HuntingMode::HuntingModeValue &huntingMode)
{
	unsigned int population = 0;
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies->getDefaultHuntingMode() == huntingMode)
		{
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(stage, currentAnimalSpecies);
			population += activeAnimals->size();
		}
	}
	return population;
}

unsigned int TerrainCell::getLifeStagePopulation(const LifeStage &stage, AnimalSpecies* species)
{
	vector<Animal*> * activeAnimals = getAnimalsBySpecies(stage, species);
	return activeAnimals->size();
}

void TerrainCell::updateWorldResourceBiomassAndAnimalsPopulation(map<ResourceSpecies*, double>* worldResourceBiomass, map<AnimalSpecies*, vector<unsigned int>* >* worldAnimalsPopulation)
{
	for (auto itResource = resources.begin(); itResource != resources.end(); itResource++)
	{
		worldResourceBiomass->at((*itResource)->getSpecies()) += (*itResource)->calculateWetMass();
	}

	for(const auto &lifeStage : LifeStage::getEnumValues())
	{
		for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
		{
			AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);
			worldAnimalsPopulation->at(currentAnimalSpecies)->at(magic_enum::enum_integer(lifeStage)) += activeAnimals->size();
		}
	}
}

void TerrainCell::updateAnimalsPopulationAndGeneticsFrequencies(map<AnimalSpecies*, vector<unsigned int>* >* worldAnimalsPopulation, map<AnimalSpecies*, vector<set<double>* >* >* worldGeneticsFrequencies)
{

	for(const auto &lifeStage : LifeStage::getEnumValues())
	{
		for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
		{
			AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
			vector<Animal*> * activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);

			worldAnimalsPopulation->at(currentAnimalSpecies)->at(magic_enum::enum_integer(lifeStage))++;
			for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				auto currentCorrelosomes = (*activeAnimalsIt)->getGenome().getHomologousCorrelosomes();
				for (unsigned int selectedTrait = 0; selectedTrait < currentAnimalSpecies->getNumberOfVariableTraits(); ++selectedTrait)
				{
					for (unsigned int selectedLoci = 0; selectedLoci < currentAnimalSpecies->getNumberOfLociPerTrait(); ++selectedLoci)
					{
						worldGeneticsFrequencies->at(currentAnimalSpecies)->at(selectedTrait*currentAnimalSpecies->getNumberOfLociPerTrait()+selectedLoci)->insert(currentCorrelosomes.at(selectedTrait).first->getAllele(selectedLoci)->getValue());
					}
					
					for (unsigned int selectedLoci = 0; selectedLoci < currentAnimalSpecies->getNumberOfLociPerTrait(); ++selectedLoci)
					{
						//We multiply x2 because there are PAIRS of chromosomes. We store this data this way: T1_CR1, T2_CR1, T3_CR1.... T2_CR1, T2_CR2, ...
						worldGeneticsFrequencies->at(currentAnimalSpecies)->at(currentAnimalSpecies->getNumberOfLociPerTrait()*currentAnimalSpecies->getNumberOfVariableTraits()+selectedTrait*currentAnimalSpecies->getNumberOfLociPerTrait()+selectedLoci)->insert(currentCorrelosomes.at(selectedTrait).second->getAllele(selectedLoci)->getValue());
					}
				}
			}
		}
	}
}

/*
bool TerrainCell::exposeToPotentialPredators(Animal* exposedAnimal, int day, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile)
{

	vector<Animal *> * activeAnimals;
	vector<Animal *> * animalsHuntingExposed = new vector<Animal *>();

	vector<AnimalSpecies *>::iterator animalSpeciesIt;

	AnimalSpecies* currentAnimalSpecies;

	Animal* huntingAnimal;
	// TODO Add reproducing animals to the equation when we fix the way they find mates.
	vector<Animal*>::iterator animalsIt;
	for(animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies->canEatAnimalSpecies(exposedAnimal->getSpecies()))
		{
			activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
			//TODO Add an if sentence only for the PREDATORS, not for all species
			for(animalsIt = activeAnimals->begin(); animalsIt != activeAnimals->end(); animalsIt++)
			{
				huntingAnimal = *animalsIt;
				if (huntingAnimal->canEatAnimal(exposedAnimal) && huntingAnimal != exposedAnimal)
				{
					animalsHuntingExposed->push_back(huntingAnimal);
				}
			}
		}
	}

	int huntingAnimalIndex;
	// Try to eat every animal whether it is a predator or prey
	// TODO WEIGHT the interactions so some animals are more likely to be eaten
	while (!animalsHuntingExposed->empty())
	{
		huntingAnimalIndex = randomIndex(animalsHuntingExposed->size());
		huntingAnimal = animalsHuntingExposed->at(huntingAnimalIndex);

		//Erase from the vector of animals who are waiting to hunt the exposed
		animalsHuntingExposed->erase(animalsHuntingExposed->begin() + huntingAnimalIndex);

		bool searchWasSuccessful = huntingAnimal->searchAnimal(exposedAnimal, day, encounterProbabilitiesFile, predationProbabilitiesFile);
		if(searchWasSuccessful)
		{
			bool huntWasSuccessful = huntingAnimal->huntAnimal(exposedAnimal, day, false, encounterProbabilitiesFile, predationProbabilitiesFile);
			if(huntWasSuccessful)
			{
				//Exposed animal was predated
				//The animal is cleaned up from its source lifestage vector
				changeAnimalToPredated(exposedAnimal, day, huntingAnimal->getId());

				//The exposed animal was cleaned up from the animals to move vector in its origin cell.
				//Therefore we do not need to clean the exposed animal up again here (it does not exist in this position's animals to move vector)

				//The exposed animal died, so return true
				return true;
			}
			//Note that there is NO COUNTERHUNT because the dynamics here correspond with the exposure of the animal
		}
	}
	//The exposed animal did not die, so return false
	return false;
}
*/

void TerrainCell::migrateAnimalTo(Animal* animalToMigrate, TerrainCell* newPosition)
{
	vector<Animal*> * lifestageAnimals = getAnimalsBySpecies(animalToMigrate->getLifeStage(), animalToMigrate->getSpecies());
	lifestageAnimals->erase(std::remove(lifestageAnimals->begin(), lifestageAnimals->end(), animalToMigrate), lifestageAnimals->end());
	newPosition->addAnimal(animalToMigrate);
}

void TerrainCell::assimilateFoodMass(int timeStep)
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
		for (auto activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
		{
			(*activeAnimalsIt)->assimilateFoodMass(timeStep);
		}
	}
}

void TerrainCell::moveAnimals(int timeStep, int timeStepsPerDay, FILE* encounterProbabilitiesFile, FILE* predationProbabilitiesFile, bool saveEdibilitiesFile, FILE* edibilitiesFile, float exitTimeThreshold, TerrainCell*(*getCellByBearing)(TerrainCell*, TerrainCell*), unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, TerrainCell*(*getCell)(unsigned int, unsigned int, unsigned int), double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	Animal* currentAnimal;
	int currentAnimalIndex;

	//cout << "moving.. " << this << endl;
	#ifdef _DEBUG
	int currentMovingAnimalId;
	#endif
	auto  t0 = clock();
	auto  t1 = clock();

	//position->getTheWorld()->getTimeExitThreshold()

	

	while(!animalsToMove->empty() && (double(t1-t0)/CLOCKS_PER_SEC) < exitTimeThreshold) //arthro and for dinos... parameter to externalize and to optimize simulations
	{
		
		currentAnimalIndex = Random::randomIndex(animalsToMove->size());
		currentAnimal = animalsToMove->at(currentAnimalIndex);

		//Erase from the vector of animals who are left to move
		animalsToMove->erase(animalsToMove->begin() + currentAnimalIndex);

		#ifdef _DEBUG
		currentMovingAnimalId = (*it)->getId();
		#endif

		//TODO lo borramos de la lista de depredadores para que no pueda depredarse a sí mismo
		//it = activeAnimals->erase(it);
		currentAnimal->move(timeStep, timeStepsPerDay, encounterProbabilitiesFile, predationProbabilitiesFile, saveEdibilitiesFile, edibilitiesFile, getCellByBearing, worldDepth, worldLength, worldWidth, getCell, pdfThreshold, muForPDF, sigmaForPDF, existingAnimalSpecies, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);

		#ifdef _DEBUG
		if(currentMovingAnimalId != currentAnimal->getId())
		{
			cerr << "The current moving animal was " << currentMovingAnimalId << " and changed to " << currentAnimal->getId() << " unexpectedly (probably due to iterator issues)" << endl;
			exit(-1);
		}
		#endif

		t1 = clock();

	}

	if((double(t1-t0)/CLOCKS_PER_SEC) > exitTimeThreshold){
		Output::cout("too many animals for too little food!!!\n");
		exit(-1);
	}

	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		vector<Animal*> * activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);
		vector<Animal*> * satiatedAnimals = getAnimalsBySpecies(LifeStage::SATIATED, currentAnimalSpecies);
		vector<Animal*> * handlingAnimals = getAnimalsBySpecies(LifeStage::HANDLING, currentAnimalSpecies);

		//Prepare the animals to metabolize and grow
		animalsToMetabolizeAndGrowth->insert(animalsToMetabolizeAndGrowth->end(), activeAnimals->begin(), activeAnimals->end());
		animalsToMetabolizeAndGrowth->insert(animalsToMetabolizeAndGrowth->end(), satiatedAnimals->begin(), satiatedAnimals->end());
		animalsToMetabolizeAndGrowth->insert(animalsToMetabolizeAndGrowth->end(), handlingAnimals->begin(), handlingAnimals->end());
	}

	//cellsAtDistance.clear();
}

void TerrainCell::purgeDeadAnimals()
{
	
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;

		vector<Animal*> * starvedAnimals = getAnimalsBySpecies(LifeStage::STARVED, currentAnimalSpecies);
		vector<Animal*> * predatedAnimals = getAnimalsBySpecies(LifeStage::PREDATED, currentAnimalSpecies);
		vector<Animal*> * backgroundAnimals = getAnimalsBySpecies(LifeStage::BACKGROUND, currentAnimalSpecies);
		vector<Animal*> * senescedAnimals = getAnimalsBySpecies(LifeStage::SENESCED, currentAnimalSpecies);
		vector<Animal*> * shockedAnimals = getAnimalsBySpecies(LifeStage::SHOCKED, currentAnimalSpecies);

		for (auto it = starvedAnimals->begin(); it != starvedAnimals->end(); it++)
		{
			delete (*it);
		}
		starvedAnimals->clear(); //this turns the size of the vector to zero
		
		for (auto it = predatedAnimals->begin(); it != predatedAnimals->end(); it++)
		{
			delete (*it);
		}
		predatedAnimals->clear();

	 	for (auto it = backgroundAnimals->begin(); it != backgroundAnimals->end(); it++)
		{
			delete (*it); //->getGenome().getHomologousChromosomes());
			//delete (*aGenome);
			//delete (*it);
		} 
		backgroundAnimals->clear();
		//free(backgroundAnimals);
		

		for (auto it = senescedAnimals->begin(); it != senescedAnimals->end(); it++)
		{
			delete (*it);
		}
		senescedAnimals->clear();

		for (auto it = shockedAnimals->begin(); it != shockedAnimals->end(); it++)
		{
			delete (*it);
		}
		shockedAnimals->clear();
	} 
}

void TerrainCell::deleteExtinguishedReproducingAnimals(AnimalSpecies* extinguishedAnimalSpecies)
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies == extinguishedAnimalSpecies)
		{
			vector<Animal*> * reproducingAnimals = getAnimalsBySpecies(LifeStage::REPRODUCING, currentAnimalSpecies);
			for (auto reproducingAnimalsIt = reproducingAnimals->begin(); reproducingAnimalsIt != reproducingAnimals->end();)
			{
				Animal* deletedAnimal = (*reproducingAnimalsIt);
				reproducingAnimalsIt = reproducingAnimals->erase(reproducingAnimalsIt);
				delete deletedAnimal;
			}
		}
	}
}

/*
//#ifdef _PTHREAD
void* TerrainCell::activateAnimalsThreadMaker(void* threadArgument)
{
        struct ThreadArgument* argument = static_cast<ThreadArgument*>(threadArgument);
        TerrainCell* terrainCell = argument->terrainCell;
        int timeStep = argument->timeStep;
        int timeStepsPerDay = argument->timeStepsPerDay;
        terrainCell->activateAndResumeAnimals(timeStep, timeStepsPerDay);
        return NULL;
}

void* TerrainCell::tuneTraitsThreadMaker(void* threadArgument)
{
        struct ThreadArgument* argument = static_cast<ThreadArgument*>(threadArgument);
        TerrainCell* terrainCell = argument->terrainCell;
        int day = argument->timeStep;
        int timeStepsPerDay = argument->timeStepsPerDay;
        //CAREFUL WITH COUT, CHANGE...
        terrainCell->tuneTraits(day, timeStepsPerDay, cout);
        return NULL;
}
//#endif //_PTHREAD
*/

#ifdef _DEBUG
void TerrainCell::increaseMultipleSameSearchedAnimalToday() {theWorld->multipleSameSearchedAnimalToday++;}
void TerrainCell::increaseMultipleSameEncounteredAnimalToday() {theWorld->multipleSameEncounteredAnimalToday++;}
void TerrainCell::increaseMultipleSameHuntedAnimalToday() {theWorld->multipleSameHuntedAnimalToday++;}
void TerrainCell::increaseMultipleSamePredatedAnimalToday() {theWorld->multipleSamePredatedAnimalToday++;}
#endif
