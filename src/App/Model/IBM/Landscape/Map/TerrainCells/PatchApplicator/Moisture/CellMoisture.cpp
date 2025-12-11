
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Moisture/CellMoisture.h"


using namespace std;







CellMoisture::CellMoisture()
    : CellMoistureInterface()
{

}

CellMoisture::CellMoisture(const MoistureSource* const newMoistureSource, const size_t newMoisturePatchPriority)
    : CellMoistureInterface(newMoisturePatchPriority), moistureSource(newMoistureSource)
{

}

CellMoisture::~CellMoisture()
{
    
}

void CellMoisture::applySource(const MoistureSource* const newMoistureSource, const size_t newMoisturePatchPriority, const PreciseDouble& cellSize)
{
    setPatchPriority(newMoisturePatchPriority);
    setTotalMaximumResourceCapacity(CellMoistureInterface::calculateTotalMaximumResourceCapacity(newMoistureSource, cellSize));
    moistureSource = newMoistureSource;
}

void CellMoisture::updateTemperature()
{
    // Not to implement
}

void CellMoisture::updateRelativeHumidity()
{
    // Not to implement
}

void CellMoisture::addTemperatureRange(unordered_set<Temperature>& globalTemperatureRange, vector<Temperature>& temperatureCycle) const
{
    transform(moistureSource->getTemperatureDynamics().getCycle().begin(), moistureSource->getTemperatureDynamics().getCycle().end(), back_inserter(temperatureCycle), [](const PreciseDouble& value) {
        return Temperature(value);
    });

    globalTemperatureRange.insert(temperatureCycle.begin(), temperatureCycle.end());
}


const MoistureSource* CellMoisture::getMoistureSource() const
{
    return moistureSource;
}


const WetMass& CellMoisture::getMaximumResourceCapacityDensity() const
{
    return moistureSource->getMaximumResourceCapacityDensity();
}

const Temperature& CellMoisture::getTemperature() const
{
    return moistureSource->getTemperature();
}

const PreciseDouble& CellMoisture::getMoisture() const
{
    return moistureSource->getMoisture();
}

bool CellMoisture::isInEnemyFreeSpace() const
{
    return moistureSource->isInEnemyFreeSpace();
}

bool CellMoisture::isInCompetitorFreeSpace() const
{
    return moistureSource->isInCompetitorFreeSpace();
}

void CellMoisture::deserializeCellMoisture(vector<pair<size_t, MoistureSource*>>& appliedMoisture, TerrainCell*)
{
    bool found = false;

    for(size_t i = 0; i < appliedMoisture.size() && !found; i++)
    {
        if(getPatchPriority() == appliedMoisture[i].first)
        {
            moistureSource = appliedMoisture[i].second;
            found = true;
        }
    }
}



BOOST_CLASS_EXPORT(CellMoisture)

template <class Archive>
void CellMoisture::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<CellMoistureInterface>(*this);
}

// Specialisation
template void CellMoisture::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellMoisture::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellMoisture::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellMoisture::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
