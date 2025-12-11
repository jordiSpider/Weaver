#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/BranchPatchApplicator.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"


using namespace std;



BranchPatchApplicator::BranchPatchApplicator()
{

}

BranchPatchApplicator::BranchPatchApplicator(TerrainCell* cell, MoistureSource* const moistureBaseSource)
    : SpatialTreePatchApplicator(cell, new BranchCellMoisture(moistureBaseSource, 0u), new BranchCellObstacle(0u), new BranchCellHabitatDomain(0u))
{

}


BranchPatchApplicator::~BranchPatchApplicator()
{
    
}


const BranchCellMoisture& BranchPatchApplicator::getCellMoisture() const
{
    return static_cast<const BranchCellMoisture&>(PatchApplicator::getCellMoisture());
}

BranchCellMoisture& BranchPatchApplicator::getMutableCellMoisture()
{
    return static_cast<BranchCellMoisture&>(PatchApplicator::getMutableCellMoisture());
}

const BranchCellResource& BranchPatchApplicator::getCellResource(const size_t resourceSpeciesId) const
{
    return static_cast<const BranchCellResource&>(PatchApplicator::getCellResource(resourceSpeciesId));
}

BranchCellResource& BranchPatchApplicator::getMutableCellResource(const size_t resourceSpeciesId)
{
    return static_cast<BranchCellResource&>(PatchApplicator::getMutableCellResource(resourceSpeciesId));
}

const BranchCellObstacle& BranchPatchApplicator::getCellObstacle() const
{
    return static_cast<const BranchCellObstacle&>(PatchApplicator::getCellObstacle());
}

BranchCellObstacle& BranchPatchApplicator::getMutableCellObstacle()
{
    return static_cast<BranchCellObstacle&>(PatchApplicator::getMutableCellObstacle());
}

const BranchCellHabitatDomain& BranchPatchApplicator::getCellHabitatDomain() const
{
    return static_cast<const BranchCellHabitatDomain&>(PatchApplicator::getCellHabitatDomain());
}

BranchCellHabitatDomain& BranchPatchApplicator::getMutableCellHabitatDomain()
{
    return static_cast<BranchCellHabitatDomain&>(PatchApplicator::getMutableCellHabitatDomain());
}


pair<bool, bool> BranchPatchApplicator::applyPatch(Landscape* const landscape, const Patch &patch)
{
    auto coverage = patch.checkCoverage(&owner->getEffectiveArea());

    if(coverage == Coverage::Full)
    {
        if(canApplyFullCoverage(patch))
        {
            applySource(landscape, patch.getSource(), patch.getPriority());
            return make_pair(true, true);
        }
        else
        {
            return applyPartialCoveragePatch(landscape, patch);
        }
    }
    else if(coverage >= Coverage::Partial)
    {
        return applyPartialCoveragePatch(landscape, patch);
    }

    return make_pair(false, false);
}


pair<bool, bool> BranchPatchApplicator::applyPartialCoveragePatch(Landscape* const landscape, const Patch &patch)
{
    bool isFullApplied = true, isApplied = false;
    
    for(const auto &child : static_cast<BranchTerrainCell*>(owner)->getMutableChildrenTerrainCells())
    {
        bool isFullChildApplied, isChildApplied;

        tie(isFullChildApplied, isChildApplied) = child->getMutablePatchApplicator().applyPatch(landscape, patch);

        isApplied = isApplied || isChildApplied;
        isFullApplied = isFullApplied && isFullChildApplied;
    }
    

    if(isFullApplied)
    {
        applySource(landscape, patch.getSource(), patch.getPriority());
    }


    return make_pair(isFullApplied, isApplied);
}



