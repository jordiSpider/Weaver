

#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCellInterface.h"

using namespace std;



BranchTerrainCellInterface::BranchTerrainCellInterface(BranchTerrainCellInterface* const parentTerrainCell, SpatialTreeInterface* const mapInterface)
    : SpatialTreeTerrainCell(parentTerrainCell, mapInterface)
{

}

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

const SpatialTreeTerrainCellInterface::Type BranchTerrainCellInterface::getSpatialTreeTerrainCellType() const
{
    return SpatialTreeTerrainCellInterface::Type::Branch;
}

template <class Archive>
void BranchTerrainCellInterface::serialize(Archive &ar, const unsigned int version, vector<ExtendedMoisture*>& appliedMoisture) 
{
    boost::serialization::base_object<SpatialTreeTerrainCell>(*this).serialize(ar, version, appliedMoisture);
}

// Specialisation
template void BranchTerrainCellInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int, vector<ExtendedMoisture*>&);
template void BranchTerrainCellInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int, vector<ExtendedMoisture*>&);

template void BranchTerrainCellInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int, vector<ExtendedMoisture*>&);
template void BranchTerrainCellInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int, vector<ExtendedMoisture*>&);
