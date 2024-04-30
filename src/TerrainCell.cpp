#include "TerrainCell.h"
#include "World.h"
#include "Animal.h"
#include "Maths/Random.h"
#include "Edible.h"
#include "Correlosome.h"
#include <thread>
#include "Utilities.h"

/*
#include "tbb/tbb.h"

using namespace tbb;
*/
using namespace std;

TerrainCell::TerrainCell(World * newWorld, int x, int y, int z, float newSize)
{
	theWorld = newWorld;
	myCoordinates.setX(x);
	myCoordinates.setY(y);
	myCoordinates.setZ(z);
	setSize(newSize);
	obstacle = false;
	temperature = 0.0;
	moisture = 0.0;

	timeStepsBetweenRainEvents = 1;
	setRelativeHumidityOnRainEvent(0);
	auxInitialFungiBiomass = 0.0;
	auxInitialFungusSpeciesId = -1;
	for (unsigned int i = Animal::LIFE_STAGES::UNBORN; i <= Animal::LIFE_STAGES::SHOCKED; ++i)
	{
		animals[i] = new map<Species*, vector<Edible*> *>();
		for (vector<Species *>::iterator speciesIt = theWorld->existingAnimalSpecies.begin(); speciesIt != theWorld->existingAnimalSpecies.end(); speciesIt++)
		{
			(*animals[i])[*speciesIt] = new vector<Edible*>();
		}
	}
	animalsToMove = new vector<Edible*>();
	animalsToReproduce = new vector<Edible*>();
	animalsToMetabolizeAndGrowth = new vector<Edible*>();
}

