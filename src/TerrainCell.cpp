#include "TerrainCell.h"
#include "ResourceSpecies.h"
#include "Maths/Random.h"
#include "Correlosome.h"
#include <thread>
#include <cmath>
#include "Utilities.h"
#include <algorithm>
#include "LineInfoException.h"

/*
#include "tbb/tbb.h"

using namespace tbb;
*/
using namespace std;
using json = nlohmann::json;

TerrainCell::TerrainCell(int x, int y, int z, float newSize, const json &moistureBasePatch, vector<AnimalSpecies*> *existingAnimalSpecies, vector<ResourceSpecies*> *existingResourceSpecies, WorldInterface* worldInterface)
	: TerrainCellInterface(worldInterface), myCoordinates(x, y, z), existingAnimalSpecies(existingAnimalSpecies), existingResourceSpecies(existingResourceSpecies)
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
			for(auto &animal : *animalsInThisSpecies)
			{
				delete animal;
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

	float targetEdibleProfitability = predatorEdible->getSpecies()->getEdibleProfitability(targetEdible->getSpecies()->getId(), predatorEdible->getInstar(), targetEdible->getInstar());

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

std::vector<TerrainCellInterface *> * TerrainCell::getCellsAtDistance(int distance, double searchArea, bool isMobile, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth)
{
	TerrainCellInterface* cellToBeAdded;

	std::vector<TerrainCellInterface *> * newNeighbors = new std::vector<TerrainCellInterface *>();

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
								if(!isMobile && worldInterface->getCell(z, y, x)->isInPatch()){ //so resource spreads only within the original patches
									//TODO Jordi check: does this optimize animal search behavior?
									cellToBeAdded = worldInterface->getCell(z, y, x);
									if(cellToBeAdded != NULL && cellToBeAdded != this && !cellToBeAdded->isObstacle())
									{
										newNeighbors->push_back(cellToBeAdded);
									}
								}
								else{//isMobile, an animal
									cellToBeAdded = worldInterface->getCell(z, y, x);
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

	std::vector<TerrainCellInterface *> * searchAreaNeighbors = new std::vector<TerrainCellInterface *>();
	std::vector<TerrainCellInterface *>::iterator newNeighborsIt;
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

std::vector<TerrainCell *> * TerrainCell::getCellsWithinDistance(int maxDistance, double searchArea, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth)
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
								cellToBeAdded = worldInterface->getCell(z, y, x);
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

double TerrainCell::getDistanceToCell(TerrainCellInterface* targetCell)
{
	return sqrt(pow(abs(getX() - targetCell->getX()) * size, 2) + pow(abs(getY() - targetCell->getY()) * size, 2) + pow(abs(getZ() - targetCell->getZ()) * size, 2));
}

void TerrainCell::dieFromBackground(int timeStep, bool growthAndReproTest)
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;

		auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

		for(auto &elem : activeAnimals) {
			for(auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
			{
				(*activeAnimalsIt)->dieFromBackground(timeStep, growthAndReproTest);
			}
		}
	}
}

void TerrainCell::breedAnimals(int timeStep, int timeStepsPerDay, fs::path outputDirectory, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
	//shuffleList(&(animals[LifeStage::REPRODUCING]));

	for (auto animalsToReproduceIt = animalsToReproduce->begin(); animalsToReproduceIt != animalsToReproduce->end(); animalsToReproduceIt++)
	{
		Animal* female = (*animalsToReproduceIt);

		list<Animal*> * offSprings = female->breed(timeStep, timeStepsPerDay, temperature, saveAnimalConstitutiveTraits, constitutiveTraitsFile);

		ofstream geneticsFile;
		createOutputFile(geneticsFile, outputDirectory, "animal_genetics", "txt", ofstream::app);
		if(!geneticsFile.is_open()) {
			cerr << "Error opening the file." << endl;
		} else {
			for(auto offSpringsIt = offSprings->begin(); offSpringsIt != offSprings->end(); offSpringsIt++)
			{
				//(*offSpringsIt)->printGenetics(geneticsFile);
			}
			geneticsFile.close();
		}

		vector<Animal*> * unbornAnimals = animals.at(LifeStage::UNBORN)->at(female->getSpecies());
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
		auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

		for(auto &elem : activeAnimals) {
			for(auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
			{
				(*activeAnimalsIt)->metabolize(timeStep, timeStepPerDay, simulType);
			}
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
		auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

		for(auto &elem : activeAnimals) {
			for(auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
			{
				(*activeAnimalsIt)->grow(timeStep, timeStepsPerDay);
			}
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
		auto reproducingAnimals = getAnimalsBySpecies(LifeStage::REPRODUCING, currentAnimalSpecies);

		for(auto &elem : reproducingAnimals) {
			//Prepare the animals to be reproduced today
			animalsToReproduce->insert(animalsToReproduce->end(), elem.first, elem.second);
		}
	}
}

void TerrainCell::tuneTraits(int day, int timeStepsPerDay, ostream& tuneTraitsFile, SimulType simulType)
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		
		vector<Animal*> * tuneTraitsAnimals = new vector<Animal*>();

		for(auto &lifeStage : {LifeStage::ACTIVE, LifeStage::PUPA, LifeStage::SATIATED, LifeStage::HANDLING, LifeStage::DIAPAUSE}) {
			auto lifeStageAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);
			
			for(auto &elem : lifeStageAnimals) {
				//Prepare the animals to be reproduced today
				tuneTraitsAnimals->insert(tuneTraitsAnimals->end(), elem.first, elem.second);
			}
		}

		for (auto tuneTraitsAnimalsIt = tuneTraitsAnimals->begin(); tuneTraitsAnimalsIt != tuneTraitsAnimals->end(); tuneTraitsAnimalsIt++)
		{
			//Growth Curves calculus added here so it will be computed every step with the new temperature
			(*tuneTraitsAnimalsIt)->calculateGrowthCurves(temperature, tuneTraitsFile, true, -1); //Dinosaurs
			(*tuneTraitsAnimalsIt)->increaseAge(1);
			(*tuneTraitsAnimalsIt)->tuneTraits(day, timeStepsPerDay, temperature, moisture, tuneTraitsFile, true, false, simulType);
		}
		delete tuneTraitsAnimals;

		auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

		for(auto &elem : activeAnimals) {
			//Prepare the animals to be moved today
			animalsToMove->insert(animalsToMove->end(), elem.first, elem.second);
		}
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
				addResource(new Resource((*it), increaseForChemostatEffect*auxInitialResourceBiomass, 0.0, competitionAmongResourceSpecies, massRatio, currentResource->canSpread()));
			}
		}
	}
}

void TerrainCell::growResource(int timeStep, bool competition, double maxCapacity, int size, SimulType simulType, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, bool competitionAmongResourceSpecies, double massRatio)
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
		if((*it)->canSpread() && excess > 0) //spread only if excess > cell size - otherwise is not biologically possible
		{              //(*it)->getSpecies()->getCellMass()                                  
			bool isMobile = (*it)->getSpecies()->isMobile();
			auto neighbors = getCellsAtDistance(1, DBL_MAX, isMobile, worldDepth, worldLength, worldWidth);

			ResourceSpecies* currentSpecies = (*it)->getSpecies();

			double massPerNeighbor;
			if(!neighbors->empty()) {
				massPerNeighbor = excess / neighbors->size(); // Distribute equally across neighbors
			}
			
			Resource* auxResource;

			for (auto neighborIt = neighbors->begin(); neighborIt != neighbors->end(); neighborIt++)
			{
				auxResource = (*neighborIt)->getResource(currentSpecies);

				double totBiomass = (*neighborIt)->getTotalResourceBiomass();

				if (auxResource != NULL)
				{
					auxResource->addBiomass(massPerNeighbor, competition, maxCapacity, size, totBiomass);
				}
				else
				{   //spread to a cell where that resource species was extinct
					if(static_cast<TerrainCell*>((*neighborIt))->isInPatch()){
						static_cast<TerrainCell*>((*neighborIt))->addResource(new Resource(currentSpecies, massPerNeighbor, (*it)->getResourceMaximumCapacity(), competitionAmongResourceSpecies, massRatio, (*it)->canSpread())); //, massPerNeighbor, this
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
map<Species *, double> * TerrainCell::getRateOfResourcePredators(Animal* animalWhoIsEvaluating, const list<Edible*> &ediblesHasTriedToPredate, bool discardZero)
{
	// TODO ROMAN A JORDI: Sigo sin ver esto claro (todos los ratios)
	map<Species *, double> * result = new map<Species *, double> ();
	unsigned int predatorsCounter = 1; // +1: Avoid division by zero
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies->canEatAnimalSpecies(animalWhoIsEvaluating->getSpecies()))
		{
			auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

			for(auto &elem : activeAnimals) {
				for (auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
				{
					if ((*activeAnimalsIt)->canEatEdible(animalWhoIsEvaluating, ediblesHasTriedToPredate))
					{
						predatorsCounter++;
					}
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



tuple<double, double, double> TerrainCell::getCellEvaluation(Animal* animalWhoIsEvaluating, const list<Edible*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
//arthros and for dinos to force searching for preferred food items be animals or resource, prompts mobility 
	double preferenceThresholdForEvaluation = 0.0;//defaults to 0 but it is a parameter to include in run_params.json
	
	unsigned int predatorAnimalSpecies = 0;

	double totalPredatoryRiskEdibilityValue = 0.0;
	double totalEdibilityValue = 0.0;
	double totalConspecificBiomass = 0.0;
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;

		if(currentAnimalSpecies->canEatAnimalSpecies(animalWhoIsEvaluating->getSpecies()))
		{
			predatorAnimalSpecies++;

			auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

			double predatoryRiskEdibilityValue = 0.0;

			for(auto &elem : activeAnimals) {
				for (auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
				{
					Animal* predatoryRiskAnimal = (*activeAnimalsIt);
					if (predatoryRiskAnimal->canEatEdible(animalWhoIsEvaluating, ediblesHasTriedToPredate))
					{
						predatoryRiskEdibilityValue += predatoryRiskAnimal->calculatePredatoryRiskEdibilityValue(animalWhoIsEvaluating, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
					}
				}
			}

			double maximumPredatoryRiskEdibilityValue = currentAnimalSpecies->getK_Value() / (animalWhoIsEvaluating->calculateDryMass() * exp(1.0));

			totalPredatoryRiskEdibilityValue += Math_Functions::linearInterpolate01(predatoryRiskEdibilityValue, maximumPredatoryRiskEdibilityValue);
		}

		if(animalWhoIsEvaluating->getSpecies()->canEatAnimalSpecies(currentAnimalSpecies))
		{
			auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

			double edibilityValue = 0.0;

			for(auto &elem : activeAnimals) {
				for (auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
				{
					Animal* edibleAnimal = (*activeAnimalsIt);
					if (animalWhoIsEvaluating->canEatEdible(edibleAnimal, ediblesHasTriedToPredate))
					{
						if (!animalWhoIsEvaluating->hasTriedToHunt(edibleAnimal, ediblesHasTriedToPredate))
						{
							if(animalWhoIsEvaluating->getSpecies()->getEdiblePreference(edibleAnimal->getSpecies()->getId(), animalWhoIsEvaluating->getInstar(), edibleAnimal->getInstar()) > preferenceThresholdForEvaluation){	
								edibilityValue += animalWhoIsEvaluating->calculateEdibilityValueWithMass(edibleAnimal, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
							}
						
						}
					}
				}
			}

			double maximumEdibilityValue = currentAnimalSpecies->getK_Value() / currentAnimalSpecies->getMaximumDryMassObserved();

			totalEdibilityValue += Math_Functions::linearInterpolate01(edibilityValue, maximumEdibilityValue);
		}

		if(animalWhoIsEvaluating->getSpecies() == currentAnimalSpecies)
		{
			auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

			double conspecificBiomass = 0.0;

			for(auto &elem : activeAnimals) {
				for (auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
				{
					Animal* conspecificAnimal = (*activeAnimalsIt);
					conspecificBiomass += conspecificAnimal->getInterpolatedDryMass();
				}
			}

			double maximumConspecificBiomass = currentAnimalSpecies->getK_Value() / currentAnimalSpecies->getMaximumDryMassObserved();

			totalConspecificBiomass += Math_Functions::linearInterpolate01(conspecificBiomass, maximumConspecificBiomass);
		}
	}

	const vector<ResourceSpecies *> * edibleResourceSpecies = animalWhoIsEvaluating->getSpecies()->getEdibleResourceSpecies();
	for (auto it = edibleResourceSpecies->begin(); it != edibleResourceSpecies->end(); it++)
	{
		ResourceSpecies* currentResourceSpecies = *it;
		Resource* resource = getResource(currentResourceSpecies);

		//arthros to force search for preferred food
		if(resource != NULL)
		{
			if(animalWhoIsEvaluating->canEatEdible(resource, ediblesHasTriedToPredate))
			{
				if(animalWhoIsEvaluating->getSpecies()->getEdiblePreference(resource->getSpecies()->getId(), animalWhoIsEvaluating->getInstar(), resource->getInstar()) > preferenceThresholdForEvaluation){	
					totalEdibilityValue += animalWhoIsEvaluating->calculateEdibilityValueWithMass(resource, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
				}
			}
		}
	}

	return make_tuple(totalEdibilityValue, totalPredatoryRiskEdibilityValue, totalConspecificBiomass);
}

unsigned int TerrainCell::getNumberOfMatureFemales(Animal* reproducingAnimal)
{
	vector<Animal*> * searchableAnimals = new vector<Animal*>();

	for(auto &lifeStage : {LifeStage::ACTIVE, LifeStage::SATIATED, LifeStage::HANDLING, LifeStage::DIAPAUSE}) {
		auto lifeStageAnimals = getAnimalsBySpecies(lifeStage, reproducingAnimal->getSpecies());
		
		for(auto &elem : lifeStageAnimals) {
			//Prepare the animals to be reproduced today
			searchableAnimals->insert(searchableAnimals->end(), elem.first, elem.second);
		}
	}

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

unsigned int TerrainCell::getNumberOfEdibleAnimals(Animal* animalWhoIsEvaluating, const list<Edible*> &ediblesHasTriedToPredate)
{
	unsigned int preysCounter = 0;

	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		if(animalWhoIsEvaluating->getSpecies()->canEatAnimalSpecies(currentAnimalSpecies))
		{
			auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

			for(auto &elem : activeAnimals) {
				for (auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
				{
					if (animalWhoIsEvaluating->canEatEdible((Edible*)(*activeAnimalsIt), ediblesHasTriedToPredate))
					{
						preysCounter++;
					}
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
			auto activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);

			for(auto &elem : activeAnimals) {
				for (auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
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
}

ostream& TerrainCell::printAnimalsVoracities(int timeStep, int timeStepsPerDay, ostream& os, SimulType simulType)
{
	//for (unsigned int i = 0; i < animals.size(); ++i)
	//{
		for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
		{
			AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
			//activeAnimals = getAnimalsBySpecies(i, currentAnimalSpecies);
			auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

			for(auto &elem : activeAnimals) {
				for (auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
				{
					(*activeAnimalsIt)->printVoracities(timeStep, timeStepsPerDay, os, simulType);
				}
			}
		}
	//}
	return os;
}

ostream& TerrainCell::printAnimals(ostream& os)
{
	for(const auto &lifeStage : LifeStage::getEnumValues())
	{
		for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
		{
			AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
			auto activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);

			for(auto &elem : activeAnimals) {
				for (auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
				{
					os << *(*activeAnimalsIt) << endl;
				}
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

	for (auto itResourceSpecies = existingResourceSpecies->begin(); itResourceSpecies != existingResourceSpecies->end(); itResourceSpecies++)
	{
		os << getResourceBiomass(*itResourceSpecies) << "\t";
	}

	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		int currentNumberOfAnimals = 0;
		for(const auto &lifeStage : LifeStage::getEnumValues())
		{
			AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
			auto activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);

			for(auto &elem : activeAnimals) {
				currentNumberOfAnimals += (elem.second - elem.first);
			}
		}
		os << currentNumberOfAnimals << "\t";
	}
	os << endl;
	return os;
}

void TerrainCell::finishCell()
{
	for(auto it = animals.begin(); it != animals.end(); it++)
	{
		map<AnimalSpecies* , vector<Animal*> *> * animalsInThisLifeStage = (*it).second;
		for(auto lifeStageIt = animalsInThisLifeStage->begin(); lifeStageIt != animalsInThisLifeStage->end(); lifeStageIt++)
		{
			vector<Animal*> * animalsInThisSpecies = (*lifeStageIt).second;
			for(auto &animal : *animalsInThisSpecies)
			{
				MacroTerrainCell* macroCell = dynamic_cast<MacroTerrainCell*>(animal->getPosition());

				if(macroCell != nullptr) {
					macroCell->finishCell(animal, this);
				}
			}
		}
	}
}

void TerrainCell::activateAndResumeAnimals(int timeStep, int timeStepsPerDay)
{
	#ifdef _DEBUG
	Output::coutFlush("what is happening: {}\n", theWorld->getExistingAnimalSpecies()->at(0)->getScientificName());
	#endif

	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		auto unbornAnimals = getAnimalsBySpecies(LifeStage::UNBORN, currentAnimalSpecies);
		auto pupaAnimals = getAnimalsBySpecies(LifeStage::PUPA, currentAnimalSpecies);
		auto handlingAnimals = getAnimalsBySpecies(LifeStage::HANDLING, currentAnimalSpecies);
		auto diapauseAnimals = getAnimalsBySpecies(LifeStage::DIAPAUSE, currentAnimalSpecies);

		#ifdef _DEBUG
		Output::cout("but am I actually doing anything?: unborn/active: {}/{} -> ", unbornAnimals->size(), activeAnimals->size());
		#endif

		for(auto &elem : diapauseAnimals) {
			for (auto diapauseAnimalsIt = elem.first; diapauseAnimalsIt != elem.second; diapauseAnimalsIt++)
			{
				(*diapauseAnimalsIt)->isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(moisture);
			}
		}

		for(auto &elem : unbornAnimals) {
			for (auto unbornAnimalsIt = elem.first; unbornAnimalsIt != elem.second; unbornAnimalsIt++)
			{
				(*unbornAnimalsIt)->isReadyToBeBorn(timeStep, timeStepsPerDay);
			}
		}

		for(auto &elem : pupaAnimals) {
			for (auto pupaAnimalsIt = elem.first; pupaAnimalsIt != elem.second; pupaAnimalsIt++)
			{
				(*pupaAnimalsIt)->isReadyToResumeFromPupaOrDecreasePupaTimer();
			}
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

		auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

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

vector<vector<Resource *>*> TerrainCell::getResources()
{
	return {&resources};
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
			auto activeAnimals = getAnimalsBySpecies(stage, currentAnimalSpecies);

			for(auto &elem : activeAnimals) {
				population += (elem.second - elem.first);
			}
		}
	}
	return population;
}

unsigned int TerrainCell::getLifeStagePopulation(const LifeStage &stage, AnimalSpecies* species)
{
	auto activeAnimals = getAnimalsBySpecies(stage, species);

	unsigned int population = 0;

	for(auto &elem : activeAnimals) {
		population += (elem.second - elem.first);
	}

	return population;
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
			auto activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);

			unsigned int population = 0;

			for(auto &elem : activeAnimals) {
				population += (elem.second - elem.first);
			}

			worldAnimalsPopulation->at(currentAnimalSpecies)->at(magic_enum::enum_integer(lifeStage)) += population;
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
			auto activeAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);

			worldAnimalsPopulation->at(currentAnimalSpecies)->at(magic_enum::enum_integer(lifeStage))++;

			for(auto &elem : activeAnimals) {
				for (auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
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

void TerrainCell::assimilateFoodMass(int timeStep)
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		auto activeAnimals = getAnimalsBySpecies(LifeStage::ACTIVE, currentAnimalSpecies);

		for(auto &elem : activeAnimals) {
			for (auto activeAnimalsIt = elem.first; activeAnimalsIt != elem.second; activeAnimalsIt++)
			{
				(*activeAnimalsIt)->assimilateFoodMass(timeStep);
			}
		}
	}
}

void TerrainCell::moveAnimals(int timeStep, int timeStepsPerDay, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, ostream& edibilitiesFile, float exitTimeThreshold, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
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
		currentAnimal->move(timeStep, timeStepsPerDay, encounterProbabilitiesFile, predationProbabilitiesFile, saveEdibilitiesFile, edibilitiesFile, worldDepth, worldLength, worldWidth, pdfThreshold, muForPDF, sigmaForPDF, existingAnimalSpecies, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);

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

		for(auto &lifeStage : {LifeStage::ACTIVE, LifeStage::SATIATED, LifeStage::HANDLING}) {
			auto lifeStageAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);
			
			for(auto &elem : lifeStageAnimals) {
				//Prepare the animals to be reproduced today
				animalsToMetabolizeAndGrowth->insert(animalsToMetabolizeAndGrowth->end(), elem.first, elem.second);
			}
		}
	}

	//cellsAtDistance.clear();
}

void TerrainCell::migrateAnimalTo(Animal* animalToMigrate, TerrainCellInterface* newPosition)
{
	eraseAnimal(animalToMigrate, animalToMigrate->getLifeStage());
	newPosition->addAnimal(animalToMigrate);
	animalToMigrate->setPosition(newPosition);
}

void TerrainCell::purgeDeadAnimals()
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;

		for(auto &lifeStage : {LifeStage::STARVED, LifeStage::PREDATED, LifeStage::BACKGROUND, LifeStage::SENESCED, LifeStage::SHOCKED}) {
			auto lifeStageAnimals = getAnimalsBySpecies(lifeStage, currentAnimalSpecies);
			
			for(auto &elem : lifeStageAnimals) {
				for (auto it = elem.first; it != elem.second; it++)
				{
					MacroTerrainCell* macroCell = dynamic_cast<MacroTerrainCell*>((*it)->getPosition());

					if(macroCell != nullptr) {
						macroCell->purgeDeadAnimals((*it), this);
					}

					delete (*it);
				}
			}

			animals.at(lifeStage)->at(currentAnimalSpecies)->clear();
		}
	} 
}

void TerrainCell::deleteExtinguishedReproducingAnimals(AnimalSpecies* extinguishedAnimalSpecies)
{
	for(auto animalSpeciesIt = existingAnimalSpecies->begin(); animalSpeciesIt != existingAnimalSpecies->end(); animalSpeciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *animalSpeciesIt;
		if(currentAnimalSpecies == extinguishedAnimalSpecies)
		{
			auto reproducingAnimals = getAnimalsBySpecies(LifeStage::REPRODUCING, currentAnimalSpecies);
			
			for(auto &elem : reproducingAnimals) {
				for (auto reproducingAnimalsIt = elem.first; reproducingAnimalsIt != elem.second; reproducingAnimalsIt++)
				{
					Animal* deletedAnimal = (*reproducingAnimalsIt);
					reproducingAnimalsIt = animals.at(LifeStage::REPRODUCING)->at(currentAnimalSpecies)->erase(reproducingAnimalsIt);
					
					MacroTerrainCell* macroCell = dynamic_cast<MacroTerrainCell*>(deletedAnimal->getPosition());

					if(macroCell != nullptr) {
						macroCell->eraseAnimal(deletedAnimal, deletedAnimal->getLifeStage(), this);
					}

					delete deletedAnimal;
				}
			}
		}
	}
}



MacroTerrainCell::MacroTerrainCell(unsigned int minX, unsigned int minY, unsigned int minZ, unsigned int size, WorldInterface* worldInterface)
	: TerrainCellInterface(worldInterface)
{
	area.resize(size);

	for(size_t i = 0; i < size; i++) {
		area[i].resize(size);

		for(size_t j = 0; j < size; j++) {
			area[i][j].resize(1);
		}
	}

	for(unsigned int i = 0; i < size; i++) {
		for(unsigned int j = 0; j < size; j++) {
			unsigned int x = minX+i, y = minY+j, z = 0;

			area[i][j][z] = worldInterface->getCell(z,y,x);
		}
	}
}

MacroTerrainCell::MacroTerrainCell(MacroTerrainCell &other)
	: TerrainCellInterface(other.worldInterface)
{
	area.resize(other.area.size());

	for(size_t i = 0; i < other.area.size(); i++) {
		area[i].resize(other.area[i].size());

		for(size_t j = 0; j < other.area[i].size(); j++) {
			area[i][j].resize(1);
		}
	}

	for(unsigned int x = 0; x < other.area.size(); x++) {
		for(unsigned int y = 0; y < other.area[x].size(); y++) {
			area[x][y][0] = other.area[x][y][0];
		}
	}
}

MacroTerrainCell::~MacroTerrainCell()
{

}

bool MacroTerrainCell::isInEnemyFreeSpace()
{
	bool inEnemyFreeSpace = true;

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			inEnemyFreeSpace = inEnemyFreeSpace && area[i][j][0]->isInEnemyFreeSpace();
		}
	}

	return inEnemyFreeSpace;
}

bool MacroTerrainCell::isInCompetitorFreeSpace() 
{
	bool inCompetitorFreeSpace = true;

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			inCompetitorFreeSpace = inCompetitorFreeSpace && area[i][j][0]->isInCompetitorFreeSpace();
		}
	}

	return inCompetitorFreeSpace;
}

float MacroTerrainCell::getTemperature()
{
	double temperature = 0.0;

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			temperature += area[i][j][0]->getTemperature();
		}
	}

	temperature /= (area.size()*2);

	return temperature;
}

void MacroTerrainCell::eraseAnimal(Animal* oldEdible, const LifeStage &oldStage)
{
	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			area[i][j][0]->eraseAnimal(oldEdible, oldStage);
		}
	}
}

void MacroTerrainCell::eraseAnimal(Animal* oldEdible, const LifeStage &oldStage, TerrainCellInterface* cell)
{
	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			if(area[i][j][0] != cell) {
				area[i][j][0]->eraseAnimal(oldEdible, oldStage);
			}
		}
	}
}

void MacroTerrainCell::addAnimal(Animal* newAnimal)
{
	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			area[i][j][0]->addAnimal(newAnimal);
		}
	}
}

unsigned int MacroTerrainCell::getNumberOfMatureFemales(Animal* reproducingAnimal)
{
	unsigned int counter = 0;

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			counter += area[i][j][0]->getNumberOfMatureFemales(reproducingAnimal);
		}
	}

	return counter;
}

tuple<double, double, double> MacroTerrainCell::getCellEvaluation(Animal* animalWhoIsEvaluating, const list<Edible*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	double totalPredatoryRiskEdibilityValue = 0.0;
	double totalEdibilityValue = 0.0;
	double totalConspecificBiomass = 0.0;

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			double edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass;
			tie(edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass) = area[i][j][0]->getCellEvaluation(animalWhoIsEvaluating, ediblesHasTriedToPredate, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
		
			totalEdibilityValue += edibilityValue;
			totalPredatoryRiskEdibilityValue += predatoryRiskEdibilityValue;
			totalConspecificBiomass += conspecificBiomass;
		}
	}

	totalEdibilityValue /= (area.size()*2);
	totalPredatoryRiskEdibilityValue /= (area.size()*2);
	totalConspecificBiomass /= (area.size()*2);

	return make_tuple(totalEdibilityValue, totalPredatoryRiskEdibilityValue, totalConspecificBiomass);
}

void MacroTerrainCell::changeAnimalToSenesced(Animal* targetAnimal, int day)
{
	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			area[i][j][0]->changeAnimalToSenesced(targetAnimal, day);
		}
	}
}

