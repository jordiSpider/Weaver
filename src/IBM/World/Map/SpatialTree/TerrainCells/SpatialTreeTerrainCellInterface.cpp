

#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCellInterface.h"
#include "IBM/World/Map/SpatialTree.h"

using namespace std;



SpatialTreeTerrainCellInterface::SpatialTreeTerrainCellInterface(PointSpatialTree* const &position, const Ring *const effectiveArea, const double &size, SpatialTree* const &map,
        LifeStageVector* const animals, const bool obstacle, const bool fullObstacle, 
        const int obstaclePatchPriority, MoistureInterface* const moistureInfo, const bool moistureSource, 
        const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity)
    : TerrainCell(position, effectiveArea, size, map, animals,
        obstacle, fullObstacle, obstaclePatchPriority, moistureInfo, moistureSource, inMoisturePatch, moisturePatchPriority,
        totalMaximumResourceCapacity)
{
    
}


SpatialTreeTerrainCellInterface::~SpatialTreeTerrainCellInterface()
{
    
}

const PointSpatialTree& SpatialTreeTerrainCellInterface::getPosition() const
{
    return static_cast<const PointSpatialTree&>(TerrainCell::getPosition());
}

PointSpatialTree& SpatialTreeTerrainCellInterface::getMutablePosition() const
{
    return static_cast<PointSpatialTree&>(TerrainCell::getMutablePosition());
}
