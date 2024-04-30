#ifndef EXTENDED_MOISTURE_H_
#define EXTENDED_MOISTURE_H_

#include <vector>
#include <nlohmann/json.hpp>
#include <string>
#include <magic_enum.hpp>

#include "World/Map/TerrainCells/Moisture/MoistureInterface.h"
#include "Physics/Temperature.h"
#include "Exceptions/LineInfoException.h"
#include "Maths/Random.h"
#include "Misc/Macros.h"


class ExtendedMoisture : public MoistureInterface
{
protected:
    // Initial absence indicator for animals in hunting mode
    bool inEnemyFreeSpace;
    // Initial absence indicator for animals not in hunting mode
    bool inCompetitorFreeSpace;

    // Current temperature
    Temperature temperature;

    // Current humidity
    double moisture;

    // Maximum capacity to all resources per spatial unit
    const double maximumResourceCapacityDensity;

    // Temperature time series
    const std::vector<Temperature> temperatureCycle;


    // Setters
    void setTemperature(const Temperature &newTemperature);
    void setMoisture(const double &newMoisture);
    void setInEnemyFreeSpace(const bool newInEnemyFreeSpace);
    void setInCompetitorFreeSpace(const bool newInCompetitorFreeSpace);

    const Temperature& getTemperatureOnTimeStep(const unsigned int timeStep) const;

    void updateTemperature();
    void updateRelativeHumidity();

    std::vector<Temperature> obtainTemperatureCycle(const nlohmann::json &temperatureCycleValues);

    void refreshTemperature(const unsigned int timeStep);
    virtual void refreshRelativeHumidity(const unsigned int timeStep)=0;

public:
    class Type {
    public:
        enum TypeValue : unsigned int
		{
            Cycle,
            RainEventAndDecayOverTime
		};

        static const TypeValue stringToEnumValue(const std::string &str);
        static constexpr size_t size();

    private:
        static const std::unordered_map<std::string_view, const TypeValue> stringToEnum;
        static const std::string enumValues;

        static std::string_view to_string(const TypeValue& type);
        static const std::unordered_map<std::string_view, const TypeValue> generateMap();
        static const std::string generateAvailableValues();
        static std::string_view printAvailableValues();
    };

    ExtendedMoisture(const nlohmann::json &moistureInfo);
    ExtendedMoisture(const ExtendedMoisture& other);
    virtual ~ExtendedMoisture();

    virtual ExtendedMoisture* clone() const=0;

    // Getters
    const Temperature& getTemperature() const;
    const double& getMoisture() const;
    const bool isInEnemyFreeSpace() const;
    const bool isInCompetitorFreeSpace() const;
    const std::vector<Temperature>& getTemperatureCycle() const;
    const Temperature& getRandomTemperature() const;
    const double& getMaximumResourceCapacityDensity() const;

    virtual const std::string showMoistureInfo() const;

    void refreshValue(const unsigned int timeStep);

    void updateInfo();
};

#endif /* EXTENDED_MOISTURE_H_ */
