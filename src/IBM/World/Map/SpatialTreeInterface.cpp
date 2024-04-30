
#include "IBM/World/Map/SpatialTreeInterface.h"

using namespace std;
using json = nlohmann::json;



SpatialTreeInterface::SpatialTreeInterface(const json &mapConfig, WorldInterface* const worldInterface)
    : Map(mapConfig, worldInterface)
{
    
}

SpatialTreeInterface::SpatialTreeInterface(WorldInterface* const worldInterface)
    : Map(worldInterface)
{

}

SpatialTreeInterface::~SpatialTreeInterface()
{
    
}

template <class Archive>
void SpatialTreeInterface::serialize(Archive &ar, const unsigned int version) {
    ar & boost::serialization::base_object<Map>(*this);
}


// Specialisation
template void SpatialTreeInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SpatialTreeInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SpatialTreeInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SpatialTreeInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
