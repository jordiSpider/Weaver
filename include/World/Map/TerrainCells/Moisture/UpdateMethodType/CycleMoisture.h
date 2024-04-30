#ifndef CYCLE_MOISTURE_H_
#define CYCLE_MOISTURE_H_

#include <vector>
#include <nlohmann/json.hpp>

#include "World/Map/TerrainCells/Moisture/ExtendedMoisture.h"


class CycleMoisture : public ExtendedMoisture
{
protected:
    // Humidity time series
    const std::vector<double> relativeHumidityCycle;


    const double& getHumidityOnTimeStep(const unsigned int timeStep) const;

    void refreshRelativeHumidity(const unsigned int timeStep);

public:
    CycleMoisture(const nlohmann::json &moistureInfo);
    CycleMoisture(const CycleMoisture &other);
    virtual ~CycleMoisture();

    CycleMoisture* clone() const override;

    // Getters
    const std::vector<double>& getRelativeHumidityCycle() const;

    const std::string showMoistureInfo() const override;
};

#endif /* CYCLE_MOISTURE_H_ */
