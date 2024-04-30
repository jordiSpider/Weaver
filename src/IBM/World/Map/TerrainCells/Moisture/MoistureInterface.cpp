
#include "IBM/World/Map/TerrainCells/Moisture/MoistureInterface.h"

using namespace std;


MoistureInterface::MoistureInterface()
{

}

MoistureInterface::~MoistureInterface()
{

}

void MoistureInterface::update()
{
    updateTemperature();
    updateRelativeHumidity();
}

template <class Archive>
void MoistureInterface::serialize(Archive &ar, const unsigned int version) 
{

}
