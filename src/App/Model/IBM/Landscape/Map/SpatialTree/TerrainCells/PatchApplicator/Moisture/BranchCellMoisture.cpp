
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Moisture/BranchCellMoisture.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"

using namespace std;



size_t BranchCellMoisture::calculateMoisturePatchPriority(const BranchTerrainCell* const summaryTerrainCell)
{
    size_t moisturePatchPriority = 0u;

    for(const auto &child : summaryTerrainCell->getChildrenTerrainCells())
    {
        if(moisturePatchPriority < child->getPatchApplicator().getCellMoisture().getPatchPriority())
        {
            moisturePatchPriority = child->getPatchApplicator().getCellMoisture().getPatchPriority();
        }
    }

    return moisturePatchPriority;
}

WetMass BranchCellMoisture::calculateTotalMaximumResourceCapacity(const BranchTerrainCell* const summaryTerrainCell)
{
    WetMass sumTotalMaximumResourceCapacity = accumulate(
        summaryTerrainCell->getChildrenTerrainCells().cbegin(), summaryTerrainCell->getChildrenTerrainCells().cend(), WetMass(0.0),
        [](WetMass accumulated, const auto &child) {
            return accumulated + child->getPatchApplicator().getCellMoisture().getTotalMaximumResourceCapacity();
        }
    );

    return sumTotalMaximumResourceCapacity;
}

bool BranchCellMoisture::calculateInEnemyFreeSpace(const BranchTerrainCell* const summaryTerrainCell)
{
    unsigned int sumInEnemyFreeSpace = accumulate(
        summaryTerrainCell->getChildrenTerrainCells().cbegin(), summaryTerrainCell->getChildrenTerrainCells().cend(), 0u,
        [](unsigned int accumulated, const auto &child) {
            return accumulated + (child->getPatchApplicator().getCellMoisture().isInEnemyFreeSpace()) ? 1u : 0u;
        }
    );

    return PreciseDouble(static_cast<double>(sumInEnemyFreeSpace)) / PreciseDouble(static_cast<double>(SpatialTreeTerrainCell::numberOfChildren)) >= 0.5;
}

bool BranchCellMoisture::calculateInCompetitorFreeSpace(const BranchTerrainCell* const summaryTerrainCell)
{
    unsigned int sumInCompetitorFreeSpace = accumulate(
        summaryTerrainCell->getChildrenTerrainCells().cbegin(), summaryTerrainCell->getChildrenTerrainCells().cend(), 0u,
        [](unsigned int accumulated, const auto &child) {
            return accumulated + (child->getPatchApplicator().getCellMoisture().isInCompetitorFreeSpace()) ? 1u : 0u;
        }
    );

    return PreciseDouble(static_cast<double>(sumInCompetitorFreeSpace)) / PreciseDouble(static_cast<double>(SpatialTreeTerrainCell::numberOfChildren)) >= 0.5;
}




BranchCellMoisture::BranchCellMoisture()
    : CellMoistureInterface()
{
    
}

BranchCellMoisture::BranchCellMoisture(const MoistureSource* const newMoistureSource, const size_t newMoisturePatchPriority)
    : CellMoistureInterface(newMoisturePatchPriority), moistureSource(newMoistureSource), isSource(true), 
      summaryTerrainCell(nullptr)
{
    
}


BranchCellMoisture::~BranchCellMoisture()
{

}


void BranchCellMoisture::applySource(const MoistureSource* const newMoistureSource, const size_t newMoisturePatchPriority, const PreciseDouble& cellSize)
{
    setPatchPriority(newMoisturePatchPriority);
    setTotalMaximumResourceCapacity(CellMoistureInterface::calculateTotalMaximumResourceCapacity(newMoistureSource, cellSize));
    moistureSource = newMoistureSource;
    isSource = true;
    summaryTerrainCell = nullptr;
}


void BranchCellMoisture::applyBranch(BranchTerrainCell* const newSummaryTerrainCell)
{
    setPatchPriority(BranchCellMoisture::calculateMoisturePatchPriority(newSummaryTerrainCell));
    setTotalMaximumResourceCapacity(BranchCellMoisture::calculateTotalMaximumResourceCapacity(newSummaryTerrainCell));
    moistureSource = nullptr;
    isSource = false;
    summaryTerrainCell = newSummaryTerrainCell;
    inEnemyFreeSpace = BranchCellMoisture::calculateInEnemyFreeSpace(newSummaryTerrainCell),
    inCompetitorFreeSpace = BranchCellMoisture::calculateInCompetitorFreeSpace(newSummaryTerrainCell);

    update();
}


