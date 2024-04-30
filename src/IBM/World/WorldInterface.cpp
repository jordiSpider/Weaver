#include "IBM/World/WorldInterface.h"

using namespace std;




WorldInterface::WorldInterface()
{

}


WorldInterface::~WorldInterface()
{

}

const bool WorldInterface::isDinosaurs() const
{
    return false;
}

template <class Archive>
void WorldInterface::serialize(Archive &ar, const unsigned int version) 
{

}
