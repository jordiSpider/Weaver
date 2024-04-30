#include "IBM/World/LivingBeings/EdibleInterface.h"

using namespace std;



EdibleInterface::EdibleInterface() 
{
	
}

EdibleInterface::~EdibleInterface()
{

}

template <class Archive>
void EdibleInterface::serialize(Archive &ar, const unsigned int version) 
{
    
}

// Specialisation
template void EdibleInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void EdibleInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void EdibleInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void EdibleInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