void BranchCellMoisture::updateTemperature()
{
    if(!isSource)
    {
        Temperature sumTemperature;

        for(const auto &child : summaryTerrainCell->getChildrenTerrainCells())
        {
            sumTemperature += child->getPatchApplicator().getCellMoisture().getTemperature();
        }

        sumTemperature.setTemperatureCelsius(sumTemperature.getTemperatureCelsius() / static_cast<double>(SpatialTreeTerrainCell::numberOfChildren));
        setTemperature(sumTemperature);
    }
}

void BranchCellMoisture::addTemperatureRange(unordered_set<Temperature>& globalTemperatureRange, vector<Temperature>& temperatureCycle) const
{
    vector<vector<Temperature>> childrenTemperatureCycle;
    pair<size_t, size_t> twoMaximumSizes(0, 0);


    for(const auto& child : summaryTerrainCell->getChildrenTerrainCells())
    {
        childrenTemperatureCycle.emplace_back();
        child->getPatchApplicator().getCellMoisture().addTemperatureRange(globalTemperatureRange, childrenTemperatureCycle.back());

        if(twoMaximumSizes.first < childrenTemperatureCycle.back().size())
        {
            twoMaximumSizes.second = twoMaximumSizes.first;
            twoMaximumSizes.first = childrenTemperatureCycle.back().size();
        }
        else if(twoMaximumSizes.second < childrenTemperatureCycle.back().size())
        {
            twoMaximumSizes.second = childrenTemperatureCycle.back().size();
        }
    }


    for(size_t i = 0; i < twoMaximumSizes.first*twoMaximumSizes.second; i++)
    {
        Temperature timeStepTemperature(0.0);

        for(const vector<Temperature>& childTemperatureCycle : childrenTemperatureCycle)
        {
            timeStepTemperature = timeStepTemperature + childTemperatureCycle.at(i % childTemperatureCycle.size());
        }

        timeStepTemperature = timeStepTemperature / static_cast<double>(childrenTemperatureCycle.size());


        temperatureCycle.push_back(timeStepTemperature);
    }


    globalTemperatureRange.insert(temperatureCycle.begin(), temperatureCycle.end());
}

void BranchCellMoisture::setTemperature(const Temperature &newTemperature)
{
    temperature = newTemperature;
}

void BranchCellMoisture::setMoisture(const PreciseDouble &newMoisture)
{
    moisture = newMoisture;
}

const Temperature& BranchCellMoisture::getTemperature() const
{
    return (isSource) ? moistureSource->getTemperature() : temperature;
}

const PreciseDouble& BranchCellMoisture::getMoisture() const
{
    return (isSource) ? moistureSource->getMoisture() : moisture;
}

bool BranchCellMoisture::isInEnemyFreeSpace() const
{
    return (isSource) ? moistureSource->isInEnemyFreeSpace() : inEnemyFreeSpace;
}

bool BranchCellMoisture::isInCompetitorFreeSpace() const
{
    return (isSource) ? moistureSource->isInCompetitorFreeSpace() : inCompetitorFreeSpace;
}

const WetMass& BranchCellMoisture::getMaximumResourceCapacityDensity() const
{
    throwLineInfoException("Summary moisture has not implemented this method");
}

void BranchCellMoisture::updateRelativeHumidity()
{
    if(!isSource)
    {
        PreciseDouble sumMoisture = accumulate(
            summaryTerrainCell->getChildrenTerrainCells().cbegin(), summaryTerrainCell->getChildrenTerrainCells().cend(), 0.0,
            [](double accumulated, const auto &child) {
                return accumulated + child->getPatchApplicator().getCellMoisture().getMoisture().getValue();
            }
        );

        setMoisture(sumMoisture / static_cast<double>(SpatialTreeTerrainCell::numberOfChildren));   
    }
}

void BranchCellMoisture::deserializeCellMoisture(vector<pair<size_t, MoistureSource*>>& appliedMoisture, TerrainCell* newTerrainCell)
{
    if(isSource)
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
    else
    {
        summaryTerrainCell = static_cast<BranchTerrainCell*>(newTerrainCell);
    }
}



BOOST_CLASS_EXPORT(BranchCellMoisture)

template <class Archive>
void BranchCellMoisture::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<CellMoistureInterface>(*this);

    ar & isSource;

    ar & inEnemyFreeSpace;
    ar & inCompetitorFreeSpace;
    ar & temperature;
    ar & moisture;
}

// Specialisation
template void BranchCellMoisture::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void BranchCellMoisture::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void BranchCellMoisture::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void BranchCellMoisture::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
