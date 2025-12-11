
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Obstacle/BranchCellObstacle.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"

using namespace std;




bool BranchCellObstacle::calculateFullObstacleValue(BranchTerrainCell* const summaryTerrainCell)
{
    size_t numberOfObstacles = 0u;

    for(const auto &child : summaryTerrainCell->getChildrenTerrainCells())
    {
        numberOfObstacles += (child->getPatchApplicator().getCellObstacle().isObstacle()) ? 1u : 0u;
    }

    return numberOfObstacles == summaryTerrainCell->getChildrenTerrainCells().size();
}

bool BranchCellObstacle::calculateObstacleValue(BranchTerrainCell* const summaryTerrainCell)
{
    size_t numberOfObstacles = 0u;

    for(const auto &child : summaryTerrainCell->getChildrenTerrainCells())
    {
        numberOfObstacles += (child->getPatchApplicator().getCellObstacle().isObstacle()) ? 1u : 0u;
    }

    return PreciseDouble(static_cast<double>(numberOfObstacles))/PreciseDouble(static_cast<double>(summaryTerrainCell->getChildrenTerrainCells().size())) > 0.5;
}

size_t BranchCellObstacle::calculateObstaclePatchPriority(BranchTerrainCell* const summaryTerrainCell)
{
    size_t obstaclePatchPriority = 0u;

    for(const auto &child : summaryTerrainCell->getChildrenTerrainCells())
    {
        if(obstaclePatchPriority < child->getPatchApplicator().getCellObstacle().getPatchPriority())
        {
            obstaclePatchPriority = child->getPatchApplicator().getCellObstacle().getPatchPriority();
        }
    }

    return obstaclePatchPriority;
}



BranchCellObstacle::BranchCellObstacle()
    : CellObstacleInterface()
{
    
}


BranchCellObstacle::BranchCellObstacle(const size_t newObstaclePatchPriority)
    : CellObstacleInterface(newObstaclePatchPriority)
{

}


BranchCellObstacle::~BranchCellObstacle()
{

}


void BranchCellObstacle::applyBranch(BranchTerrainCell* const newSummaryTerrainCell)
{
    setPatchPriority(calculateObstaclePatchPriority(newSummaryTerrainCell));
    setFullObstacleValue(calculateFullObstacleValue(newSummaryTerrainCell));
    setObstacleValue(calculateObstacleValue(newSummaryTerrainCell));
}




BOOST_CLASS_EXPORT(BranchCellObstacle)

template <class Archive>
void BranchCellObstacle::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<CellObstacleInterface>(*this);
}

// Specialisation
template void BranchCellObstacle::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void BranchCellObstacle::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void BranchCellObstacle::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void BranchCellObstacle::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
