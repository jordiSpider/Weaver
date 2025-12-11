#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Moisture/CellMoistureInterface.h"

#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"


using namespace std;




WetMass CellMoistureInterface::calculateTotalMaximumResourceCapacity(const MoistureSource* source, const PreciseDouble& cellSize)
{
    return WetMass(pow(cellSize, DIMENSIONS) * source->getMaximumResourceCapacityDensity().getValue());
}



CellMoistureInterface::CellMoistureInterface()
    : CellElement()
{

}

CellMoistureInterface::CellMoistureInterface(const size_t newMoisturePatchPriority)
    : CellElement(newMoisturePatchPriority), totalMaximumResourceCapacity(0.0)
{

}

CellMoistureInterface::~CellMoistureInterface()
{

}

void CellMoistureInterface::update()
{
    updateTemperature();
    updateRelativeHumidity();
}

const WetMass& CellMoistureInterface::getTotalMaximumResourceCapacity() const
{
    return totalMaximumResourceCapacity;
}

void CellMoistureInterface::setTotalMaximumResourceCapacity(const WetMass& newValue)
{
    totalMaximumResourceCapacity = newValue;
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(CellMoistureInterface)

template <class Archive>
void CellMoistureInterface::serialize(Archive& ar, const unsigned int) 
{
    ar & boost::serialization::base_object<CellElement>(*this);

    ar & totalMaximumResourceCapacity;
}

// Specialisation
template void CellMoistureInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellMoistureInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellMoistureInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellMoistureInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
