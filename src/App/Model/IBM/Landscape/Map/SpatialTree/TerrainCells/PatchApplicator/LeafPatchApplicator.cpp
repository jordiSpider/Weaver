#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/LeafPatchApplicator.h"

#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"
#include "App/Model/IBM/Landscape/Landscape.h"


using namespace std;



LeafPatchApplicator::LeafPatchApplicator()
{

}

LeafPatchApplicator::LeafPatchApplicator(TerrainCell* cell, MoistureSource* const moistureBaseSource)
    : SpatialTreePatchApplicator(cell, new CellMoisture(moistureBaseSource, 0u), new CellObstacle(0u), new CellHabitatDomain(0u)),
      totalResourceBiomass(0.0), totalFullCapacity(false)
{

}


LeafPatchApplicator::~LeafPatchApplicator()
{
    
}


const CellMoisture& LeafPatchApplicator::getCellMoisture() const
{
    return static_cast<const CellMoisture&>(PatchApplicator::getCellMoisture());
}

CellMoisture& LeafPatchApplicator::getMutableCellMoisture()
{
    return static_cast<CellMoisture&>(PatchApplicator::getMutableCellMoisture());
}

const CellResource& LeafPatchApplicator::getCellResource(const size_t resourceSpeciesId) const
{
    return static_cast<const CellResource&>(PatchApplicator::getCellResource(resourceSpeciesId));
}

CellResource& LeafPatchApplicator::getMutableCellResource(const size_t resourceSpeciesId)
{
    return static_cast<CellResource&>(PatchApplicator::getMutableCellResource(resourceSpeciesId));
}

const CellObstacle& LeafPatchApplicator::getCellObstacle() const
{
    return static_cast<const CellObstacle&>(PatchApplicator::getCellObstacle());
}

CellObstacle& LeafPatchApplicator::getMutableCellObstacle()
{
    return static_cast<CellObstacle&>(PatchApplicator::getMutableCellObstacle());
}

const CellHabitatDomain& LeafPatchApplicator::getCellHabitatDomain() const
{
    return static_cast<const CellHabitatDomain&>(PatchApplicator::getCellHabitatDomain());
}

CellHabitatDomain& LeafPatchApplicator::getMutableCellHabitatDomain()
{
    return static_cast<CellHabitatDomain&>(PatchApplicator::getMutableCellHabitatDomain());
}


void LeafPatchApplicator::addResourceSpecies(Landscape* const landscape, vector<vector<vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource)
{
    CellResource* newResource = new CellResource(landscape, owner, 0u, resourceSpecies, resourceBaseSource);

    cellResources.push_back(newResource);
    
    landscapeResources[owner->getPosition().get(Axis::X)][owner->getPosition().get(Axis::Y)].push_back(newResource);
}





pair<bool, bool> LeafPatchApplicator::applyPatch(Landscape* const landscape, const Patch &patch)
{
    if(canApplyFullCoverage(patch))
    {
        auto coverage = patch.checkCoverage(&owner->getEffectiveArea());

        if(coverage >= Coverage::Over50Percent)
        {
            applySource(landscape, patch.getSource(), patch.getPriority());
            return make_pair(true, true);
        }
    }

    return make_pair(false, false);
}


pair<bool, bool> LeafPatchApplicator::applyPartialCoveragePatch(Landscape* const landscape, const Patch &patch)
{
    return applyPatch(landscape, patch);
}


void LeafPatchApplicator::applySource(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority)
{
    setCellElement(landscape, source, newPatchPriority);

    if(static_cast<SpatialTreeTerrainCell*>(owner)->getParent() != nullptr)
    {
        static_cast<BranchPatchApplicator&>(static_cast<SpatialTreeTerrainCell*>(owner)->getMutableParent()->getMutablePatchApplicator()).applyUp(source);
    }
}


void LeafPatchApplicator::applyDown(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority)
{
    setCellElement(landscape, source, newPatchPriority);
}



void LeafPatchApplicator::setCellMoisture(Landscape* const, const MoistureSource &source, const size_t newMoisturePatchPriority)
{
    getMutableCellMoisture().applySource(&source, newMoisturePatchPriority, owner->getSize());
}


void LeafPatchApplicator::setCellResource(Landscape* const landscape, const ResourceSource &source, const size_t newResourcePatchPriority)
{
    getMutableCellResource(source.getResourceSpeciesId()).applySource(landscape, &source, newResourcePatchPriority);
}


void LeafPatchApplicator::registerResources(vector<vector<vector<CellResource*>>>& landscapeResources)
{
    size_t x = owner->getPosition().get(Axis::X);
	size_t y = owner->getPosition().get(Axis::Y);

    for(size_t resourceSpeciesId = 0; resourceSpeciesId < getNumberOfResources(); resourceSpeciesId++)
    {
        landscapeResources[x][y][resourceSpeciesId] = &getMutableCellResource(resourceSpeciesId);
    }
}


void LeafPatchApplicator::setCellObstacle(Landscape* const, const ObstacleSource&, const size_t newObstaclePatchPriority)
{
    getMutableCellObstacle().applySource(newObstaclePatchPriority);
}


void LeafPatchApplicator::setCellHabitatDomain(Landscape* const, const HabitatDomainSource &source, const size_t newHabitatDomainPatchPriority)
{
    getMutableCellHabitatDomain().applySource(source, newHabitatDomainPatchPriority);
}

const WetMass& LeafPatchApplicator::getTotalResourceBiomass() const
{
    return totalResourceBiomass;
}

void LeafPatchApplicator::addResourceBiomass(const WetMass& biomassToAdd)
{
    totalResourceBiomass = totalResourceBiomass + biomassToAdd;
}

void LeafPatchApplicator::removeResourceBiomass(const WetMass& biomassToRemove)
{
    totalResourceBiomass = totalResourceBiomass - biomassToRemove;
}

bool LeafPatchApplicator::isTotalFullCapacity() const
{
    return totalFullCapacity;
}

void LeafPatchApplicator::setTotalFullCapacity(const bool newTotalFullCapacityValue)
{
    totalFullCapacity = newTotalFullCapacityValue;
}





BOOST_CLASS_EXPORT(LeafPatchApplicator)

template <class Archive>
void LeafPatchApplicator::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<SpatialTreePatchApplicator>(*this);

    ar & totalResourceBiomass;

    ar & totalFullCapacity;
}

// Specialisation
template void LeafPatchApplicator::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void LeafPatchApplicator::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void LeafPatchApplicator::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void LeafPatchApplicator::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