void BranchPatchApplicator::applySource(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority)
{
    for(auto &child : static_cast<BranchTerrainCell*>(owner)->getMutableChildrenTerrainCells())
    {
        child->getMutablePatchApplicator().applyDown(landscape, source, newPatchPriority);
    }

    PatchApplicator::setCellElement(landscape, source, newPatchPriority);

    if(static_cast<SpatialTreeTerrainCell*>(owner)->getParent() != nullptr)
    {
        static_cast<BranchPatchApplicator&>(static_cast<SpatialTreeTerrainCell*>(owner)->getMutableParent()->getMutablePatchApplicator()).applyUp(source);
    }
}


void BranchPatchApplicator::applyDown(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority)
{
    for(auto &child : static_cast<BranchTerrainCell*>(owner)->getMutableChildrenTerrainCells())
    {
        child->getMutablePatchApplicator().applyDown(landscape, source, newPatchPriority);
    }

    PatchApplicator::setCellElement(landscape, source, newPatchPriority);
}


void BranchPatchApplicator::applyUp(const PathSource &source)
{
    setCellElement(source);

    if(static_cast<SpatialTreeTerrainCell*>(owner)->getParent() != nullptr)
    {
        static_cast<BranchPatchApplicator&>(static_cast<SpatialTreeTerrainCell*>(owner)->getMutableParent()->getMutablePatchApplicator()).applyUp(source);
    }
}


void BranchPatchApplicator::addResourceSpecies(Landscape* const, vector<vector<vector<CellResource*>>>&, ResourceSpecies& resourceSpecies, ResourceSource* const)
{
    cellResources.push_back(new BranchCellResource(static_cast<BranchTerrainCell*>(owner), 0u, resourceSpecies));
}



void BranchPatchApplicator::setCellElement(const PathSource &source)
{
    switch(source.getType()) {
        case PathSource::Type::Moisture: {
            setCellMoisture(static_cast<const MoistureSource&>(source));
            break;
        }
        case PathSource::Type::Obstacle: {
            setCellObstacle(static_cast<const ObstacleSource&>(source));
            break;
        }
        case PathSource::Type::Resource: {
            setCellResource(static_cast<const ResourceSource&>(source));
            break;
        }
        case PathSource::Type::HabitatDomain: {
            setCellHabitatDomain(static_cast<const HabitatDomainSource&>(source));
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}

void BranchPatchApplicator::setCellMoisture(const MoistureSource &)
{
    getMutableCellMoisture().applyBranch(static_cast<BranchTerrainCell*>(owner));
}

void BranchPatchApplicator::setCellMoisture(Landscape* const, const MoistureSource &source, const size_t newMoisturePatchPriority)
{
    getMutableCellMoisture().applySource(&source, newMoisturePatchPriority, owner->getSize());
}

void BranchPatchApplicator::setCellResource(const ResourceSource &source)
{
    getMutableCellResource(source.getResourceSpeciesId()).applyBranch();
}

void BranchPatchApplicator::setCellResource(Landscape* const, const ResourceSource &source, const size_t)
{
    setCellResource(source);
}

void BranchPatchApplicator::setCellObstacle(const ObstacleSource &)
{
    getMutableCellObstacle().applyBranch(static_cast<BranchTerrainCell*>(owner));
}

void BranchPatchApplicator::setCellObstacle(Landscape* const, const ObstacleSource &, const size_t newObstaclePatchPriority)
{
    getMutableCellObstacle().applySource(newObstaclePatchPriority);
}

void BranchPatchApplicator::setCellHabitatDomain(const HabitatDomainSource &)
{
    getMutableCellHabitatDomain().applyBranch(static_cast<BranchTerrainCell*>(owner));
}

void BranchPatchApplicator::setCellHabitatDomain(Landscape* const, const HabitatDomainSource &source, const size_t newHabitatDomainPatchPriority)
{
    getMutableCellHabitatDomain().applySource(source, newHabitatDomainPatchPriority);
}









BOOST_CLASS_EXPORT(BranchPatchApplicator)

template <class Archive>
void BranchPatchApplicator::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<SpatialTreePatchApplicator>(*this);
}

// Specialisation
template void BranchPatchApplicator::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void BranchPatchApplicator::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void BranchPatchApplicator::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void BranchPatchApplicator::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
