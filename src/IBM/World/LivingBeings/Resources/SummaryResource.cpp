
#include "IBM/World/LivingBeings/Resources/SummaryResource.h"

#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCellInterface.h"
#include "IBM/World/WorldInterface.h"
#include "IBM/World/Map/MapInterface.h"

using namespace std;



SummaryResource::SummaryResource(BranchTerrainCellInterface* const summaryTerrainCell)
    : ResourceInterface(summaryTerrainCell), summaryTerrainCell(summaryTerrainCell)
{

}

SummaryResource::SummaryResource(const unsigned int resourceSpeciesId, BranchTerrainCellInterface* const summaryTerrainCell)
    : ResourceInterface(
        summaryTerrainCell->getMapInterface().getWorldInterface()->getExistingResourceSpecies()[resourceSpeciesId], 
        summaryTerrainCell, 
        Instar(summaryTerrainCell->getMapInterface().getWorldInterface()->getExistingResourceSpecies()[resourceSpeciesId]->getNumberOfInstars()),
        0.0
      ),
      resourceSpeciesId(resourceSpeciesId),
      summaryTerrainCell(summaryTerrainCell)
{

}


SummaryResource::~SummaryResource()
{

}

void SummaryResource::update()
{
    const unsigned int resourceSpeciesId = getResourceSpeciesId();

    double sumBiomass = accumulate(
        summaryTerrainCell->getChildrenTerrainCells().cbegin(), summaryTerrainCell->getChildrenTerrainCells().cend(), 0.0,
        [resourceSpeciesId](double accumulated, const auto &child) {
            return accumulated + child->getBiomass(resourceSpeciesId);
        }
    );

    setBiomass(sumBiomass);

    bool allFullCapacity = accumulate(
        summaryTerrainCell->getChildrenTerrainCells().cbegin(), summaryTerrainCell->getChildrenTerrainCells().cend(), true,
        [resourceSpeciesId](bool accumulated, const auto &child) {
            if(child->resourceIsPresent(resourceSpeciesId))
            {
                return accumulated && child->getResource(resourceSpeciesId)->isFullCapacity();
            }
            else
            {
                return accumulated;
            }
        }
    );

    ResourceInterface::setFullCapacity(allFullCapacity);
}

const unsigned int SummaryResource::getResourceSpeciesId() const
{
    return resourceSpeciesId;
}

double SummaryResource::getResourceMaximumCapacity() const
{
    const unsigned int resourceSpeciesId = getResourceSpeciesId();

    return accumulate(
        summaryTerrainCell->getChildrenTerrainCells().cbegin(), summaryTerrainCell->getChildrenTerrainCells().cend(), 0.0,
        [resourceSpeciesId](double accumulated, const auto &child) {
            return accumulated + child->getResource(resourceSpeciesId)->getResourceMaximumCapacity();
        }
    );
}

bool SummaryResource::canSpread() const
{
    return false;
}

void SummaryResource::removeBiomass()
{
    ResourceInterface::removeBiomass();

    for(auto &child : summaryTerrainCell->getMutableChildrenTerrainCells())
    {
        child->getMutableResource(getResourceSpeciesId())->removeBiomass();
    }
}

double SummaryResource::substractBiomass(double dryMassToBeSubstracted) 
{ 
    double remainingDryMass = ResourceInterface::substractBiomass(dryMassToBeSubstracted);

    if(remainingDryMass > 0.0)
    {
        removeBiomass();
    }
    else
    {
        double dryMassRemainingToBeRemoved = dryMassToBeSubstracted;

        vector<SpatialTreeTerrainCellInterface*> availableCells;

        for(auto &child : summaryTerrainCell->getMutableChildrenTerrainCells())
        {
            if(child->resourceIsPresent(getResourceSpeciesId()))
            {
                if(child->getResource(getResourceSpeciesId())->calculateDryMass() > 0.0)
                {
                    availableCells.push_back(child);
                }
            }
        }

        while(dryMassRemainingToBeRemoved > 0.0 && !availableCells.empty())
        {
            const double dryMassPerChild = dryMassRemainingToBeRemoved / availableCells.size();

            dryMassRemainingToBeRemoved = 0.0;

            for(auto &child : availableCells)
            {
                dryMassRemainingToBeRemoved += child->getMutableResource(getResourceSpeciesId())->substractBiomass(dryMassPerChild);
            }

            availableCells.clear();
            for(auto &child : summaryTerrainCell->getMutableChildrenTerrainCells())
            {
                if(child->resourceIsPresent(getResourceSpeciesId()))
                {
                    if(child->getResource(getResourceSpeciesId())->calculateDryMass() > 0.0)
                    {
                        availableCells.push_back(child);
                    }
                }
            }
        }
    }

    return remainingDryMass;
}

