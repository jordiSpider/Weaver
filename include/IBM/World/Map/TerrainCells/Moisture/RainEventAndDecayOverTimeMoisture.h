#ifndef RAIN_EVENT_AND_DECAY_OVER_TIME_MOISTURE_H_
#define RAIN_EVENT_AND_DECAY_OVER_TIME_MOISTURE_H_

#include <vector>
#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"


class RainEventAndDecayOverTimeMoisture : public ExtendedMoisture
{
private:
    friend class boost::serialization::access;

protected:
    // Humidity during the rain event
    double relativeHumidityOnRainEvent;
    // Time between rain events
    unsigned int timeStepsBetweenRainEvents;
    // I don't know what it is
    double standardDeviationForRainEvent;

    // Humidity loss over time
    double relativeHumidityDecayOverTime;


    bool isOnRainEvent(const unsigned int timeStep) const;

    void refreshRelativeHumidity(const unsigned int timeStep);

public:
    RainEventAndDecayOverTimeMoisture();
    RainEventAndDecayOverTimeMoisture(const nlohmann::json &moistureInfo);
    RainEventAndDecayOverTimeMoisture(const RainEventAndDecayOverTimeMoisture &other);
    virtual ~RainEventAndDecayOverTimeMoisture();

    RainEventAndDecayOverTimeMoisture* clone() const override;

    // Getters
    const double& getRelativeHumidityOnRainEvent() const;
    const double& getRelativeHumidityDecayOverTime() const;
    const unsigned int getTimeStepsBetweenRainEvents() const;
    const double getStandardDeviationForRainEvent() const;

    const Type getExtendedMoistureType() const;

    const std::string showMoistureInfo() const override;

    /**
     * @brief Serialize the RainEventAndDecayOverTimeMoisture object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* RAIN_EVENT_AND_DECAY_OVER_TIME_MOISTURE_H_ */
