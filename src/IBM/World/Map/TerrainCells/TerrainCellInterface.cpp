
#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"

using namespace std;



TerrainCellInterface::TerrainCellInterface()
{
    
}


TerrainCellInterface::~TerrainCellInterface()
{
    
}

template <class Archive>
void TerrainCellInterface::serialize(Archive &ar, const unsigned int version) 
{

}

// Specialisation
template void TerrainCellInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void TerrainCellInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void TerrainCellInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void TerrainCellInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
