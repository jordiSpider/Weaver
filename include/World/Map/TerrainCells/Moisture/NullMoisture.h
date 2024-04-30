#ifndef NULL_MOISTURE_H_
#define NULL_MOISTURE_H_

#include "World/Map/TerrainCells/Moisture/MoistureInterface.h"
#include "Exceptions/LineInfoException.h"


class NullMoisture : public MoistureInterface
{
protected:
    // Initial absence indicator for animals in hunting mode
    const bool inEnemyFreeSpace;
    // Initial absence indicator for animals not in hunting mode
    const bool inCompetitorFreeSpace;

    // Current temperature
    const Temperature temperature;

    // Current humidity
    const double moisture;

    void updateTemperature();
    void updateRelativeHumidity();

public:
    NullMoisture();
    virtual ~NullMoisture();

    // Getters
    const Temperature& getTemperature() const;
    const double& getMoisture() const;
    const bool isInEnemyFreeSpace() const;
    const bool isInCompetitorFreeSpace() const;
    const double& getMaximumResourceCapacityDensity() const;
    const Temperature& getRandomTemperature() const;

    void updateInfo();
};

#endif /* NULL_MOISTURE_H_ */
