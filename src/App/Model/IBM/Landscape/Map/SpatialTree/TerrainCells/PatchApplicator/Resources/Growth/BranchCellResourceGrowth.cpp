
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Resources/Growth/BranchCellResourceGrowth.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResourceInterface.h"


using namespace std;



BranchCellResourceGrowth::BranchCellResourceGrowth()
    : CellResourceInterfaceGrowth()
{

}

BranchCellResourceGrowth::BranchCellResourceGrowth(CellResourceInterface* owner, ResourceSpeciesGrowth* const newSpeciesGrowth)
    : CellResourceInterfaceGrowth(owner, newSpeciesGrowth), minimumEdibleBiomass(0.0)
{

}


BranchCellResourceGrowth::~BranchCellResourceGrowth()
{

}


void BranchCellResourceGrowth::applyBranch()
{
    update();

    minimumEdibleBiomass = accumulate(
        static_cast<const BranchTerrainCell* const>(getOwner()->getTerrainCell())->getChildrenTerrainCells().cbegin(), static_cast<const BranchTerrainCell* const>(getOwner()->getTerrainCell())->getChildrenTerrainCells().cend(), WetMass(0.0),
        [this](WetMass accumulated, const auto &child) {
            return accumulated + child->getPatchApplicator().getCellResource(getOwner()->getSpecies()->getResourceSpeciesId()).getGrowthBuildingBlock().getMinimumEdibleBiomass();
        }
    );
}


void BranchCellResourceGrowth::update()
{
    WetMass sumBiomass = accumulate(
        static_cast<const BranchTerrainCell* const>(getOwner()->getTerrainCell())->getChildrenTerrainCells().cbegin(), static_cast<const BranchTerrainCell* const>(getOwner()->getTerrainCell())->getChildrenTerrainCells().cend(), WetMass(0.0),
        [this](WetMass accumulated, const auto &child) {
            return accumulated + child->getPatchApplicator().getCellResource(getOwner()->getSpecies()->getResourceSpeciesId()).getGrowthBuildingBlock().getCurrentTotalWetMass();
        }
    );

    setCurrentTotalWetMass(sumBiomass);
}


const WetMass& BranchCellResourceGrowth::getMinimumEdibleBiomass() const
{
    return minimumEdibleBiomass;
}



BOOST_CLASS_EXPORT(BranchCellResourceGrowth)

template <class Archive>
void BranchCellResourceGrowth::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<CellResourceInterfaceGrowth>(*this);

    ar & minimumEdibleBiomass;
}

// Specialisation
template void BranchCellResourceGrowth::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void BranchCellResourceGrowth::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void BranchCellResourceGrowth::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void BranchCellResourceGrowth::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