const vector<float> MacroTerrainCell::getTemperatureCycle() const
{
	vector<float> temperatureCycle(area[0][0][0]->getTemperatureCycle().size(), 0.0);

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			std::transform(temperatureCycle.begin(), temperatureCycle.end(), area[i][j][0]->getTemperatureCycle().begin(), temperatureCycle.begin(), std::plus<float>());
		}
	}

	return temperatureCycle;
}

float MacroTerrainCell::getSize()
{
	return area.size()*area[0][0][0]->getSize();
}

vector<std::pair<std::vector<Animal *>::iterator, std::vector<Animal *>::iterator>> MacroTerrainCell::getAnimalsBySpecies(const LifeStage &lifeStage, AnimalSpecies* species)
{
	vector<pair<vector<Animal *>::iterator, vector<Animal *>::iterator>> animals;

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			auto cellAnimals = area[i][j][0]->getAnimalsBySpecies(lifeStage, species);
			animals.insert(animals.end(), cellAnimals.begin(), cellAnimals.end());
		}
	}

	return animals;
}

void MacroTerrainCell::purgeDeadAnimals(Animal* animal, TerrainCell* cell)
{
	eraseAnimal(animal, animal->getLifeStage(), cell);
}

vector<vector<Resource *>*> MacroTerrainCell::getResources()
{
	vector<vector<Resource *>*> resources;

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			auto resourcesCell = area[i][j][0]->getResources();

			resources.insert(resources.end(), resourcesCell.begin(), resourcesCell.end());
		}
	}

	return resources;
}