TerrainCell::~TerrainCell()
{
	for(auto it = animals.begin(); it != animals.end(); it++)
	{
		map<Species* , vector<Edible*> *> * animalsInThisLifeStage = (*it).second;
		for(auto lifeStageIt = animalsInThisLifeStage->begin(); lifeStageIt != animalsInThisLifeStage->end(); lifeStageIt++)
		{
			vector<Edible*> * animalsInThisSpecies = (*lifeStageIt).second;
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

	for(auto it = fungi.begin(); it != fungi.end(); it++)
	{
		delete (*it);
	}
	fungi.clear();

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

void TerrainCell::setAuxInitialFungiBiomass(float auxInitialFungiBiomass, int auxInitialFungusSpeciesId)
{
	this->auxInitialFungiBiomass = auxInitialFungiBiomass;
	this->auxInitialFungusSpeciesId = auxInitialFungusSpeciesId;
}

void TerrainCell::addFungus(Fungus* newFungus)
{
	newFungus->setPosition(this);
	//newFungus->setBiomass(0);
	fungi.push_back(newFungus);
}

void TerrainCell::deleteFungus(Edible* edible)
{
	fungi.erase(std::remove(fungi.begin(), fungi.end(), edible), fungi.end());
}

void TerrainCell::addAnimal(Edible* newAnimal)
{
	vector<Edible* > * animalsVector = animals[newAnimal->getLifeStage()]->at(newAnimal->getSpecies());
	animalsVector->push_back(newAnimal);
}

double TerrainCell::turnEdibleIntoDryMassToBeEaten(Edible* targetEdible, int day, Edible* predatorEdible, double leftovers)
{
	#ifdef _DEBUG
	cout << predatorId << " " << "predated " << targetEdible->getId() << endl << flush;
	#endif

	float targetEdibleProfitability = predatorEdible->getSpecies()->getEdibleProfitability(targetEdible->getSpecies());

	//double dryMassToBeEaten = 0;

	double dryMassToBeEaten = targetEdible->turnIntoDryMassToBeEaten(predatorEdible->getRemainingVoracity(), targetEdibleProfitability + predatorEdible->getAssim(), leftovers);
	

	if(targetEdible->getSpecies()->isMobile())
	{
		targetEdible->setNewLifeStage(Animal::LIFE_STAGES::PREDATED, day, predatorEdible->getId());

		animalsToMove->erase(std::remove(animalsToMove->begin(), animalsToMove->end(), targetEdible), animalsToMove->end());
	}
	else
	{
		
		//dryMassToBeEaten = targetEdible->turnIntoDryMassToBeEaten(predatorEdible->getRemainingVoracity(), targetEdibleProfitability + predatorEdible->getAssim());
		//arthros and for dinos, very big bug in feeding on fungi and updating ungi fixed
		/* cout << "BEFOREEEEEEEEEE AND AFTEEEEEEEERRRRRRRRRRRRRRRRRR" << endl;
		cout << targetEdible->calculateDryMass() << endl;
		cout << dryMassToBeEaten << endl; */
		targetEdible->substractBiomass(dryMassToBeEaten);
		//cout << targetEdible->calculateDryMass() << endl;
	
	}

	if(dryMassToBeEaten < 0)
	{
		cout << "Edible id: " << predatorEdible->getId() << " (" << predatorEdible->getSpecies()->getScientificName() << ") - The dry mass to be eaten after considering profitability resulted in a negative value." << endl;
	}

	return dryMassToBeEaten;
}

void TerrainCell::changeAnimalToSenesced(Edible* targetAnimal, int day)
{
	#ifdef _DEBUG
	cout << predatorId << " " << "predated " << targetAnimal->getId() << endl << flush;
	#endif

	targetAnimal->setNewLifeStage(Animal::LIFE_STAGES::SENESCED, day);

	animalsToMove->erase(std::remove(animalsToMove->begin(), animalsToMove->end(), targetAnimal), animalsToMove->end());
}

void TerrainCell::eraseAnimal(Edible* oldEdible, int oldStage)
{
	vector<Edible*> * listOfAnimals = animals[oldStage]->at(oldEdible->getSpecies());
	listOfAnimals->erase(std::remove(listOfAnimals->begin(), listOfAnimals->end(), oldEdible), listOfAnimals->end());
}

void TerrainCell::setMaximumCapacities(float maximumFungiCapacity)
{
	this->maximumFungiCapacity = maximumFungiCapacity;
	maximumPreysCapacity = 0.08*pow(maximumFungiCapacity,0.75);
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
	if (relativeHumidityOnRainEvent >= 0)
	{
		this->moisture = relativeHumidityOnRainEvent;
		this->relativeHumidityOnRainEvent = relativeHumidityOnRainEvent;
	}
	else
	{
		std::cerr << "Water resource cannot be negative on a terrain cell" << std::endl;
		exit(-1);
	}
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
		std::cout << std::fixed << std::setprecision(STOICHIOMETRY_PRECISION) << "Cell Stoichiometry (Water:C:N:P): "
				<< this->moisture / min << ":" << this->C / min << ":" << this->N / min << ":" << this->P / min
				<< std::endl;
	else
		std::cout << std::fixed << std::setprecision(STOICHIOMETRY_PRECISION) << "Cell Stoichiometry (C:N:P): "
				<< this->C / min << ":" << this->N / min << ":" << this->P / min << std::endl;

}

std::vector<TerrainCell *> * TerrainCell::getCellsAtDistance(int distance, double searchArea, bool isMobile)
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
									if(!isMobile && theWorld->getCell(z, y, x)->isInPatch()){ //so fungus spreads only within the original patches
									//TODO Jordi check: does this optimize animal search behavior?
									cellToBeAdded = theWorld->getCell(z, y, x);
									if(cellToBeAdded != NULL && cellToBeAdded != this && !cellToBeAdded->isObstacle())
									{
										newNeighbors->push_back(cellToBeAdded);
									}
									}else{//isMobile, an animal
									cellToBeAdded = theWorld->getCell(z, y, x);
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

std::vector<TerrainCell *> * TerrainCell::getCellsWithinDistance(int maxDistance, double searchArea)
{
	//TODO cellsWithinDistance vector might be useful in order to improve performance, although it uses so much memory
	//map<unsigned int, std::vector<TerrainCell *> *>::iterator it = cellsWithinDistance.find(maxDistance);
	//it = cellsWithinDistance.find(maxDistance);
	//if (it == cellsWithinDistance.end())
	//{
		std::vector<TerrainCell *> * newNeighbors = new std::vector<TerrainCell *>();
		int myZ, myY, myX;
		int worldDepth, worldLength, worldWidth;

		myZ = myCoordinates.Z();
		myY = myCoordinates.Y();
		myX = myCoordinates.X();

		worldDepth = theWorld->getDepth();
		worldLength = theWorld->getLength();
		worldWidth = theWorld->getWidth();

		int minX = myX - min(maxDistance, myX);
		int maxX = myX + min(maxDistance, (worldWidth-1)-myX);
		int minY = myY - min(maxDistance, myY);
		int maxY = myY + min(maxDistance, (worldLength-1)-myY);
		int minZ = myZ - min(maxDistance, myZ);
		int maxZ = myZ + min(maxDistance, (worldDepth-1)-myZ);

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
								cellToBeAdded = theWorld->getCell(z, y, x);
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

void TerrainCell::diePredatorsFromBackground(int timeStep)
{
	for(vector<Species*>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		vector<Edible*> activeAnimals = *(getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies));

		if(!currentAnimalSpecies->getEdibleAnimalSpecies()->empty())
		{
			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals.begin(); activeAnimalsIt != activeAnimals.end(); activeAnimalsIt++)
			{
				(*activeAnimalsIt)->dieFromBackground(timeStep);
			}
		}
	}
}

void TerrainCell::breedAnimals(int timeStep, int timeStepsPerDay, fs::path outputDirectory)
{
	//shuffleList(&(animals[Animal::LIFE_STAGES::REPRODUCING]));

	for (vector<Edible*>::iterator animalsToReproduceIt = animalsToReproduce->begin(); animalsToReproduceIt != animalsToReproduce->end(); animalsToReproduceIt++)
	{
		Edible* female = (*animalsToReproduceIt);

		list<Edible*> * offSprings = female->breed(timeStep, timeStepsPerDay, temperature);

		ofstream geneticsFile, constitutiveTraitsFile;
		createOutputFile(geneticsFile, outputDirectory, "animal_genetics", "txt", ofstream::app);
		if(!geneticsFile.is_open()) {
			cerr << "Error opening the file." << endl;
		} else {
			createOutputFile(constitutiveTraitsFile, outputDirectory, "animal_constitutive_traits", "txt", ofstream::app);
			if(!constitutiveTraitsFile.is_open()) {
				cerr << "Error opening the file." << endl;
			} else {
				for(list<Edible*>::iterator offSpringsIt = offSprings->begin(); offSpringsIt != offSprings->end(); offSpringsIt++)
				{
					//(*offSpringsIt)->printGenetics(geneticsFile);
				}
				geneticsFile.close();
				constitutiveTraitsFile.close();
			}
		}

		vector<Edible*> * unbornAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::UNBORN, female->getSpecies());
		unbornAnimals->insert(unbornAnimals->end(), offSprings->begin(), offSprings->end());
	
		offSprings->clear();
		delete offSprings;
	}
	animalsToReproduce->clear();
}

void TerrainCell::metabolizeAnimals(int timeStep, int timeStepPerDay)
{
	for(vector<Species*>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
		for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
		{
			(*activeAnimalsIt)->metabolize(timeStep, timeStepPerDay);
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
	for(vector<Species*>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		vector<Edible*> activeAnimals = *(getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies));
		for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals.begin(); activeAnimalsIt != activeAnimals.end(); activeAnimalsIt++)
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

	for(vector<Species*>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		vector<Edible*> * reproducingAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::REPRODUCING, currentAnimalSpecies);

		//Prepare the animals to be reproduced today
		animalsToReproduce->insert(animalsToReproduce->end(), reproducingAnimals->begin(), reproducingAnimals->end());
	}
}

void TerrainCell::tuneTraits(int day, int timeStepsPerDay, ostream& tuneTraitsFile)
{
	for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
		vector<Edible*> * pupaAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::PUPA, currentAnimalSpecies);
		vector<Edible*> * satiatedAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::SATIATED, currentAnimalSpecies);
		vector<Edible*> * handlingAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::HANDLING, currentAnimalSpecies);
		vector<Edible*> * diapauseAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::DIAPAUSE, currentAnimalSpecies);
		vector<Edible*> * tuneTraitsAnimals = new vector<Edible*>();
		tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), activeAnimals->begin(), activeAnimals->end());
		tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), pupaAnimals->begin(), pupaAnimals->end());
		tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), satiatedAnimals->begin(), satiatedAnimals->end());
		tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), handlingAnimals->begin(), handlingAnimals->end());
		tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), diapauseAnimals->begin(), diapauseAnimals->end());

		for (vector<Edible*>::iterator tuneTraitsAnimalsIt = tuneTraitsAnimals->begin(); tuneTraitsAnimalsIt != tuneTraitsAnimals->end(); tuneTraitsAnimalsIt++)
		{
			//Growth Curves calculus added here so it will be computed every step with the new temperature
			(*tuneTraitsAnimalsIt)->calculateGrowthCurves(temperature, tuneTraitsFile, true, -1); //Dinosaurs
			(*tuneTraitsAnimalsIt)->increaseAge(1);
			(*tuneTraitsAnimalsIt)->tuneTraits(day, timeStepsPerDay, temperature, moisture, tuneTraitsFile, true, false);
		}
		delete tuneTraitsAnimals;

		activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
		//Prepare the animals to be moved today
		animalsToMove->insert(animalsToMove->end(), activeAnimals->begin(), activeAnimals->end());
		//animalsToMove->insert(animalsToMove->end(), reproducingAnimals->begin(), reproducingAnimals->end());
	}
}

