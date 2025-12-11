
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/PatchApplicator.h"

#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"
#include "App/Model/IBM/Landscape/Landscape.h"


using namespace std;



PatchApplicator::PatchApplicator()
{

}

PatchApplicator::PatchApplicator(
        TerrainCell* cell, CellMoistureInterface* cellMoisture, CellObstacleInterface* cellObstacle, 
        CellHabitatDomainInterface* cellHabitatDomain
    )
    : owner(cell), cellMoisture(cellMoisture), cellObstacle(cellObstacle), cellHabitatDomain(cellHabitatDomain)
{

}


PatchApplicator::~PatchApplicator()
{
    delete cellMoisture;

    for(auto resource : cellResources)
    {
        delete resource;
    }

    delete cellObstacle;

    delete cellHabitatDomain;
}


const CellMoistureInterface& PatchApplicator::getCellMoisture() const
{
    return *cellMoisture;
}

CellMoistureInterface& PatchApplicator::getMutableCellMoisture()
{
    return *cellMoisture;
}


const CellResourceInterface& PatchApplicator::getCellResource(const size_t resourceSpeciesId) const
{
    return *cellResources[resourceSpeciesId];
}

CellResourceInterface& PatchApplicator::getMutableCellResource(const size_t resourceSpeciesId)
{
    return *cellResources[resourceSpeciesId];
}


size_t PatchApplicator::getNumberOfResources() const
{
    return cellResources.size();
}


const CellObstacleInterface& PatchApplicator::getCellObstacle() const
{
    return *cellObstacle;
}

CellObstacleInterface& PatchApplicator::getMutableCellObstacle()
{
    return *cellObstacle;
}


const CellHabitatDomainInterface& PatchApplicator::getCellHabitatDomain() const
{
    return *cellHabitatDomain;
}

CellHabitatDomainInterface& PatchApplicator::getMutableCellHabitatDomain()
{
    return *cellHabitatDomain;
}


void PatchApplicator::setOwner(TerrainCell* cell)
{
    owner = cell;

    for(size_t i = 0; i < getNumberOfResources(); i++)
    {
        getMutableCellResource(i).setTerrainCell(cell);
    }
}




void PatchApplicator::deserializeSpecies(std::vector<ResourceSpecies*>& existingResourceSpecies)
{
    for(size_t i = 0; i < existingResourceSpecies.size(); i++)
    {
        getMutableCellResource(i).setSpecies(existingResourceSpecies[i]);
    }
}

void PatchApplicator::deserializeSources(
        vector<pair<size_t, MoistureSource*>>& appliedMoisture,
        std::vector<vector<pair<size_t, ResourceSource*>>>& appliedResource
    )
{
    getMutableCellMoisture().deserializeCellMoisture(appliedMoisture, owner);

    for(size_t i = 0; i < getNumberOfResources(); i++)
    {
        getMutableCellResource(i).deserializeCellResource(appliedResource[i]);
    }
}

size_t PatchApplicator::getCellResourcePatchPriority(const size_t resourceSpeciesId) const
{
    return getCellResource(resourceSpeciesId).getPatchPriority();
}

void PatchApplicator::update()
{
    getMutableCellMoisture().update();

    for(size_t i = 0; i < getNumberOfResources(); i++)
    {
        getMutableCellResource(i).update();
    }
}

void PatchApplicator::addAnimalSpecies(const AnimalSpecies& animalSpecies)
{
    getMutableCellHabitatDomain().addAnimalSpecies(animalSpecies);
}


bool PatchApplicator::canApplyFullCoverage(const Patch &patch) const
{
    switch(patch.getSource().getType()) {
        case PathSource::Type::Moisture: {
            return canApplyFullCoverageMoisture(patch.getPriority());
            break;
        }
        case PathSource::Type::Obstacle: {
            return canApplyFullCoverageObstacle(patch.getPriority());
            break;
        }
        case PathSource::Type::Resource: {
            return canApplyFullCoverageResource(static_cast<const ResourceSource&>(patch.getSource()), patch.getPriority());
            break;
        }
        case PathSource::Type::HabitatDomain: {
            return canApplyFullCoverageHabitatDomain(patch.getPriority());
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}

bool PatchApplicator::canApplyFullCoverageMoisture(const size_t patchPriority) const
{
    return patchPriority > getCellMoisture().getPatchPriority();
}

bool PatchApplicator::canApplyFullCoverageResource(const ResourceSource &source, const size_t patchPriority) const
{
    return !getCellObstacle().isObstacle() && 
            patchPriority > getCellResource(source.getResourceSpeciesId()).getPatchPriority();
}

bool PatchApplicator::canApplyFullCoverageObstacle(const size_t patchPriority) const
{
    return patchPriority > getCellObstacle().getPatchPriority();
}

bool PatchApplicator::canApplyFullCoverageHabitatDomain(const size_t patchPriority) const
{
    return patchPriority > getCellHabitatDomain().getPatchPriority();
}


void PatchApplicator::setCellElement(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority)
{
    switch(source.getType()) {
        case PathSource::Type::Moisture: {
            setCellMoisture(landscape, static_cast<const MoistureSource&>(source), newPatchPriority);
            break;
        }
        case PathSource::Type::Obstacle: {
            setCellObstacle(landscape, static_cast<const ObstacleSource&>(source), newPatchPriority);
            break;
        }
        case PathSource::Type::Resource: {
            setCellResource(landscape, static_cast<const ResourceSource&>(source), newPatchPriority);
            break;
        }
        case PathSource::Type::HabitatDomain: {
            setCellHabitatDomain(landscape, static_cast<const HabitatDomainSource&>(source), newPatchPriority);
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(PatchApplicator)

template <class Archive>
void PatchApplicator::serialize(Archive& ar, const unsigned int) {
    ar & cellMoisture;

    ar & cellResources;

    ar & cellObstacle;

    ar & cellHabitatDomain;
}

// Specialisation
template void PatchApplicator::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PatchApplicator::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PatchApplicator::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PatchApplicator::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
