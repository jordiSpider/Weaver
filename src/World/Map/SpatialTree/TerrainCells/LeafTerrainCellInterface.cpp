

#include "World/Map/SpatialTree/TerrainCells/LeafTerrainCellInterface.h"

using namespace std;



LeafTerrainCellInterface::LeafTerrainCellInterface(BranchTerrainCellInterface* const parentTerrainCell, 
        PointSpatialTree* const position, const double &size, SpatialTreeInterface* const mapInterface,
        LifeStageVector* const animals, 
        const bool obstacle, const bool fullObstacle, const int obstaclePatchPriority, MoistureInterface* const moistureInfo, 
        const bool moistureSource, const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity)
    : SpatialTreeTerrainCell(parentTerrainCell, position, makeEffectiveArea(position, size), size, mapInterface, animals,
        obstacle, fullObstacle, obstaclePatchPriority, moistureInfo, moistureSource, inMoisturePatch, moisturePatchPriority, totalMaximumResourceCapacity)
{
    
}

LeafTerrainCellInterface::~LeafTerrainCellInterface()
{
    
}