void TerrainCell::commitFungiSpread()
{
	//TODO Añadir las contribuciones de cada especie EN EL MISMO ORDEN
	//que se añadieron los crecimientos en la celda ORIGEN.
	//std::random_shuffle(fungi.begin(), fungi.end());

	vector<Edible*> markedForRemoval;
	for (std::vector<Edible*>::iterator it = fungi.begin(); it != fungi.end(); it++)
	{
		(*it)->processExternalContributions();
		if ((*it)->isExtinct())
		{
			markedForRemoval.push_back((*it));
		}
	}

	for (std::vector<Edible*>::iterator it = markedForRemoval.begin(); it != markedForRemoval.end(); it++)
	{
		deleteFungus((*it));
		delete(*it);
	}
}

void TerrainCell::setFungiFromChemostatEffect(vector<Species *>* existingFungiSpecies, double increaseForChemostatEffect) {
	if(moisture > 0)
	{
		//LO QUITAMOS PROVISIONALMENTE!
		//double fractionPerSpecies = 1.0/(double)existingFungiSpecies->size();
		for (std::vector<Species*>::iterator it = existingFungiSpecies->begin(); it != existingFungiSpecies->end(); it++)
		{
			Edible* currentFungus = getFungus(*it);
			if(currentFungus != NULL && (int)(*it)->getId() == auxInitialFungusSpeciesId)
			{
				//TODO PROVISIONAL! CAMBIAR POR MOISTURE
				currentFungus->setBiomass(increaseForChemostatEffect*auxInitialFungiBiomass); // Adds biomass to this fungus species living within this cell

			}
			else if((int)(*it)->getId() == auxInitialFungusSpeciesId)
			{
				//TODO PROVISIONAL! CAMBIAR POR MOISTURE
				addFungus(new Fungus((*it), increaseForChemostatEffect*auxInitialFungiBiomass, this));
			}
		}
	}
}