std::vector<TerrainCellInterface *> * MacroTerrainCell::getCellsAtDistance(int distance, double searchArea, bool isMobile, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth)
{
	MacroTerrainCell* cellToBeAdded;

	std::vector<TerrainCellInterface *> * newNeighbors = new std::vector<TerrainCellInterface *>();

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
	
	for(int z = -distance ; z <= distance; z++) {
		for(int y = -distance ; y <= distance; y++) {
			for(int x = -distance ; x <= distance; x++) {
				if(x != 0 || y != 0 || z != 0) {
					int minX = area[0][0][0]->getX() + x*getSize();
					minX = max(minX, 0);
					minX = min(minX, (int)worldWidth-(int)getSize()-1);

					int minY = area[0][0][0]->getY() + y*getSize();
					minY = max(minY, 0);
					minY = min(minY, (int)worldLength-(int)getSize()-1);

					int minZ = 0;

					cellToBeAdded = new MacroTerrainCell(minX, minY, minZ, getSize(), worldInterface);

					if(!cellToBeAdded->isObstacle()) {
						double distanceToTheCellToBeAdded = getDistanceToCell(cellToBeAdded);

						if(distanceToTheCellToBeAdded <= searchArea)
						{
							newNeighbors->push_back(cellToBeAdded);
						}
						else {
							delete cellToBeAdded;
						}
					}
					else {
						delete cellToBeAdded;
					}
				}
			}
		}
	}

	return newNeighbors;
}

