#ifndef MAP_H_
#define MAP_H_

#include <nlohmann/json.hpp>
#include <boost/geometry/algorithms/convert.hpp>
#include <fstream>

#include "Misc/Types.h"
#include "World/Map/MapInterface.h"
#include "World/Map/Patches/Moisture/MoistureBasePatch.h"
#include "World/LivingBeings/LifeStage.h"


class WorldInterface;
class AnimalSearchParams;


class Map : public MapInterface {
protected:
    const double minCellSize;
    const unsigned int numberOfCellsPerAxis;
    WorldInterface* const worldInterface;
    std::vector<TerrainCellInterface*> inhabitableTerrainCells;
    std::vector<AnimalSearchParams> lifeStageSearchParams;

    bool isSpeciesInhabitableTerrainCell(
        const AnimalSpecies &animalSpecies, const TerrainCellInterface* const potentialInhabitableTerrainCell, 
        const Instar &instar
    );
    void addInhabitableTerrainCell(TerrainCellInterface* newInhabitableTerrainCell);
    virtual void obtainMapCellsInfo(std::vector<std::pair<std::vector<double>, std::vector<unsigned int>>> &mapCellsInfo)=0;

public:
    Map(const nlohmann::json &mapConfig, WorldInterface* const worldInterface);
    virtual ~Map();

    const double& getMinCellSize() const;
    const unsigned int& getNumberOfCellsPerAxis() const;
    const WorldInterface* const getWorldInterface() const;
    void printCellAlongCells(std::ofstream &file);
    WorldInterface* const getMutableWorldInterface() const;
    const std::vector<TerrainCellInterface*>& getInhabitableTerrainCells() const;
    std::vector<TerrainCellInterface*>& getMutableInhabitableTerrainCells();
    TerrainCellInterface*& getMutableInhabitableTerrainCell(const unsigned int index);
    void obtainLifeStageSearchParams();
    const AnimalSearchParams& getLifeStageSearchParams(const LifeStage::LifeStageValue &lifeStage) const;
};

#endif /* MAP_H_ */