#ifndef RAIN_EVENT_AND_DECAY_OVER_TIME_MOISTURE_H_
#define RAIN_EVENT_AND_DECAY_OVER_TIME_MOISTURE_H_

#include <vector>
#include <nlohmann/json.hpp>

#include "World/Map/TerrainCells/Moisture/ExtendedMoisture.h"


class RainEventAndDecayOverTimeMoisture : public ExtendedMoisture
{
protected:
    // Humidity during the rain event
    const double relativeHumidityOnRainEvent;
    // Time between rain events
    const unsigned int timeStepsBetweenRainEvents;
    // I don't know what it is
    const double standardDeviationForRainEvent;

    // Humidity loss over time
    const double relativeHumidityDecayOverTime;


    bool isOnRainEvent(const unsigned int timeStep) const;

    void refreshRelativeHumidity(const unsigned int timeStep);

public:
    RainEventAndDecayOverTimeMoisture(const nlohmann::json &moistureInfo);
    RainEventAndDecayOverTimeMoisture(const RainEventAndDecayOverTimeMoisture &other);
    virtual ~RainEventAndDecayOverTimeMoisture();

    RainEventAndDecayOverTimeMoisture* clone() const override;

    // Getters
    const double& getRelativeHumidityOnRainEvent() const;
    const double& getRelativeHumidityDecayOverTime() const;
    const unsigned int getTimeStepsBetweenRainEvents() const;
    const double getStandardDeviationForRainEvent() const;

    const std::string showMoistureInfo() const override;
};

#endif /* RAIN_EVENT_AND_DECAY_OVER_TIME_MOISTURE_H_ */
