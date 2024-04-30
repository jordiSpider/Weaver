#ifndef CYCLE_MOISTURE_H_
#define CYCLE_MOISTURE_H_

#include <vector>
#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"


class CycleMoisture : public ExtendedMoisture
{
private:
    friend class boost::serialization::access;

protected:
    // Humidity time series
    const std::vector<double> relativeHumidityCycle;


    const double& getHumidityOnTimeStep(const unsigned int numberOfTimeSteps) const;

    void refreshRelativeHumidity(const unsigned int numberOfTimeSteps);

public:
    CycleMoisture(const nlohmann::json &moistureInfo);
    CycleMoisture(const CycleMoisture &other);
    virtual ~CycleMoisture();

    CycleMoisture* clone() const override;

    // Getters
    const std::vector<double>& getRelativeHumidityCycle() const;

    const std::string showMoistureInfo() const override;

    /**
     * @brief Serialize the CycleMoisture object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* CYCLE_MOISTURE_H_ */
