#ifndef LEAF_TERRAINCELL_INTERFACE_H_
#define LEAF_TERRAINCELL_INTERFACE_H_


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"



class BranchTerrainCell;



class LeafTerrainCellInterface : public SpatialTreeTerrainCell
{
public:
    LeafTerrainCellInterface(BranchTerrainCellInterface* const parentTerrainCell, SpatialTreeInterface* const mapInterface);
    LeafTerrainCellInterface(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, const double &size, 
        SpatialTreeInterface* const mapInterface, LifeStageVector* const animals, 
        const bool obstacle, const bool fullObstacle, 
        const int obstaclePatchPriority, MoistureInterface* const moistureInfo, const bool moistureSource, 
        const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity);
    virtual ~LeafTerrainCellInterface();

    virtual const Type getSpatialTreeTerrainCellType() const;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version, std::vector<ExtendedMoisture*>& appliedMoisture);
};

#endif /* LEAF_TERRAINCELL_INTERFACE_H_ */
