#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Obstacle/CellObstacleInterface.h"


using namespace std;





CellObstacleInterface::CellObstacleInterface()
    : CellElement()
{

}

CellObstacleInterface::CellObstacleInterface(const size_t newObstaclePatchPriority)
    : CellElement(newObstaclePatchPriority), obstacle(false), fullObstacle(false)
{

}

CellObstacleInterface::~CellObstacleInterface()
{

}

void CellObstacleInterface::applySource(const size_t obstaclePatchPriority)
{
    setPatchPriority(obstaclePatchPriority);
    setFullObstacleValue(true);
    setObstacleValue(true);
}

bool CellObstacleInterface::isFullObstacle() const
{
    return fullObstacle;
}

bool CellObstacleInterface::isObstacle() const {
    return obstacle;
}

void CellObstacleInterface::setObstacleValue(const bool newValue)
{
    obstacle = newValue;
}

void CellObstacleInterface::setFullObstacleValue(const bool newValue)
{
    fullObstacle = newValue;
}




BOOST_SERIALIZATION_ASSUME_ABSTRACT(CellObstacleInterface)

template <class Archive>
void CellObstacleInterface::serialize(Archive& ar, const unsigned int) 
{
    ar & boost::serialization::base_object<CellElement>(*this);

    ar & obstacle;
    ar & fullObstacle;
}

// Specialisation
template void CellObstacleInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellObstacleInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellObstacleInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellObstacleInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
