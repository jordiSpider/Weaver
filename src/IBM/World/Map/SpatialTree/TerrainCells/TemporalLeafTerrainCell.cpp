

#include "IBM/World/Map/SpatialTree/TerrainCells/TemporalLeafTerrainCell.h"
#include "IBM/World/Map/SpatialTree.h"
#include "IBM/World/World.h"

using namespace std;
using json = nlohmann::json;



TemporalLeafTerrainCell::TemporalLeafTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, vector<ResourceInterface*>* const parentResources, const vector<int>* const parentResourcePatchPriority)
    : LeafTerrainCell(parentTerrainCell, position, parentResources, parentResourcePatchPriority)
{
    
}


TemporalLeafTerrainCell::~TemporalLeafTerrainCell()
{
    
}


void TemporalLeafTerrainCell::insertAnimal(Animal* const newAnimal)
{
    if(getPosition().getDepth() == newAnimal->getSpecies()->getCellDepthPerInstar()[newAnimal->getInstar()])
    {
        TerrainCell::insertAnimal(newAnimal);
    }
    else
    {
        throw SpatialTreeTerrainCell::TemporalLeaf2Branch();
    }
}


tuple<bool, TerrainCellInterface*, TerrainCellInterface*, Animal*, unsigned int> TemporalLeafTerrainCell::randomInsertAnimal(const Instar &instar, AnimalSpecies* animalSpecies, const bool isStatistical)
{
    if(getPosition().getDepth() == animalSpecies->getCellDepthPerInstar()[instar])
    {
        return TerrainCell::randomInsertAnimal(instar, animalSpecies, isStatistical);
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
        for(auto &animalSpecies : getMap().getWorld()->getExistingAnimalSpecies())
        {
            auto population = getAnimalsBy(animalSpecies->getPopulationSearchParams());

            for(const auto &elem : *population)
            {
                for(const auto &animal : *elem)
                {
                    PointMap animalPosOnLeaf = getMap().obtainPointMap(
                        animal->getPosition(), static_cast<const SpatialTree &>(getMap()).getMapDepth()-1
                    );

                    unsigned int index = 0;
                    for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
                    {
                        index += animalPosOnLeaf.getAxisValues().at(axis) * pow(getMap().getNumberOfCellsPerAxis(), axis);
                    }

                    mapCellsInfo[index].second[animalSpecies->getAnimalSpeciesId()] += 1;
                }
            }
        }


        const unsigned int depthDifference = static_cast<const SpatialTree &>(getMap()).getMapDepth()-1 - getPosition().getDepth();

        vector<int> initialCoords(DIMENSIONS);
        for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
        {
            initialCoords[axis] = getPosition().getAxisValues().at(axis) * pow(SpatialTree::numbreOfSubdivisions, depthDifference);
        }

        auto points = static_cast<const SpatialTree &>(getMap()).generatePoints(pow(SpatialTree::numbreOfSubdivisions, depthDifference), initialCoords, static_cast<const SpatialTree &>(getMap()).getMapDepth()-1);

        vector<unsigned int> pointsIndex(points->size());
        for(unsigned int i = 0; i < points->size(); i++)
        {
            unsigned int index = 0;
            for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
            {
                index += points->at(i).getAxisValues().at(axis) * pow(getMap().getNumberOfCellsPerAxis(), axis);
            }

            pointsIndex[i] = index;
        }

        for(unsigned int i = 0; i < getMap().getWorld()->getExistingResourceSpecies().size(); i++)
        {
            double resourceOnPoint = getBiomass(i) / points->size();

            for(const auto &index : pointsIndex)
            {
                mapCellsInfo[index].first[i] += resourceOnPoint;
            }
        }
    }
}
