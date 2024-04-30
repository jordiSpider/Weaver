
#include "IBM/World/Map/TerrainCells/Moisture/UpdateMethodType/RainEventAndDecayOverTimeMoisture.h"

using namespace std;
using json = nlohmann::json;


RainEventAndDecayOverTimeMoisture::RainEventAndDecayOverTimeMoisture(const json &moistureInfo)
    : ExtendedMoisture(moistureInfo),
      relativeHumidityOnRainEvent(moistureInfo["relativeHumidityUpdateMethod"]["rainEventAndDecayOverTimeParams"]["relativeHumidityOnRainEvent"]), 
      timeStepsBetweenRainEvents(moistureInfo["relativeHumidityUpdateMethod"]["rainEventAndDecayOverTimeParams"]["timeStepsBetweenRainEvents"]),
      standardDeviationForRainEvent(moistureInfo["relativeHumidityUpdateMethod"]["rainEventAndDecayOverTimeParams"]["standardDeviationForRainEvent"]), 
      relativeHumidityDecayOverTime(moistureInfo["relativeHumidityUpdateMethod"]["rainEventAndDecayOverTimeParams"]["relativeHumidityDecayOverTime"])
{

}

RainEventAndDecayOverTimeMoisture::RainEventAndDecayOverTimeMoisture(const RainEventAndDecayOverTimeMoisture &other)
    : ExtendedMoisture(other),
      relativeHumidityOnRainEvent(other.getRelativeHumidityOnRainEvent()),
      timeStepsBetweenRainEvents(other.getTimeStepsBetweenRainEvents()), 
      standardDeviationForRainEvent(other.getStandardDeviationForRainEvent()), 
      relativeHumidityDecayOverTime(other.getRelativeHumidityDecayOverTime())
{

}

RainEventAndDecayOverTimeMoisture::~RainEventAndDecayOverTimeMoisture()
{

}

RainEventAndDecayOverTimeMoisture* RainEventAndDecayOverTimeMoisture::clone() const
{
    return new RainEventAndDecayOverTimeMoisture(*this);
}

const double& RainEventAndDecayOverTimeMoisture::getRelativeHumidityOnRainEvent() const
{
    return relativeHumidityOnRainEvent;
}

const double& RainEventAndDecayOverTimeMoisture::getRelativeHumidityDecayOverTime() const
{
    return relativeHumidityDecayOverTime;
}

const unsigned int RainEventAndDecayOverTimeMoisture::getTimeStepsBetweenRainEvents() const
{
    return timeStepsBetweenRainEvents;
}

const double RainEventAndDecayOverTimeMoisture::getStandardDeviationForRainEvent() const
{
    return standardDeviationForRainEvent;
}

void RainEventAndDecayOverTimeMoisture::refreshRelativeHumidity(const unsigned int numberOfTimeSteps)
{
    if(isOnRainEvent(numberOfTimeSteps))
	{
		setMoisture(getRelativeHumidityOnRainEvent());
	}
	else
	{
		double newMoisture = getMoisture() - getRelativeHumidityDecayOverTime();
		setMoisture((newMoisture < 0.0) ? 0.0 : newMoisture);
	}
}

bool RainEventAndDecayOverTimeMoisture::isOnRainEvent(const unsigned int timeStep) const
{
    return timeStep % getTimeStepsBetweenRainEvents() == 0;
}

const string RainEventAndDecayOverTimeMoisture::showMoistureInfo() const
{
	return fmt::format(
		"{}\n   - Relative humidity on rain event = {}\n   - Time steps between rain events = {}\n   - Standard deviation for rain event = {}\n   - Relative humidity decay over time = {}", 
        ExtendedMoisture::showMoistureInfo(), getRelativeHumidityOnRainEvent(), getTimeStepsBetweenRainEvents(),
        getStandardDeviationForRainEvent(), getRelativeHumidityDecayOverTime()
	);
}

template <class Archive>
void RainEventAndDecayOverTimeMoisture::serialize(Archive &ar, const unsigned int version) {
    ar & relativeHumidityOnRainEvent;
    ar & timeStepsBetweenRainEvents;
    ar & standardDeviationForRainEvent;
    ar & relativeHumidityDecayOverTime;
}