bool MacroTerrainCell::isObstacle() const {
	unsigned int obstableCount = 0;

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			if(area[i][j][0]->isObstacle()) {
				obstableCount++;
			}
		}
	}

	if(static_cast<double>(obstableCount)/(area.size()*2) <= 0.3) {
		return false;
	}
	else {
		return true;
	}
}

double MacroTerrainCell::getDistanceToCell(TerrainCellInterface* targetCell)
{
	return sqrt(pow(abs(getX() - targetCell->getX()) * getSize(), 2) + pow(abs(getY() - targetCell->getY()) * getSize(), 2) + pow(abs(getZ() - targetCell->getZ()) * getSize(), 2));
}

int MacroTerrainCell::getX() const
{
	return area[0][0][0]->getX();
}

int MacroTerrainCell::getY() const
{
	return area[0][0][0]->getY();
}

int MacroTerrainCell::getZ() const
{
	return area[0][0][0]->getZ();
}

void MacroTerrainCell::migrateAnimalTo(Animal* animalToMigrate, TerrainCellInterface* newPosition)
{
	eraseAnimal(animalToMigrate, animalToMigrate->getLifeStage());
	newPosition->addAnimal(animalToMigrate);
	delete reinterpret_cast<MacroTerrainCell*>(animalToMigrate->getPosition());
	animalToMigrate->setPosition(newPosition);
}