void TerrainCell::growFungi(int timeStep, bool competition, double maxCapacity, int size)
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
	std::random_shuffle(fungi.begin(), fungi.end());
	vector<Edible*> markedForRemoval;
	for (std::vector<Edible *>::iterator it = fungi.begin(); it != fungi.end(); it++)
	{

		//cout << "wetMass:  " << (*it)->calculateWetMass() << endl;
		double excess = (*it)->grow(timeStep); // Grows this fungus species living within this cell
		
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
				std::vector<TerrainCell *> * neighbors = getCellsAtDistance(1, DBL_MAX, isMobile);

				Species* currentSpecies = (*it)->getSpecies();
				double massPerNeighbor = excess / neighbors->size(); // Distribute equally across neighbors
				
				Edible* auxFungus;

				for (std::vector<TerrainCell *>::iterator neighborIt = neighbors->begin(); neighborIt != neighbors->end(); neighborIt++)
				{
					auxFungus = (*neighborIt)->getFungus(currentSpecies);

					double totBiomass = (*neighborIt)->getTotalFungusBiomass();

					if (auxFungus != NULL)
					{
						auxFungus->addBiomass(massPerNeighbor, competition, maxCapacity, size, totBiomass);
					}
					else
					{   //spread to a cell where that fungus species was extinct
						if((*neighborIt)->isInPatch()){
						(*neighborIt)->addFungus(new Fungus(currentSpecies, massPerNeighbor, this)); //, massPerNeighbor, this
						auxFungus = (*neighborIt)->getFungus(currentSpecies);
						auxFungus->addBiomass(massPerNeighbor, competition, maxCapacity, size, totBiomass);
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

	for (std::vector<Edible *>::iterator it = markedForRemoval.begin(); it != markedForRemoval.end(); it++)
	{
		fungi.erase(std::remove(fungi.begin(), fungi.end(), (*it)), fungi.end());
		delete (*it);
	}





}

/*
 * Returns, for each requested fungus species, their stocks in this cell.
 * This is used to take decisions on preys movement but not taking into account the
 * potentially existing of predators on the cell.
 */
map<Species *, double> * TerrainCell::getFungiStock(Animal* animalWhoIsEvaluating, bool discardZero)
{
	vector<Species *> * edibleFungusSpecies = animalWhoIsEvaluating->getSpecies()->getEdibleFungusSpecies();
	map<Species *, double> * result = new map<Species *, double> ();

	float mass;
	for (vector<Species *>::iterator it = edibleFungusSpecies->begin(); it != edibleFungusSpecies->end(); it++)
	{
		mass = getFungusBiomass(*it);

		if ((mass > 0) || (!discardZero))
		{
			(*result)[(*it)] = mass;
		}
	}

	return result;
}

/*
 * Returns for each requested fungus species, the ratio between their stocks and the number of
 * predator in this cell. This is used to take decisions on preys movement.
 */
map<Species *, double> * TerrainCell::getRateOfFungiPredators(Animal* animalWhoIsEvaluating, bool discardZero)
{
	// TODO ROMAN A JORDI: Sigo sin ver esto claro (todos los ratios)
	map<Species *, double> * result = new map<Species *, double> ();
	unsigned int predatorsCounter = 1; // +1: Avoid division by zero
	for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies->canEatAnimalSpecies(animalWhoIsEvaluating->getSpecies()))
		{
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
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
	vector<Species *> * edibleFungusSpecies = animalWhoIsEvaluating->getSpecies()->getEdibleFungusSpecies();
	for (vector<Species *>::iterator it = edibleFungusSpecies->begin(); it != edibleFungusSpecies->end(); it++)
	{
		mass = getFungusBiomass(*it);

		if ((mass > 0) || (!discardZero))
		{
			(*result)[(*it)] = mass * inversePredatorsCounter; // This is like dividing as I have inverted predatorsCounter first
		}
	}

	return result;
}



tuple<double, double, double> TerrainCell::getCellEvaluation(Edible* animalWhoIsEvaluating)
{
//arthros and for dinos to force searching for preferred food items be animals or fungi, prompts mobility 
	double preferenceThresholdForEvaluation = 0.0;//defaults to 0 but it is a parameter to include in run_params.json
	
	double totalPredatoryRiskEdibilityValue = 0.0;
	double totalEdibilityValue = 0.0;
	double totalConspecificBiomass = 0.0;
	for(vector<Species*>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies->canEatAnimalSpecies(animalWhoIsEvaluating->getSpecies()))
		{
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				Edible* predatoryRiskAnimal = (*activeAnimalsIt);
				if (predatoryRiskAnimal->canEatEdible(animalWhoIsEvaluating))
				{
					double predatoryRiskEdibilityValue = predatoryRiskAnimal->calculatePredatoryRiskEdibilityValue(animalWhoIsEvaluating);
					totalPredatoryRiskEdibilityValue += predatoryRiskEdibilityValue;
				}
			}
		}
		if(animalWhoIsEvaluating->getSpecies()->canEatAnimalSpecies(currentAnimalSpecies))
		{
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				Edible* edibleAnimal = (*activeAnimalsIt);
				if (!animalWhoIsEvaluating->hasTriedToHunt(edibleAnimal))
				{
					
					if(animalWhoIsEvaluating->getSpecies()->getEdiblePreference(edibleAnimal->getSpecies()) > preferenceThresholdForEvaluation){	
					double edibilityValue = animalWhoIsEvaluating->calculateEdibilityValueWithMass(edibleAnimal);
					totalEdibilityValue += edibilityValue;
					}
				
				}
			}
		}
		if(animalWhoIsEvaluating->getSpecies() == currentAnimalSpecies)
		{
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				Edible* conspecificAnimal = (*activeAnimalsIt);
				totalConspecificBiomass += conspecificAnimal->calculateDryMass();
			}
		}
	}

	vector<Species *> * edibleFungusSpecies = animalWhoIsEvaluating->getSpecies()->getEdibleFungusSpecies();
	for (vector<Species *>::iterator it = edibleFungusSpecies->begin(); it != edibleFungusSpecies->end(); it++)
	{
		Species* currentFungusSpecies = *it;
		Edible* fungus = getFungus(currentFungusSpecies);
		//arthros to force search for preferred food
			if(fungus != NULL)
			{
				if(animalWhoIsEvaluating->getSpecies()->getEdiblePreference(fungus->getSpecies()) > preferenceThresholdForEvaluation){	
					totalEdibilityValue += animalWhoIsEvaluating->calculateEdibilityValueWithMass(fungus);
				}
			}
		
	}

	return make_tuple(totalEdibilityValue, totalPredatoryRiskEdibilityValue, totalConspecificBiomass);
}

