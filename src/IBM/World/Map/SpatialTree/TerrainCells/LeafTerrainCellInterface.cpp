

#include "IBM/World/Map/SpatialTree/TerrainCells/LeafTerrainCellInterface.h"

using namespace std;



LeafTerrainCellInterface::LeafTerrainCellInterface(BranchTerrainCellInterface* const parentTerrainCell, SpatialTreeInterface* const mapInterface)
    : SpatialTreeTerrainCell(parentTerrainCell, mapInterface)
{

}

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

const SpatialTreeTerrainCellInterface::Type LeafTerrainCellInterface::getSpatialTreeTerrainCellType() const
{
    return SpatialTreeTerrainCellInterface::Type::Leaf;
}

template <class Archive>
void LeafTerrainCellInterface::serialize(Archive &ar, const unsigned int version, vector<ExtendedMoisture*>& appliedMoisture) 
{
    boost::serialization::base_object<SpatialTreeTerrainCell>(*this).serialize(ar, version, appliedMoisture);
}

// Specialisation
template void LeafTerrainCellInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int, vector<ExtendedMoisture*>&);
template void LeafTerrainCellInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int, vector<ExtendedMoisture*>&);

template void LeafTerrainCellInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int, vector<ExtendedMoisture*>&);
template void LeafTerrainCellInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int, vector<ExtendedMoisture*>&);
