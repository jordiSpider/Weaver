
#include "World/Map/SpatialTree/TerrainCells/Moisture/SummaryMoisture.h"
#include "World/Map/SpatialTree/TerrainCells/BranchTerrainCellInterface.h"

using namespace std;



SummaryMoisture::SummaryMoisture(BranchTerrainCellInterface* const summaryTerrainCell)
    : MoistureInterface(),
      summaryTerrainCell(summaryTerrainCell),
      temperature(0.0), moisture(0.0)
{
    
}


SummaryMoisture::~SummaryMoisture()
{

}


void SummaryMoisture::updateTemperature()
{
    Temperature sumTemperature;

    for(const auto &child : summaryTerrainCell->getChildrenTerrainCells())
    {
        sumTemperature += child->getMoistureInfo()->getTemperature();
    }

    sumTemperature.setTemperatureCelsius(sumTemperature.getTemperatureCelsius() / static_cast<double>(SpatialTreeTerrainCell::numberOfChildren));
    setTemperature(sumTemperature);
}

void SummaryMoisture::updateInEnemyFreeSpace()
{
    unsigned int sumInEnemyFreeSpace = accumulate(
        summaryTerrainCell->getChildrenTerrainCells().cbegin(), summaryTerrainCell->getChildrenTerrainCells().cend(), 0,
        [](double accumulated, const auto &child) {
            return accumulated + (child->getMoistureInfo()->isInEnemyFreeSpace()) ? 1 : 0;
        }
    );

    setInEnemyFreeSpace(static_cast<double>(sumInEnemyFreeSpace) / static_cast<double>(SpatialTreeTerrainCell::numberOfChildren) >= 0.5);
}

void SummaryMoisture::updateInCompetitorFreeSpace()
{
    unsigned int sumInCompetitorFreeSpace = accumulate(
        summaryTerrainCell->getChildrenTerrainCells().cbegin(), summaryTerrainCell->getChildrenTerrainCells().cend(), 0,
        [](double accumulated, const auto &child) {
            return accumulated + (child->getMoistureInfo()->isInCompetitorFreeSpace()) ? 1 : 0;
        }
    );

    setInCompetitorFreeSpace(static_cast<double>(sumInCompetitorFreeSpace) / static_cast<double>(SpatialTreeTerrainCell::numberOfChildren) >= 0.5);
}

void SummaryMoisture::updateInfo()
{
    updateInEnemyFreeSpace();
    updateInCompetitorFreeSpace();
}

void SummaryMoisture::setTemperature(const Temperature &newTemperature)
{
    temperature = newTemperature;
}

void SummaryMoisture::setMoisture(const double &newMoisture)
{
    moisture = newMoisture;
}

void SummaryMoisture::setInEnemyFreeSpace(const bool newInEnemyFreeSpace)
{
    inEnemyFreeSpace = newInEnemyFreeSpace;
}

void SummaryMoisture::setInCompetitorFreeSpace(const bool newInCompetitorFreeSpace)
{
    inCompetitorFreeSpace = newInCompetitorFreeSpace;
}

const Temperature& SummaryMoisture::getTemperature() const
{
    return temperature;
}

const double& SummaryMoisture::getMoisture() const
{
    return moisture;
}

const bool SummaryMoisture::isInEnemyFreeSpace() const
{
    return inEnemyFreeSpace;
}

const bool SummaryMoisture::isInCompetitorFreeSpace() const
{
    return inCompetitorFreeSpace;
}

const double& SummaryMoisture::getMaximumResourceCapacityDensity() const
{
    throwLineInfoException("Summary moisture has not implemented this method");
}

void SummaryMoisture::updateRelativeHumidity()
{
    double sumMoisture = accumulate(
        summaryTerrainCell->getChildrenTerrainCells().cbegin(), summaryTerrainCell->getChildrenTerrainCells().cend(), 0.0,
        [](double accumulated, const auto &child) {
            return accumulated + child->getMoistureInfo()->getMoisture();
        }
    );

    setMoisture(sumMoisture / static_cast<double>(SpatialTreeTerrainCell::numberOfChildren));
}

const Temperature& SummaryMoisture::getRandomTemperature() const
{
    return summaryTerrainCell->getChildrenTerrainCells()[Random::randomIndex(SpatialTreeTerrainCell::numberOfChildren)]->getMoistureInfo()->getRandomTemperature();
}
