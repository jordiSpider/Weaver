

#include "World/Map/SpatialTree/TerrainCells/RootTerrainCell.h"

using namespace std;
using json = nlohmann::json;



RootTerrainCell::RootTerrainCell(SpatialTreeInterface* const mapInterface)
    : BranchTerrainCell(nullptr, new PointSpatialTree(), mapInterface)
{
    
}


RootTerrainCell::~RootTerrainCell()
{

}


const BranchTerrainCell* const RootTerrainCell::getParent() const
{
    return nullptr;
}

BranchTerrainCell* const RootTerrainCell::getMutableParent() const
{
    return nullptr;
}

unique_ptr<FullCoverageAnimals> RootTerrainCell::getMutableAnimalsUp(const AnimalSearchParams &animalSearchParams)
{
    return make_unique<FullCoverageAnimals>();
}

unique_ptr<PartialCoverageAnimals> RootTerrainCell::getMutableAnimalsUp(
        function<bool(AnimalInterface&)> upChecker, const AnimalSearchParams &animalSearchParams)
{
    return make_unique<PartialCoverageAnimals>();
}

SearchableEdibles RootTerrainCell::getEdiblesOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const Ring &radiusArea, const EdibleSearchParams &edibleSearchParams)
{   
    auto intersection = Geometry::calculateIntersection(radiusArea, getEffectiveArea());

    if(Geometry::checkCoverage(intersection.get(), getEffectiveArea()) == Coverage::Full)
    {
        return getMutableEdiblesOnAllCell(TerrainCellInterface::Full, sourcePosition, radius, radiusArea, edibleSearchParams);
    }
    else
    {
        return getMutableEdiblesOnAllCell(TerrainCellInterface::Partial, sourcePosition, radius, radiusArea, edibleSearchParams);
    }
}

unique_ptr<vector<pair<TerrainCellInterface*, pair<TerrainCellInterface::TerrainCellCoverage, unique_ptr<Ring>>>>> RootTerrainCell::getNeighboursCellsOnRadius(
        const Ring &radiusArea, const unsigned int searchDepth)
{
    return getRadiusTerrainCells(radiusArea, searchDepth);
}

void RootTerrainCell::obtainWorldResourceBiomassAndAnimalsPopulation(vector<double> &worldResourceBiomass, vector<vector<unsigned int>> &worldAnimalsPopulation)
{
    for(const auto &resource : getResources())
    {
        if(resource != nullptr)
        {
            worldResourceBiomass[resource->getSpecies()->getResourceSpeciesId()] += resource->calculateWetMass();
        }
    }

    obtainWorldAnimalsPopulation(worldAnimalsPopulation);
}

void RootTerrainCell::substractBiomassUp(const unsigned int resourceSpeciesId, double dryMassToBeSubstracted)
{
    // Not to implement
}
