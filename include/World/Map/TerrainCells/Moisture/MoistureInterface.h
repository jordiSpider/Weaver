#ifndef MOISTURE_INTERFACE_H_
#define MOISTURE_INTERFACE_H_

#include "Physics/Temperature.h"

class MoistureInterface
{
protected:
    virtual void updateTemperature()=0;
    virtual void updateRelativeHumidity()=0;

public:
    MoistureInterface();
    virtual ~MoistureInterface();

    // Getters
    virtual const Temperature& getTemperature() const=0;
    virtual const double& getMoisture() const=0;
    virtual const bool isInEnemyFreeSpace() const=0;
    virtual const bool isInCompetitorFreeSpace() const=0;
    virtual const double& getMaximumResourceCapacityDensity() const=0;
    virtual const Temperature& getRandomTemperature() const=0;

    virtual void updateInfo()=0;

    void update();
};

#endif /* MOISTURE_INTERFACE_H_ */