double MacroTerrainCell::turnEdibleIntoDryMassToBeEaten(Edible* targetEdible, int day, Animal* predatorEdible, double leftovers)
{
	#ifdef _DEBUG
	Output::coutFlush("{} predated {}\n", predatorId, targetEdible->getId());
	#endif

	float targetEdibleProfitability = predatorEdible->getSpecies()->getEdibleProfitability(targetEdible->getSpecies()->getId(), predatorEdible->getInstar(), targetEdible->getInstar());

	//double dryMassToBeEaten = 0;

	double dryMassToBeEaten = targetEdible->turnIntoDryMassToBeEaten(predatorEdible->getRemainingVoracity(), targetEdibleProfitability + predatorEdible->getAssim(), leftovers);
	

	if(targetEdible->getSpecies()->isMobile())
	{
		targetEdible->setNewLifeStage(LifeStage::PREDATED, day, predatorEdible->getId());

		for(unsigned int i = 0; i < area.size(); i++) {
			for(unsigned int j = 0; j < area[i].size(); j++) {
				auto animalsToMove = area[i][j][0]->getAnimalsToMove();

				animalsToMove->erase(std::remove(animalsToMove->begin(), animalsToMove->end(), targetEdible), animalsToMove->end());
			}
		}
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

Resource* MacroTerrainCell::getResource(ResourceSpecies* species)
{
	Resource * resource;

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			resource = area[i][j][0]->getResource(species);

			if(resource != NULL) {
				return resource;
			}
		}
	}

	return NULL;
}

double MacroTerrainCell::getTotalResourceBiomass()
{
	double mass = 0;

	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			mass += area[i][j][0]->getTotalResourceBiomass();
		}
	}

	return mass;
}

void MacroTerrainCell::finishCell(Animal* animal, TerrainCell* cell)
{
	for(unsigned int i = 0; i < area.size(); i++) {
		for(unsigned int j = 0; j < area[i].size(); j++) {
			if(area[i][j][0] != cell) {
				area[i][j][0]->eraseAnimal(animal, animal->getLifeStage());
			}
		}
	}
}

bool operator==(const MacroTerrainCell& lhs, const MacroTerrainCell& rhs)
{
	if(lhs.area.size() == rhs.area.size()) {
		if(lhs.area[0][0][0]->getX() == rhs.area[0][0][0]->getX() &&
			lhs.area[0][0][0]->getY() == rhs.area[0][0][0]->getY() &&
			lhs.area[0][0][0]->getZ() == rhs.area[0][0][0]->getZ()) {
			return true;
		}
	}

	return false;
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