double SummaryResource::substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea, const double &minimumDryMass)
{
    double remainingDryMass = ResourceInterface::substractBiomass(dryMassToBeSubstracted, perceptionArea, minimumDryMass);

    double dryMassRemainingToBeRemoved = dryMassToBeSubstracted;

    vector<tuple<SpatialTreeTerrainCellInterface*, Coverage, double>> childTerrainCells;

    for(auto &child : summaryTerrainCell->getMutableChildrenTerrainCells())
    {
        if(child->resourceIsPresent(getResourceSpeciesId()))
        {
            auto intersection = Geometry::calculateIntersection(*perceptionArea, child->getEffectiveArea());

            double percent = Geometry::calculateCoveragePercent(intersection.get(), child->getEffectiveArea());

            childTerrainCells.push_back(make_tuple(child, Geometry::checkCoverage(percent), child->getResource(getResourceSpeciesId())->calculateDryMass() * (1.0 - percent)));
        }
    }

    vector<tuple<SpatialTreeTerrainCellInterface*, Coverage, double>> availableCells;

    for(auto &childInfo : childTerrainCells)
    {
        SpatialTreeTerrainCellInterface* child;
        Coverage childCoverage;
        double childMinimumDryMass;

        tie(child, childCoverage, childMinimumDryMass) = childInfo;

        if(child->getResource(getResourceSpeciesId())->calculateDryMass() > childMinimumDryMass)
        {
            availableCells.push_back(childInfo);
        }
    }

    while(dryMassRemainingToBeRemoved > 0.0 && !availableCells.empty())
    {
        const double dryMassPerChild = dryMassRemainingToBeRemoved / availableCells.size();

        dryMassRemainingToBeRemoved = 0.0;

        for(auto &childInfo : availableCells)
        {
            SpatialTreeTerrainCellInterface* child;
            Coverage childCoverage;
            double childMinimumDryMass;

            tie(child, childCoverage, childMinimumDryMass) = childInfo;

            if(childCoverage == Coverage::Full)
            {
                dryMassRemainingToBeRemoved += child->getMutableResource(getResourceSpeciesId())->substractBiomass(dryMassPerChild);
            }
            else if(childCoverage >= Coverage::Partial)
            {
                dryMassRemainingToBeRemoved += child->getMutableResource(getResourceSpeciesId())->substractBiomass(dryMassPerChild, perceptionArea, childMinimumDryMass);
            }
        }

        availableCells.clear();
        for(auto &childInfo : childTerrainCells)
        {
            SpatialTreeTerrainCellInterface* child;
            Coverage childCoverage;
            double childMinimumDryMass;

            tie(child, childCoverage, childMinimumDryMass) = childInfo;

            if(child->getResource(getResourceSpeciesId())->calculateDryMass() > childMinimumDryMass)
            {
                availableCells.push_back(childInfo);
            }
        }
    }

    return remainingDryMass;
}

void SummaryResource::setFullCapacity(const bool newFullCapacityValue, const TerrainCellInterface* const source)
{
    auto childrenPositions = summaryTerrainCell->calculateChildrenPositions(source->getPosition());

    setFullCapacity(newFullCapacityValue, childrenPositions);
}

void SummaryResource::setFullCapacity(const bool newFullCapacityValue, const std::unique_ptr<std::vector<unsigned int>> &childrenPositions)
{
    for(const auto &childPosition : *childrenPositions)
    {
        if(summaryTerrainCell->getChildTerrainCell(childPosition)->resourceIsPresent(getResourceSpeciesId()))
        {
            summaryTerrainCell->getMutableChildTerrainCell(childPosition)->getMutableResource(getResourceSpeciesId())->setFullCapacity(newFullCapacityValue, childrenPositions);
        }
    }
}

double SummaryResource::addBiomass(double addedMass, const TerrainCellInterface* const source) 
{ 
    auto childrenPositions = summaryTerrainCell->calculateChildrenPositions(source->getPosition());

    return addBiomass(addedMass, childrenPositions);
};

double SummaryResource::addBiomass(double addedMass, const unique_ptr<vector<unsigned int>> &childrenPositions)
{
    vector<unsigned int> positionsAvailable;

    for(const auto &childPosition : *childrenPositions)
    {
        if(summaryTerrainCell->getChildTerrainCell(childPosition)->resourceIsPresent(getResourceSpeciesId()))
        {
            if(!summaryTerrainCell->getChildTerrainCell(childPosition)->getResource(resourceSpeciesId)->isFullCapacity())
            {
                positionsAvailable.push_back(childPosition);
            }
        }
    }

    if(!positionsAvailable.empty())
    {
        double excess = 0.0;

        const double massPerChild = addedMass / positionsAvailable.size();

        for(const auto &childPosition : positionsAvailable)
        {
            excess += summaryTerrainCell->getMutableChildTerrainCell(childPosition)->addResourceBiomass(massPerChild, getResourceSpeciesId(), childrenPositions);
        }

        return excess;
    }
    else
    {
        return addedMass;
    }
}

double SummaryResource::substractBiomass(double dryMassToBeSubstracted, const Ring* const perceptionArea)
{
    double remainingDryMass = ResourceInterface::substractBiomass(dryMassToBeSubstracted, perceptionArea);
    
    summaryTerrainCell->substractBiomassUp(getResourceSpeciesId(), dryMassToBeSubstracted - remainingDryMass);

	return remainingDryMass;
}

template <class Archive>
void SummaryResource::serialize(Archive &ar, const unsigned int version) {
    ar & boost::serialization::base_object<ResourceInterface>(*this);
    
    ar & resourceSpeciesId;
}

// Specialisation
template void SummaryResource::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SummaryResource::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SummaryResource::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SummaryResource::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
