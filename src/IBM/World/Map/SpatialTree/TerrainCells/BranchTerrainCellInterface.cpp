

#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCellInterface.h"

using namespace std;



BranchTerrainCellInterface::BranchTerrainCellInterface(BranchTerrainCellInterface* const parentTerrainCell, 
        PointSpatialTree* const position, const Ring *const effectiveArea, const double &size, SpatialTreeInterface* const mapInterface,
        LifeStageVector* const animals, 
        const bool obstacle, const bool fullObstacle, const int obstaclePatchPriority, MoistureInterface* const moistureInfo, 
        const bool moistureSource, const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity)
    : SpatialTreeTerrainCell(parentTerrainCell, position, effectiveArea, size, mapInterface, animals,
        obstacle, fullObstacle, obstaclePatchPriority, moistureInfo, moistureSource, inMoisturePatch, moisturePatchPriority, totalMaximumResourceCapacity)
{
    
}

BranchTerrainCellInterface::~BranchTerrainCellInterface()
{
    
}
