#ifndef EXTENDED_MOISTURE_H_
#define EXTENDED_MOISTURE_H_

#include <vector>
#include <nlohmann/json.hpp>
#include <string>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/TerrainCells/Moisture/MoistureInterface.h"
#include "IBM/Physics/Temperature.h"
#include "Exceptions/LineInfoException.h"
#include "IBM/Maths/Random.h"
#include "Misc/Macros.h"
#include "Misc/EnumClass.h"


class ExtendedMoisture : public MoistureInterface
{
private:
    friend class boost::serialization::access;

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
    double maximumResourceCapacityDensity;

    // Temperature time series
    std::vector<Temperature> temperatureCycle;


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
    enum class Type : unsigned int
    {
        Cycle,
        RainEventAndDecayOverTime
    };


    static std::unique_ptr<ExtendedMoisture> createInstance(const nlohmann::json &moistureInfo);
    static std::unique_ptr<ExtendedMoisture> createInstance(const Type &extendedMoistureType);


    ExtendedMoisture();
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

    virtual const Type getExtendedMoistureType() const=0;

    virtual const std::string showMoistureInfo() const;

    void refreshValue(const unsigned int timeStep);

    void updateInfo();

    /**
     * @brief Serialize the ExtendedMoisture object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};


namespace boost {
    namespace serialization {
        /**
         * @brief Serialize a ExtendedMoisture pointer.
         *
         * This function handles the serialization and deserialization of a ExtendedMoisture pointer.
         *
         * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
         * @param ar The archive to use.
         * @param extendedMoisturePtr The ExtendedMoisture pointer to serialize or deserialize.
         * @param version The version of the serialization format.
         */
        template <class Archive>
        void serialize(Archive &ar, ExtendedMoisture* &extendedMoisturePtr, const unsigned int version, std::vector<ExtendedMoisture*>& appliedMoisture);
    }
}


#endif /* EXTENDED_MOISTURE_H_ */
