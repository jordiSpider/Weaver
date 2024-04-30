
#include "World/Map/TerrainCells/Moisture/NullMoisture.h"

using namespace std;


NullMoisture::NullMoisture()
    : MoistureInterface(), 
      inEnemyFreeSpace(false), inCompetitorFreeSpace(false), temperature(0.0), moisture(0.0)
{

}

NullMoisture::~NullMoisture()
{

}

void NullMoisture::updateTemperature()
{
    throwLineInfoException("Null moisture is not usable");
}

void NullMoisture::updateRelativeHumidity()
{
    throwLineInfoException("Null moisture is not usable");
}

void NullMoisture::updateInfo()
{
    throwLineInfoException("Null moisture is not usable");
}

const Temperature& NullMoisture::getTemperature() const
{
    return temperature;
}

const double& NullMoisture::getMoisture() const
{
    return moisture;
}

const bool NullMoisture::isInEnemyFreeSpace() const
{
    return inEnemyFreeSpace;
}

const bool NullMoisture::isInCompetitorFreeSpace() const
{
    return inCompetitorFreeSpace;
}

const double& NullMoisture::getMaximumResourceCapacityDensity() const
{
    throwLineInfoException("Null moisture is not usable");
}

const Temperature& NullMoisture::getRandomTemperature() const
{
    throwLineInfoException("Null moisture is not usable");
}
