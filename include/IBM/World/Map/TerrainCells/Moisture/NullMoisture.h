#ifndef NULL_MOISTURE_H_
#define NULL_MOISTURE_H_

#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/TerrainCells/Moisture/MoistureInterface.h"
#include "Exceptions/LineInfoException.h"


class NullMoisture : public MoistureInterface
{
private:
    friend class boost::serialization::access;

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

    /**
     * @brief Serialize the NullMoisture object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* NULL_MOISTURE_H_ */
