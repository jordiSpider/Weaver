
#include "World/Map/TerrainCells/Moisture/MoistureInterface.h"

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