unsigned int TerrainCell::getNumberOfMatureFemales(Animal* reproducingAnimal)
{
	vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, reproducingAnimal->getSpecies());
	vector<Edible*> * satiatedAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::SATIATED, reproducingAnimal->getSpecies());
	vector<Edible*> * handlingAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::HANDLING, reproducingAnimal->getSpecies());
	vector<Edible*> * diapauseAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::DIAPAUSE, reproducingAnimal->getSpecies());
	vector<Edible*> * searchableAnimals = new vector<Edible*>();

	searchableAnimals->insert(searchableAnimals->end(), activeAnimals->begin(), activeAnimals->end());
	searchableAnimals->insert(searchableAnimals->end(), satiatedAnimals->begin(), satiatedAnimals->end());
	searchableAnimals->insert(searchableAnimals->end(), handlingAnimals->begin(), handlingAnimals->end());
	searchableAnimals->insert(searchableAnimals->end(), diapauseAnimals->begin(), diapauseAnimals->end());

	unsigned int counter = 0;
	for (vector<Edible*>::iterator searchableAnimalsIt = searchableAnimals->begin(); searchableAnimalsIt != searchableAnimals->end(); searchableAnimalsIt++)
	{
		if ((*searchableAnimalsIt)->getGender() == AnimalSpecies::GENDERS::FEMALE && (*searchableAnimalsIt)->isMature() && !(*searchableAnimalsIt)->isMated())
		{
			counter++;
		}
	}
	delete searchableAnimals;
	return counter;
}

