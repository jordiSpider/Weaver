
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/HabitatDomain/BranchCellHabitatDomain.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"

using namespace std;




size_t BranchCellHabitatDomain::calculateHabitatDomainPatchPriority(BranchTerrainCell* const summaryTerrainCell)
{
    size_t habitatDomainPatchPriority = 0u;

    for(const auto &child : summaryTerrainCell->getChildrenTerrainCells())
    {
        if(habitatDomainPatchPriority < child->getPatchApplicator().getCellHabitatDomain().getPatchPriority())
        {
            habitatDomainPatchPriority = child->getPatchApplicator().getCellHabitatDomain().getPatchPriority();
        }
    }

    return habitatDomainPatchPriority;
}



BranchCellHabitatDomain::BranchCellHabitatDomain()
    : CellHabitatDomainInterface()
{
    
}


BranchCellHabitatDomain::BranchCellHabitatDomain(const size_t newHabitatDomainPatchPriority)
    : CellHabitatDomainInterface(newHabitatDomainPatchPriority)
{

}


BranchCellHabitatDomain::~BranchCellHabitatDomain()
{

}


void BranchCellHabitatDomain::applyBranch(BranchTerrainCell* const summaryTerrainCell)
{
    setPatchPriority(calculateHabitatDomainPatchPriority(summaryTerrainCell));

    for(size_t idIndex = 0; idIndex < habitatDomainStatus.size(); idIndex++)
    {
        AnimalSpeciesID id(idIndex);

        for(size_t instarIndex = 0; instarIndex < habitatDomainStatus[id].size(); instarIndex++)
        {
            Instar instar = Instar::convertToIndex(static_cast<unsigned int>(instarIndex));

            size_t numberOfHabitatDomain = 0u;

            for(const auto &child : summaryTerrainCell->getChildrenTerrainCells())
            {
                numberOfHabitatDomain += (child->getPatchApplicator().getCellHabitatDomain().getHabitatDomainStatus().at(id).at(instar)) ? 1u : 0u;
            }

            habitatDomainStatus[id][instar] = PreciseDouble(static_cast<double>(numberOfHabitatDomain))/PreciseDouble(static_cast<double>(summaryTerrainCell->getChildrenTerrainCells().size())) > 0.5;
        }
    }
}



BOOST_CLASS_EXPORT(BranchCellHabitatDomain)

template <class Archive>
void BranchCellHabitatDomain::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<CellHabitatDomainInterface>(*this);
}

// Specialisation
template void BranchCellHabitatDomain::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void BranchCellHabitatDomain::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void BranchCellHabitatDomain::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void BranchCellHabitatDomain::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
