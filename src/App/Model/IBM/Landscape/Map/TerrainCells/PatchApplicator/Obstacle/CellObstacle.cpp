
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Obstacle/CellObstacle.h"


using namespace std;




CellObstacle::CellObstacle()
    : CellObstacleInterface() 
{

}

CellObstacle::CellObstacle(const size_t newObstaclePatchPriority)
    : CellObstacleInterface(newObstaclePatchPriority) 
{

}

CellObstacle::~CellObstacle()
{
    
}



BOOST_CLASS_EXPORT(CellObstacle)

template <class Archive>
void CellObstacle::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<CellObstacleInterface>(*this);
}

// Specialisation
template void CellObstacle::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellObstacle::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellObstacle::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellObstacle::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
