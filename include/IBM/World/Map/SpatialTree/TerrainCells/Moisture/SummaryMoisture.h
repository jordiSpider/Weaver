#ifndef SUMMARY_MOISTURE_H_
#define SUMMARY_MOISTURE_H_

#include <vector>
#include <numeric>

#include "IBM/World/Map/TerrainCells/Moisture/MoistureInterface.h"


// Forward declaration
class BranchTerrainCellInterface;

class SummaryMoisture : public MoistureInterface
{
protected:
    BranchTerrainCellInterface* const summaryTerrainCell;

    // Initial absence indicator for animals in hunting mode
    bool inEnemyFreeSpace;
    // Initial absence indicator for animals not in hunting mode
    bool inCompetitorFreeSpace;

    // Current temperature
    Temperature temperature;

    // Current humidity
    double moisture;

    void updateTemperature();
    void updateRelativeHumidity();

    void updateInEnemyFreeSpace();
    void updateInCompetitorFreeSpace();

    void setTemperature(const Temperature &newTemperature);
    void setMoisture(const double &newMoisture);
    void setInEnemyFreeSpace(const bool newInEnemyFreeSpace);
    void setInCompetitorFreeSpace(const bool newInCompetitorFreeSpace);

public:
    SummaryMoisture(BranchTerrainCellInterface* const summaryTerrainCell);
    virtual ~SummaryMoisture();

    const Temperature& getRandomTemperature() const;
    const Temperature& getTemperature() const;
    const double& getMoisture() const;
    const bool isInEnemyFreeSpace() const;
    const bool isInCompetitorFreeSpace() const;
    const double& getMaximumResourceCapacityDensity() const;

    void updateInfo();
};

#endif /* SUMMARY_MOISTURE_H_ */
