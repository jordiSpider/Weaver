#ifndef MOISTURE_INTERFACE_H_
#define MOISTURE_INTERFACE_H_

#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/Physics/Temperature.h"

class MoistureInterface
{
private:
    friend class boost::serialization::access;

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

    /**
     * @brief Serialize the MoistureInterface object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(MoistureInterface)

#endif /* MOISTURE_INTERFACE_H_ */