vector<Edible*> * TerrainCell::getAnimalsBySpecies(unsigned int lifeStage, Species* species)
{
	
	return animals.at(lifeStage)->at(species);
		
}


unsigned int TerrainCell::getNumberOfEdibleAnimals(Animal* animalWhoIsEvaluating)
{
	unsigned int preysCounter = 0;

	for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		if(animalWhoIsEvaluating->getSpecies()->canEatAnimalSpecies(currentAnimalSpecies))
		{
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				if (animalWhoIsEvaluating->canEatEdible((*activeAnimalsIt)))
				{
					preysCounter++;
				}
			}
		}
	}
	return preysCounter;
}

void TerrainCell::printInteractionMatrices(ostream& encountersMatrixFile, ostream& predationsMatrixFile,
		ostream& nodesMatrixFile, unsigned int totalInitialPopulation )
{
	unsigned int j;
	for (unsigned int i = 0; i < animals.size(); ++i)
	{
		j = 0;
		for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
		{
			Species* currentAnimalSpecies = *animalSpeciesIt;
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(i, currentAnimalSpecies);

			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
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

ostream& TerrainCell::printAnimalsVoracities(int timeStep, int timeStepsPerDay, ostream& os)
{
	//for (unsigned int i = 0; i < animals.size(); ++i)
	//{
		for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
		{
			Species* currentAnimalSpecies = *animalSpeciesIt;
			//activeAnimals = getAnimalsBySpecies(i, currentAnimalSpecies);
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);

			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				(*activeAnimalsIt)->printVoracities(timeStep, timeStepsPerDay, os);
			}
		}
	//}
	return os;
}

ostream& TerrainCell::printAnimals(ostream& os)
{
	for (unsigned int i = 0; i < animals.size(); ++i)
	{
		for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
		{
			Species* currentAnimalSpecies = *animalSpeciesIt;
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(i, currentAnimalSpecies);

			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				os << *(*activeAnimalsIt) << endl;
			}
		}
	}
	return os;
}

ostream& TerrainCell::printCell(ostream& os)
{
	os << getX() << "\t"
	   << getY() << "\t"
	   << getZ() << "\t";

	for (vector<Species *>::iterator itFungiSpecies = theWorld->existingFungiSpecies.begin(); itFungiSpecies != theWorld->existingFungiSpecies.end(); itFungiSpecies++)
	{
		os << getFungusBiomass(*itFungiSpecies) << "\t";
	}

	for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		int currentNumberOfAnimals = 0;
		for (unsigned int i = 0; i < animals.size(); ++i)
		{
			Species* currentAnimalSpecies = *animalSpeciesIt;
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(i, currentAnimalSpecies);
			currentNumberOfAnimals += activeAnimals->size();
		}
		os << currentNumberOfAnimals << "\t";
	}
	os << endl;
	return os;
}

void TerrainCell::activateAndResumeAnimals(int timeStep, int timeStepsPerDay)
{
	#ifdef _DEBUG
	cout << "what is happening: " << theWorld->getExistingAnimalSpecies()->at(0)->getScientificName() << endl << flush;
	#endif

	for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		vector<Edible*> unbornAnimals = *(getAnimalsBySpecies(Animal::LIFE_STAGES::UNBORN, currentAnimalSpecies));
		vector<Edible*> pupaAnimals = *(getAnimalsBySpecies(Animal::LIFE_STAGES::PUPA, currentAnimalSpecies));
		vector<Edible*> * handlingAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::HANDLING, currentAnimalSpecies);
		vector<Edible*> diapauseAnimals = *(getAnimalsBySpecies(Animal::LIFE_STAGES::DIAPAUSE, currentAnimalSpecies));

		#ifdef _DEBUG
		cout << "but am I actually doing anything?: unborn/active: " << unbornAnimals->size() << "/" << activeAnimals->size() << " -> " << flush;
		#endif

		for (vector<Edible*>::iterator diapauseAnimalsIt = diapauseAnimals.begin(); diapauseAnimalsIt != diapauseAnimals.end(); diapauseAnimalsIt++)
		{
			(*diapauseAnimalsIt)->isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(moisture);
		}

		for (vector<Edible*>::iterator unbornAnimalsIt = unbornAnimals.begin(); unbornAnimalsIt != unbornAnimals.end(); unbornAnimalsIt++)
		{
			(*unbornAnimalsIt)->isReadyToBeBorn(timeStep, timeStepsPerDay);
		}

		for (vector<Edible*>::iterator pupaAnimalsIt = pupaAnimals.begin(); pupaAnimalsIt != pupaAnimals.end(); pupaAnimalsIt++)
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

		vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);

		#ifdef _DEBUG
		cout << unbornAnimals->size() << "/" << activeAnimals->size() << endl << flush;
		#endif

	}

	#ifdef _DEBUG
	cout << "activation finished for terrain field " << this << endl << flush;
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

