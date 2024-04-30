
#include "IBM/World/Map/TerrainCells/Moisture/SubdivisionMoisture.h"

using namespace std;


SubdivisionMoisture::SubdivisionMoisture(ExtendedMoisture* const moistureSource)
    : MoistureInterface(),
      moistureSource(moistureSource)
{

}

SubdivisionMoisture::~SubdivisionMoisture()
{

}

void SubdivisionMoisture::updateTemperature()
{
    // Not to implement
}

void SubdivisionMoisture::updateRelativeHumidity()
{
    // Not to implement
}

void SubdivisionMoisture::updateInfo()
{
    throwLineInfoException("Subdivision moisture is not updated");
}

const double& SubdivisionMoisture::getMaximumResourceCapacityDensity() const
{
    return getMoistureSource()->getMaximumResourceCapacityDensity();
}

const Temperature& SubdivisionMoisture::getTemperature() const
{
    return getMoistureSource()->getTemperature();
}

const double& SubdivisionMoisture::getMoisture() const
{
    return getMoistureSource()->getMoisture();
}

const bool SubdivisionMoisture::isInEnemyFreeSpace() const
{
    return getMoistureSource()->isInEnemyFreeSpace();
}

const bool SubdivisionMoisture::isInCompetitorFreeSpace() const
{
    return getMoistureSource()->isInCompetitorFreeSpace();
}

const ExtendedMoisture* const SubdivisionMoisture::getMoistureSource() const
{
    return moistureSource;
}

ExtendedMoisture* const SubdivisionMoisture::getMutableMoistureSource() const
{
    return moistureSource;
}

const Temperature& SubdivisionMoisture::getRandomTemperature() const
{
    return getMoistureSource()->getRandomTemperature();
}

template <class Archive>
void SubdivisionMoisture::serialize(Archive &ar, const unsigned int version) 
{
    ar & boost::serialization::base_object<MoistureInterface>(*this);
}

// Specialisation
template void SubdivisionMoisture::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SubdivisionMoisture::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SubdivisionMoisture::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SubdivisionMoisture::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
