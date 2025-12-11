#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/SpatialTreePatchApplicator.h"


#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"


using namespace std;



SpatialTreePatchApplicator::SpatialTreePatchApplicator()
{

}

SpatialTreePatchApplicator::SpatialTreePatchApplicator(
        TerrainCell* cell, CellMoistureInterface* cellMoisture, CellObstacleInterface* cellObstacle, 
        CellHabitatDomainInterface* cellHabitatDomain
    )
    : PatchApplicator(cell, cellMoisture, cellObstacle, cellHabitatDomain)
{

}


SpatialTreePatchApplicator::~SpatialTreePatchApplicator()
{
    
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(SpatialTreePatchApplicator)

template <class Archive>
void SpatialTreePatchApplicator::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<PatchApplicator>(*this);
}


// // Specialisation
template void SpatialTreePatchApplicator::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SpatialTreePatchApplicator::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SpatialTreePatchApplicator::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SpatialTreePatchApplicator::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