Edible* TerrainCell::getFungus(Species* species)
{
	for (std::vector<Edible *>::iterator it = fungi.begin(); it != fungi.end(); it++)
	{
		if ((*it)->getSpecies() == species)
		{
			return (*it);
		}
	}
	return NULL;
}

vector<Edible *>* TerrainCell::getFungi()
{
	return &fungi;
}

float TerrainCell::getFungusBiomass(Species* species)
{
	Edible* fungus = getFungus(species);

	if (fungus != NULL)
	{
		return fungus->calculateWetMass();
	}
	else
	{
		return 0;
	}
}

//TODO - generalize this independently of alphabetical order
double TerrainCell::getTotalFungusBiomass()
{
	double mass = 0;
	for (vector<Edible *>::iterator it = fungi.begin(); it != (fungi.end()); it++)
	{
		mass = mass + (*it)->calculateWetMass();
	}
	return mass;
}

unsigned int TerrainCell::getLifeStagePopulation(unsigned int stage, unsigned int huntingMode)
{
	unsigned int population = 0;
	for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies->getDefaultHuntingMode() == huntingMode)
		{
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(stage, currentAnimalSpecies);
			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				population++;
			}
		}
	}
	return population;
}

unsigned int TerrainCell::getLifeStagePopulation(unsigned int stage, Species* species)
{
	vector<Edible*> * activeAnimals = getAnimalsBySpecies(stage, species);
	return activeAnimals->size();
}

void TerrainCell::updateWorldFungiBiomassAndAnimalsPopulation(map<Species*, double>* worldFungiBiomass, map<Species*, vector<unsigned int>* >* worldAnimalsPopulation)
{
	for (vector<Edible *>::iterator itFungi = fungi.begin(); itFungi != fungi.end(); itFungi++)
	{
		worldFungiBiomass->at((*itFungi)->getSpecies()) += (*itFungi)->calculateWetMass();
	}

	for (unsigned int lifeStage = 0; lifeStage < animals.size(); ++lifeStage)
	{
		for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
		{
			Species* currentAnimalSpecies = *animalSpeciesIt;
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);
			worldAnimalsPopulation->at(currentAnimalSpecies)->at(lifeStage) += activeAnimals->size();
		}
	}
}

