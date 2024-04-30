#ifndef LEAF_TERRAINCELL_INTERFACE_H_
#define LEAF_TERRAINCELL_INTERFACE_H_


#include "World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"



class BranchTerrainCell;



class LeafTerrainCellInterface : public SpatialTreeTerrainCell
{
public:
    LeafTerrainCellInterface(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, const double &size, 
        SpatialTreeInterface* const mapInterface, LifeStageVector* const animals, 
        const bool obstacle, const bool fullObstacle, 
        const int obstaclePatchPriority, MoistureInterface* const moistureInfo, const bool moistureSource, 
        const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity);
    virtual ~LeafTerrainCellInterface();
};

#endif /* LEAF_TERRAINCELL_INTERFACE_H_ */