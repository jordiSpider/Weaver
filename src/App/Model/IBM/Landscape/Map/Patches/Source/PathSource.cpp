
#include "App/Model/IBM/Landscape/Map/Patches/Source/PathSource.h"

using namespace std;


PathSource::PathSource()
{

}

PathSource::~PathSource()
{
    
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(PathSource)

template<class Archive>
void PathSource::serialize(Archive &, const unsigned int) {
    
}

// Specialisation
template void PathSource::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PathSource::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PathSource::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PathSource::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);