void TerrainCell::updateAnimalsPopulationAndGeneticsFrequencies(map<Species*, vector<unsigned int>* >* worldAnimalsPopulation, map<Species*, vector<set<double>* >* >* worldGeneticsFrequencies)
{

	const vector<Allele*>* currentChromosomeAlleles;
	for (unsigned int lifeStage = 0; lifeStage < animals.size(); ++lifeStage)
	{
		for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
		{
			Species* currentAnimalSpecies = *animalSpeciesIt;
			vector<Edible*> * activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);

			worldAnimalsPopulation->at(currentAnimalSpecies)->at(lifeStage)++;
			for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
			{
				const vector<pair<Correlosome*, Correlosome*> >* currentChromosomes = (*activeAnimalsIt)->getGenome()->getHomologousCorrelosomes();
				for (unsigned int selectedTrait = 0; selectedTrait < currentAnimalSpecies->getNumberOfTraits(); ++selectedTrait)
				{
					currentChromosomeAlleles = currentChromosomes->at(selectedTrait).first->getAlleles();
					for (unsigned int selectedLoci = 0; selectedLoci < currentAnimalSpecies->getNumberOfLociPerTrait(); ++selectedLoci)
					{
						worldGeneticsFrequencies->at(currentAnimalSpecies)->at(selectedTrait*currentAnimalSpecies->getNumberOfLociPerTrait()+selectedLoci)->insert(currentChromosomeAlleles->at(selectedLoci)->getValue());
					}
					currentChromosomeAlleles = currentChromosomes->at(selectedTrait).second->getAlleles();
					for (unsigned int selectedLoci = 0; selectedLoci < currentAnimalSpecies->getNumberOfLociPerTrait(); ++selectedLoci)
					{
						//We multiply x2 because there are PAIRS of chromosomes. We store this data this way: T1_CR1, T2_CR1, T3_CR1.... T2_CR1, T2_CR2, ...
						worldGeneticsFrequencies->at(currentAnimalSpecies)->at(currentAnimalSpecies->getNumberOfLociPerTrait()*currentAnimalSpecies->getNumberOfTraits()+selectedTrait*currentAnimalSpecies->getNumberOfLociPerTrait()+selectedLoci)->insert(currentChromosomeAlleles->at(selectedLoci)->getValue());
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
			activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
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
	vector<Edible*> * lifestageAnimals = getAnimalsBySpecies(animalToMigrate->getLifeStage(), animalToMigrate->getSpecies());
	lifestageAnimals->erase(std::remove(lifestageAnimals->begin(), lifestageAnimals->end(), animalToMigrate), lifestageAnimals->end());
	newPosition->addAnimal(animalToMigrate);
}

void TerrainCell::assimilateFoodMass(int timeStep)
{
	for(vector<Species*>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
		for (vector<Edible*>::iterator activeAnimalsIt = activeAnimals->begin(); activeAnimalsIt != activeAnimals->end(); activeAnimalsIt++)
		{
			(*activeAnimalsIt)->assimilateFoodMass(timeStep);
		}
	}
}

void TerrainCell::moveAnimals(int timeStep, int timeStepsPerDay, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, ostream& edibilitiesFile, float exitTimeThreshold)
{
	Edible* currentAnimal;
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
		currentAnimal->move(timeStep, timeStepsPerDay, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile);

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
		cout << "too many animals for too little food!!!" << endl;
		exit(-1);
	}

	for(vector<Species*>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		vector<Edible*> * activeAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::ACTIVE, currentAnimalSpecies);
		vector<Edible*> * satiatedAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::SATIATED, currentAnimalSpecies);
		vector<Edible*> * handlingAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::HANDLING, currentAnimalSpecies);

		//Prepare the animals to metabolize and grow
		animalsToMetabolizeAndGrowth->insert(animalsToMetabolizeAndGrowth->end(), activeAnimals->begin(), activeAnimals->end());
		animalsToMetabolizeAndGrowth->insert(animalsToMetabolizeAndGrowth->end(), satiatedAnimals->begin(), satiatedAnimals->end());
		animalsToMetabolizeAndGrowth->insert(animalsToMetabolizeAndGrowth->end(), handlingAnimals->begin(), handlingAnimals->end());
	}

	//cellsAtDistance.clear();
}

void TerrainCell::purgeDeadAnimals()
{
	
	for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;

		vector<Edible*> * starvedAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::STARVED, currentAnimalSpecies);
		vector<Edible*> * predatedAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::PREDATED, currentAnimalSpecies);
		vector<Edible*> * backgroundAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::BACKGROUND, currentAnimalSpecies);
		vector<Edible*> * senescedAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::SENESCED, currentAnimalSpecies);
		vector<Edible*> * shockedAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::SHOCKED, currentAnimalSpecies);

		for (vector<Edible*>::iterator it = starvedAnimals->begin(); it != starvedAnimals->end(); it++)
		{
			delete (*it);
		}
		starvedAnimals->clear(); //this turns the size of the vector to zero
		
		for (vector<Edible*>::iterator it = predatedAnimals->begin(); it != predatedAnimals->end(); it++)
		{
			delete (*it);
		}
		predatedAnimals->clear();

	 	for (vector<Edible*>::iterator it = backgroundAnimals->begin(); it != backgroundAnimals->end(); it++)
		{
			delete (*it); //->getGenome()->getHomologousChromosomes());
			//delete (*aGenome);
			//delete (*it);
		} 
		backgroundAnimals->clear();
		//free(backgroundAnimals);
		

		for (vector<Edible*>::iterator it = senescedAnimals->begin(); it != senescedAnimals->end(); it++)
		{
			delete (*it);
		}
		senescedAnimals->clear();

		for (vector<Edible*>::iterator it = shockedAnimals->begin(); it != shockedAnimals->end(); it++)
		{
			delete (*it);
		}
		shockedAnimals->clear();
	} 
}

void TerrainCell::deleteExtinguishedReproducingAnimals(Species* extinguishedAnimalSpecies)
{
	for(vector<Species *>::iterator animalSpeciesIt = theWorld->existingAnimalSpecies.begin(); animalSpeciesIt != theWorld->existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies == extinguishedAnimalSpecies)
		{
			vector<Edible*> * reproducingAnimals = getAnimalsBySpecies(Animal::LIFE_STAGES::REPRODUCING, currentAnimalSpecies);
			for (vector<Edible*>::iterator reproducingAnimalsIt = reproducingAnimals->begin(); reproducingAnimalsIt != reproducingAnimals->end();)
			{
				Edible* deletedAnimal = (*reproducingAnimalsIt);
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
