

#include "IBM/World/Map/SpatialTree/TerrainCells/TemporalLeafTerrainCell.h"
#include "IBM/World/Map/SpatialTree/SpatialTreeInterface.h"
#include "IBM/World/WorldInterface.h"

using namespace std;
using json = nlohmann::json;



TemporalLeafTerrainCell::TemporalLeafTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, const vector<const ResourceInterface*>* const parentResources, const vector<int>* const parentResourcePatchPriority)
    : LeafTerrainCell(parentTerrainCell, position, parentResources, parentResourcePatchPriority)
{
    
}


TemporalLeafTerrainCell::~TemporalLeafTerrainCell()
{
    
}


void TemporalLeafTerrainCell::insertAnimal(AnimalInterface* const newAnimal)
{
    if(static_cast<const SpatialTreeInterface&>(getMapInterface()).getCellSize(getPosition().getDepth()+1) < newAnimal->getCurrentLength())
    {
        TerrainCell::insertAnimal(newAnimal);
    }
    else
    {
        throw SpatialTreeTerrainCell::TemporalLeaf2Branch();
    }
}


tuple<bool, TerrainCellInterface*, TerrainCellInterface*> TemporalLeafTerrainCell::randomInsertAnimal(AnimalInterface* const newAnimal)
{
    if(static_cast<const SpatialTreeInterface&>(getMapInterface()).getCellSize(getPosition().getDepth()+1) < newAnimal->getCurrentLength())
    {
        return TerrainCell::randomInsertAnimal(newAnimal);
    }
    else
    {
        throw SpatialTreeTerrainCell::TemporalLeaf2Branch();
    }
}


bool TemporalLeafTerrainCell::applyObstaclePatch(const ObstaclePatch &obstaclePatch)
{
    auto coverage = obstaclePatch.checkCoverage(TemporalLeafTerrainCell::getEffectiveArea());

    if(coverage == Coverage::Full)
    {
        setFullObstacle(obstaclePatch);
        return true;
    }
    else if(coverage >= Coverage::Partial)
    {
        throw SpatialTreeTerrainCell::TemporalLeaf2Branch();
    }
    else
    {
        return false;
    }
}


pair<bool, bool> TemporalLeafTerrainCell::applyMoisturePatch(const MoisturePatch &moisturePatch)
{
    auto coverage = moisturePatch.checkCoverage(TemporalLeafTerrainCell::getEffectiveArea());

    if(coverage == Coverage::Full)
    {
        return applyFullCoverageMoisturePatch(moisturePatch);
    }
    else if(coverage >= Coverage::Partial)
    {
        throw SpatialTreeTerrainCell::TemporalLeaf2Branch();
    }
    else
    {
        return make_pair(false, false);
    }
}


bool TemporalLeafTerrainCell::applyResourcePatch(const ResourcePatch &resourcePatch)
{
    auto coverage = resourcePatch.checkCoverage(getEffectiveArea());

    if(coverage == Coverage::Full)
    {
        applyFullCoverageResourcePatch(resourcePatch);
        return true;
    }
    else if(coverage >= Coverage::Partial)
    {
        throw SpatialTreeTerrainCell::TemporalLeaf2Branch();
    }
    else
    {
        return false;
    }
}

void TemporalLeafTerrainCell::printCell(vector<pair<vector<double>, vector<unsigned int>>> &mapCellsInfo)
{
    if(!isObstacle())
    {
        for(auto &animalSpecies : getMapInterface().getWorldInterface()->getExistingAnimalSpecies())
        {
            auto population = getAnimalsBy(animalSpecies->getPopulationSearchParams());

            for(const auto &elem : *population)
            {
                for(const auto &animal : *elem)
                {
                    PointMap animalPosOnLeaf = getMapInterface().obtainPointMap(
                        animal->getPosition(), static_cast<const SpatialTreeInterface &>(getMapInterface()).getMapDepth()-1
                    );

                    unsigned int index = 0;
                    for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
                    {
                        index += animalPosOnLeaf.getAxisValues().at(axis) * pow(getMapInterface().getNumberOfCellsPerAxis(), axis);
                    }

                    mapCellsInfo[index].second[animalSpecies->getAnimalSpeciesId()] += 1;
                }
            }
        }


        const unsigned int depthDifference = static_cast<const SpatialTreeInterface &>(getMapInterface()).getMapDepth()-1 - getPosition().getDepth();

        vector<int> initialCoords(DIMENSIONS);
        for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
        {
            initialCoords[axis] = getPosition().getAxisValues().at(axis) * pow(SpatialTreeInterface::numbreOfSubdivisions, depthDifference);
        }

        auto points = static_cast<const SpatialTreeInterface &>(getMapInterface()).generatePoints(pow(SpatialTreeInterface::numbreOfSubdivisions, depthDifference), initialCoords, static_cast<const SpatialTreeInterface &>(getMapInterface()).getMapDepth()-1);

        vector<unsigned int> pointsIndex(points->size());
        for(unsigned int i = 0; i < points->size(); i++)
        {
            unsigned int index = 0;
            for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
            {
                index += points->at(i).getAxisValues().at(axis) * pow(getMapInterface().getNumberOfCellsPerAxis(), axis);
            }

            pointsIndex[i] = index;
        }

        for(unsigned int i = 0; i < getMapInterface().getWorldInterface()->getExistingResourceSpecies().size(); i++)
        {
            double resourceOnPoint = getBiomass(i) / points->size();

            for(const auto &index : pointsIndex)
            {
                mapCellsInfo[index].first[i] += resourceOnPoint;
            }
        }
    }
}
