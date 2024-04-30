#ifndef SUBDIVISION_MOISTURE_H_
#define SUBDIVISION_MOISTURE_H_


#include "IBM/World/Map/TerrainCells/Moisture/MoistureInterface.h"
#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"



class SubdivisionMoisture : public MoistureInterface
{
protected:
    ExtendedMoisture* const moistureSource;

    void updateTemperature();
    void updateRelativeHumidity();

public:
    SubdivisionMoisture(ExtendedMoisture* const moistureSource);
    virtual ~SubdivisionMoisture();

    // Getters
    const Temperature& getTemperature() const;
    const double& getMoisture() const;
    const bool isInEnemyFreeSpace() const;
    const bool isInCompetitorFreeSpace() const;
    const double& getMaximumResourceCapacityDensity() const;
    const ExtendedMoisture* const getMoistureSource() const;
    ExtendedMoisture* const getMutableMoistureSource() const;
    const Temperature& getRandomTemperature() const;

    void updateInfo();

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* SUBDIVISION_MOISTURE_H_ */
