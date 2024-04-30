
#include "IBM/World/Map/SpatialTree/TerrainCells/Moisture/SubdivisionMoisture.h"

using namespace std;


SubdivisionMoisture::SubdivisionMoisture(ExtendedMoisture* const moistureSource)
    : MoistureInterface(),
      moistureSource(moistureSource)
{

}

SubdivisionMoisture::~SubdivisionMoisture()
{

}

void SubdivisionMoisture::updateTemperature()
{
    // Not to implement
}

void SubdivisionMoisture::updateRelativeHumidity()
{
    // Not to implement
}

void SubdivisionMoisture::updateInfo()
{
    throwLineInfoException("Subdivision moisture is not updated");
}

const double& SubdivisionMoisture::getMaximumResourceCapacityDensity() const
{
    return getMoistureSource()->getMaximumResourceCapacityDensity();
}

const Temperature& SubdivisionMoisture::getTemperature() const
{
    return getMoistureSource()->getTemperature();
}

const double& SubdivisionMoisture::getMoisture() const
{
    return getMoistureSource()->getMoisture();
}

const bool SubdivisionMoisture::isInEnemyFreeSpace() const
{
    return getMoistureSource()->isInEnemyFreeSpace();
}

const bool SubdivisionMoisture::isInCompetitorFreeSpace() const
{
    return getMoistureSource()->isInCompetitorFreeSpace();
}

const ExtendedMoisture* const SubdivisionMoisture::getMoistureSource() const
{
    return moistureSource;
}

ExtendedMoisture* const SubdivisionMoisture::getMutableMoistureSource() const
{
    return moistureSource;
}

const Temperature& SubdivisionMoisture::getRandomTemperature() const
{
    return getMoistureSource()->getRandomTemperature();
}
