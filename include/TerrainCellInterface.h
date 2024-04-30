#ifndef TERRAIN_CELL_INTERFACE_HPP_
#define TERRAIN_CELL_INTERFACE_HPP_

#include "WorldInterface.h"
#include "Edible.h"
#include <tuple>
#include <vector>
#include <utility>
#include "Animal.h"
#include "Resource.h"

class TerrainCellInterface
{
protected:
	WorldInterface* worldInterface;

public:
	TerrainCellInterface(WorldInterface* worldInterface);
	~TerrainCellInterface();

	virtual bool isInEnemyFreeSpace()=0;
	virtual bool isInCompetitorFreeSpace()=0;
	virtual float getTemperature()=0;
	virtual void eraseAnimal(Animal* oldEdible, const LifeStage &oldStage)=0;
	virtual void addAnimal(Animal* newAnimal)=0;
	virtual unsigned int getNumberOfMatureFemales(Animal* reproducingAnimal)=0;
	virtual std::tuple<double, double, double> getCellEvaluation(Animal* animalWhoIsEvaluating, const std::list<Edible*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)=0;
	virtual void changeAnimalToSenesced(Animal* targetAnimal, int day)=0;
	virtual const std::vector<float> getTemperatureCycle() const=0;
	virtual float getSize()=0;
	virtual std::vector<std::pair<std::vector<Animal *>::iterator, std::vector<Animal *>::iterator>> getAnimalsBySpecies(const LifeStage &lifeStage, AnimalSpecies* species)=0;
	virtual std::vector<std::vector<Resource *>*> getResources()=0;
	virtual std::vector<TerrainCellInterface *> * getCellsAtDistance(int distance, double searchArea, bool isMobile, unsigned int worldDepth, unsigned int worldLength, unsigned int worldWidth)=0;
	virtual bool isObstacle() const=0;
	virtual double getDistanceToCell(TerrainCellInterface* targetCell)=0;
	virtual int getX() const=0;
    virtual int getY() const=0;
    virtual int getZ() const=0;
	virtual void migrateAnimalTo(Animal* animalToMigrate, TerrainCellInterface* newPosition)=0;
	virtual double turnEdibleIntoDryMassToBeEaten(Edible* targetAnimal, int day, Animal* predatorEdible, double leftovers)=0;
	virtual Resource* getResource(ResourceSpecies* species)=0;
	virtual double getTotalResourceBiomass()=0;
};

struct TerrainCellEvaluation {
    TerrainCellInterface* cell;
    double value;

    TerrainCellEvaluation();
    TerrainCellEvaluation(AnimalSpecies* animalSpecies, TerrainCellInterface* cell, const double &edibilityValue, const double &predatoryRiskEdibilityValue, const double &conspecificBiomass);

	bool operator==(const TerrainCellEvaluation& other) const;
    bool operator<(const TerrainCellEvaluation& other) const;
    bool operator>(const TerrainCellEvaluation& other) const;
};



#endif // TERRAIN_CELL_INTERFACE_HPP_
