#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Resources/BranchCellResource.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Landscape/Map/Map.h"

using namespace std;




size_t BranchCellResource::calculateResourcePatchPriority(const BranchTerrainCell* const summaryTerrainCell, const ResourceSpecies::ResourceID& resourceSpeciesId)
{
    size_t resourcePatchPriority = 0u;

    for(const auto &child : summaryTerrainCell->getChildrenTerrainCells())
    {
        if(!child->getPatchApplicator().getCellObstacle().isFullObstacle())
        {
            if(resourcePatchPriority < child->getPatchApplicator().getCellResource(resourceSpeciesId).getPatchPriority())
            {
                resourcePatchPriority = child->getPatchApplicator().getCellResource(resourceSpeciesId).getPatchPriority();
            }
        }
    }

    return resourcePatchPriority;
}



BranchCellResource::BranchCellResource()
    : CellResourceInterface()
{
    
}

BranchCellResource::BranchCellResource(BranchTerrainCell* const summaryTerrainCell, const size_t newResourcePatchPriority, ResourceSpecies& resourceSpecies)
    : CellResourceInterface(id_type(0), summaryTerrainCell, newResourcePatchPriority, resourceSpecies),
      growthBuildingBlock(this, &resourceSpecies.getMutableGrowthBuildingBlock())
{
    
}


BranchCellResource::~BranchCellResource()
{

}

const BranchCellResourceGrowth& BranchCellResource::getGrowthBuildingBlock() const
{
	return growthBuildingBlock;
}

BranchCellResourceGrowth& BranchCellResource::getMutableGrowthBuildingBlock()
{
	return growthBuildingBlock;
}

void BranchCellResource::applyBranch()
{
    setPatchPriority(BranchCellResource::calculateResourcePatchPriority(static_cast<const BranchTerrainCell* const>(getTerrainCell()), getSpecies()->getResourceSpeciesId()));

    getMutableGrowthBuildingBlock().applyBranch();
}

void BranchCellResource::update()
{
    getMutableGrowthBuildingBlock().update();
}

void BranchCellResource::setTerrainCell(TerrainCell* newTerrainCell)
{
    Edible::setTerrainCell(newTerrainCell);
}

void BranchCellResource::deserializeCellResource(std::vector<std::pair<size_t, ResourceSource*>>&)
{

}

void BranchCellResource::setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth)
{
	growthBuildingBlock.setSpeciesGrowth(newSpeciesGrowth);
}

DryMass BranchCellResource::calculateDryMassAvailable(const bool fullCoverage, const PointContinuous* const sourcePosition, const PreciseDouble &radius) const
{
	DryMass dryMassAvailable(0.0);

    if(fullCoverage)
    {
        dryMassAvailable = getGrowthBuildingBlock().getCurrentTotalDryMass() - DryMass(getGrowthBuildingBlock().getMinimumEdibleBiomass(), getSpecies()->getGrowthBuildingBlock().getConversionToWetMass(getGrowthBuildingBlock().getInstar()));
    }
    else
    {
        for(auto &child : static_cast<BranchTerrainCell*>(getMutableTerrainCell())->getMutableChildrenTerrainCells())
        {
            bool childFullCoveraged = fullCoverage;

            if(!childFullCoveraged)
            {
                childFullCoveraged = Geometry::fullCoveredBySphere(child->getEffectiveArea(), *sourcePosition, radius);
            }

            dryMassAvailable = dryMassAvailable + child->getPatchApplicator().getCellResource(getSpecies()->getResourceSpeciesId()).calculateDryMassAvailable(childFullCoveraged, sourcePosition, radius);
        }
    }

    dryMassAvailable = DryMass(fmax(0.0, dryMassAvailable.getValue()));

	return dryMassAvailable;
}

void BranchCellResource::substractBiomass(const DryMass& dryMassToBeSubstracted, bool fullCoverage, const PointContinuous &sourcePosition, const PreciseDouble &radius, const bool competitionAmongResourceSpecies)
{
    substractBiomassDown(dryMassToBeSubstracted, fullCoverage, sourcePosition, radius, competitionAmongResourceSpecies);
    

    if(static_cast<const SpatialTreeTerrainCell*>(getTerrainCell())->getParent() != nullptr)
    {
		static_cast<BranchCellResource&>(static_cast<BranchTerrainCell*>(getMutableTerrainCell())->getMutableParent()->getMutablePatchApplicator().getMutableCellResource(getSpecies()->getResourceSpeciesId())).substractBiomassUp(dryMassToBeSubstracted);
	}
}

void BranchCellResource::substractBiomassDown(const DryMass& dryMassToBeSubstracted, const bool fullCoverage, const PointContinuous &sourcePosition, const PreciseDouble &radius, const bool competitionAmongResourceSpecies)
{
    DryMass totalDryMassAvailable = calculateDryMassAvailable(fullCoverage, &sourcePosition, radius);


    for(auto &child : static_cast<BranchTerrainCell*>(getMutableTerrainCell())->getMutableChildrenTerrainCells())
    {
        if(child->getPatchApplicator().getCellResource(getSpecies()->getResourceSpeciesId()).getGrowthBuildingBlock().getCurrentTotalWetMass() > 0.0)
        {
            bool childFullCoverage = fullCoverage;

            if(!childFullCoverage)
            {
                childFullCoverage = Geometry::fullCoveredBySphere(child->getEffectiveArea(), sourcePosition, radius);
            }


            DryMass childDryMassAvailable = child->getPatchApplicator().getCellResource(getSpecies()->getResourceSpeciesId()).calculateDryMassAvailable(childFullCoverage, &sourcePosition, radius);


            if(childDryMassAvailable > 0.0)
            {
                const PreciseDouble percentage = childDryMassAvailable.getValue() / totalDryMassAvailable.getValue();

                child->getMutablePatchApplicator().getMutableCellResource(getSpecies()->getResourceSpeciesId()).substractBiomassDown(DryMass(dryMassToBeSubstracted.getValue()*percentage), childFullCoverage, sourcePosition, radius, competitionAmongResourceSpecies);
            }
        }
    }

    
    getMutableGrowthBuildingBlock().substractBiomass(dryMassToBeSubstracted);
}

void BranchCellResource::substractBiomassUp(const DryMass& dryMassToBeSubstracted)
{
    getMutableGrowthBuildingBlock().substractBiomass(dryMassToBeSubstracted);

    if(static_cast<const SpatialTreeTerrainCell*>(getTerrainCell())->getParent() != nullptr)
    {
		static_cast<BranchCellResource&>(static_cast<BranchTerrainCell*>(getMutableTerrainCell())->getMutableParent()->getMutablePatchApplicator().getMutableCellResource(getSpecies()->getResourceSpeciesId())).substractBiomassUp(dryMassToBeSubstracted);
	}
}




BOOST_CLASS_EXPORT(BranchCellResource)

template <class Archive>
void BranchCellResource::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<CellResourceInterface>(*this);

    ar & growthBuildingBlock;

    if (Archive::is_loading::value)
	{
        growthBuildingBlock.setOwner(this);
    }
}

// Specialisation
template void BranchCellResource::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void BranchCellResource::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void BranchCellResource::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void BranchCellResource::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
