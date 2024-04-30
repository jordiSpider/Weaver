

#include "IBM/World/Map/SpatialTree/TerrainCells/LeafTerrainCell.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "IBM/World/Map/SpatialTree.h"
#include "IBM/World/Map/TerrainCells/EdibleSearchParams.h"
#include "IBM/World/World.h"


using namespace std;
using json = nlohmann::json;
namespace bg = boost::geometry;




LeafTerrainCell::LeafTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, vector<ResourceInterface*>* const parentResources, const vector<int>* const parentResourcePatchPriority)
    : LeafTerrainCellInterface(parentTerrainCell, position, 
        static_cast<const SpatialTree&>(parentTerrainCell->getMap()).getCellSize(position->getDepth()), 
        &static_cast<SpatialTree&>(parentTerrainCell->getMutableMap()), 
        makeDefaultAnimals(&parentTerrainCell->getMutableMap()), parentTerrainCell->isFullObstacle(), 
        parentTerrainCell->isFullObstacle(), parentTerrainCell->getObstaclePatchPriority(), new NullMoisture(), false, false, -1, 0.0
      )
{
    if(parentTerrainCell->isInMoisturePatch())
    {
        if(parentTerrainCell->isMoistureSource())
        {
            setSubdivisionMoisturePatch(static_cast<ExtendedMoisture* const>(parentTerrainCell->getMutableMoistureInfo()), parentTerrainCell->getMoisturePatchPriority());
        }
        else
        {
            setSubdivisionMoisturePatch(
                static_cast<SubdivisionMoisture* const>(parentTerrainCell->getMutableMoistureInfo())->getMutableMoistureSource(), 
                parentTerrainCell->getMoisturePatchPriority()
            );
        }
    }

    if(parentResources != nullptr)
    {
        for(unsigned int resource = 0; resource < parentResources->size(); resource++)
        {
            if(parentResources->at(resource) != nullptr)
            {
                unique_ptr<ResourceInterface> newResource = ResourceFactory::createInstance(
                    parentResources->at(resource)->getMutableSpecies(), this, 
                    parentResources->at(resource)->getBiomass() / static_cast<const double>(BranchTerrainCell::numberOfChildren), 
                    parentResources->at(resource)->getResourceMaximumCapacity() / static_cast<const double>(BranchTerrainCell::numberOfChildren), 
                    getMap().getWorld()->getMassRatio(), parentResources->at(resource)->canSpread()
                );

                setResourceSourcePatch(newResource, parentResourcePatchPriority->at(resource));
            }
        }
    }
}


LeafTerrainCell::~LeafTerrainCell()
{
    
}


SpatialTreeTerrainCell* const& LeafTerrainCell::getChildTerrainCell(const PointSpatialTree &cellPos) const
{
    throwLineInfoException("Attempting to retrieve a nonexistent cell");
}

const bool LeafTerrainCell::isLeaf() const
{
    return true;
}

unique_ptr<FullCoverageAnimals> LeafTerrainCell::getMutableAnimalsDown(const AnimalSearchParams &animalSearchParams)
{
    return make_unique<FullCoverageAnimals>();
}

EdiblesOnRadius LeafTerrainCell::getMutableEdiblesOnCellAndDown(
        function<bool(Animal&)> downChecker, const Ring &effectiveArea, const EdibleSearchParams &edibleSearchParams)
{
    unique_ptr<ResourcesOnRadius> searchableResources;
    auto searchablePartialCoverageAnimals = make_unique<PartialCoverageAnimals>();
    auto searchableFullCoverageAnimals = make_unique<FullCoverageAnimals>();

    if(!isObstacle())
    {
        unique_ptr<AnimalVector> animalsOnCell;
        tie(searchableResources, animalsOnCell) = getMutableEdiblesBy(downChecker, effectiveArea, edibleSearchParams);
        searchablePartialCoverageAnimals->push_back(move(animalsOnCell));
    }
    else
    {
        searchableResources = make_unique<ResourcesOnRadius>();
    }

    return make_pair(move(searchableResources), make_pair(move(searchablePartialCoverageAnimals), move(searchableFullCoverageAnimals)));
}

unique_ptr<vector<pair<TerrainCellInterface*, pair<TerrainCellInterface::TerrainCellCoverage, unique_ptr<Ring>>>>> LeafTerrainCell::getRadiusTerrainCells(
        const Ring &radiusArea, const unsigned int searchDepth)
{
    auto radiusTerrainCells = make_unique<vector<pair<TerrainCellInterface*, pair<TerrainCellInterface::TerrainCellCoverage, unique_ptr<Ring>>>>>();

    if(static_cast<const PointSpatialTree&>(getPosition()).getDepth() == searchDepth)
    {
        auto intersection = Geometry::calculateIntersection(radiusArea, getEffectiveArea());

        auto coverage = Geometry::checkCoverage(intersection.get(), getEffectiveArea());

        if(coverage == Coverage::Full)
        {
            radiusTerrainCells->push_back(make_pair(this, make_pair(TerrainCellInterface::Full, move(intersection))));
        }
        else if(coverage >= Coverage::Partial)
        {
            radiusTerrainCells->push_back(make_pair(this, make_pair(TerrainCellInterface::Partial, move(intersection))));
        }
    }
    else
    {
        unsigned int depthDifference = searchDepth - static_cast<const PointSpatialTree&>(getPosition()).getDepth();

        vector<int> initialCoords(DIMENSIONS);

        for(unsigned int i = 0; i < DIMENSIONS; i++)
        {
            Axis axis = magic_enum::enum_cast<Axis>(i).value();

            initialCoords[i] = getPosition().get(axis) * displacementPower(1, depthDifference);
        }

        auto childPoints = static_cast<const SpatialTree &>(getMap()).generatePoints(displacementPower(1, depthDifference), initialCoords, searchDepth);

        for(auto &point : *childPoints)
        {
            PointContinuous minCorner;

            for(unsigned int i = 0; i < DIMENSIONS; i++)
            {
                Axis axis = magic_enum::enum_cast<Axis>(i).value();
                setPositionAxisValue(minCorner, i, point.get(axis) * size);
            }

            PointContinuous maxCorner = minCorner;
            boost::geometry::add_value(maxCorner, size);

            Box boxEffectiveArea(minCorner, maxCorner);

            Ring ringEffectiveArea;
            boost::geometry::convert(boxEffectiveArea, ringEffectiveArea);


            auto intersection = Geometry::calculateIntersection(radiusArea, ringEffectiveArea);

            auto coverage = Geometry::checkCoverage(intersection.get(), ringEffectiveArea);

            if(coverage == Coverage::Full)
            {
                radiusTerrainCells->push_back(make_pair(this, make_pair(TerrainCellInterface::Sub_Terrain_Cell, move(intersection))));
            }
            else if(coverage >= Coverage::Partial)
            {
                radiusTerrainCells->push_back(make_pair(this, make_pair(TerrainCellInterface::Partial, move(intersection))));
            }
        }
    }

    return radiusTerrainCells;
}

EdiblesOnRadius LeafTerrainCell::getMutableEdiblesDown(
        function<bool(Animal&)> downChecker, const Ring &effectiveArea, const EdibleSearchParams &edibleSearchParams)
{
    return make_pair(
        getResourcesBy(effectiveArea, edibleSearchParams.getResourceSearchParams()), 
        make_pair(make_unique<PartialCoverageAnimals>(), make_unique<FullCoverageAnimals>())
    );
}

SpatialTreeTerrainCellInterface* const LeafTerrainCell::getCell(const PointSpatialTree &cellPos)
{
    return this;
}

bool LeafTerrainCell::isChild(const PointSpatialTree &childPos) const
{
    return false;
}

bool LeafTerrainCell::applyObstaclePatch(const ObstaclePatch &obstaclePatch)
{
    auto coverage = obstaclePatch.checkCoverage(getEffectiveArea());

    if(coverage >= Coverage::Over50Percent)
    {
        setFullObstacle(obstaclePatch);
        return true;
    }
    else
    {
        return false;
    }
}


bool LeafTerrainCell::applyFullCoverageObstaclePatch(const ObstaclePatch &obstaclePatch)
{
    setFullObstacle(obstaclePatch);
    return true;
}


bool LeafTerrainCell::applyPartialCoverageObstaclePatch(const ObstaclePatch &obstaclePatch)
{
    return applyObstaclePatch(obstaclePatch);
}


pair<bool, bool> LeafTerrainCell::applyMoisturePatch(const MoisturePatch &moisturePatch)
{
    auto coverage = moisturePatch.checkCoverage(getEffectiveArea());

    if(coverage >= Coverage::Over50Percent)
    {
        return applyFullCoverageMoisturePatch(moisturePatch);
    }
    else
    {
        return make_pair(false, false);
    }
}


pair<bool, bool> LeafTerrainCell::applyPartialCoverageMoisturePatch(const MoisturePatch &moisturePatch)
{
    return applyMoisturePatch(moisturePatch);
}


const int LeafTerrainCell::getMaximumMoisturePatchPriority() const
{
    return getMoisturePatchPriority();
}


const int LeafTerrainCell::getMaximumResourcePatchPriority(const unsigned int resourceSpeciesId) const
{
    return getResourcePatchPriority(resourceSpeciesId);
}


bool LeafTerrainCell::applyResourcePatch(const ResourcePatch &resourcePatch)
{
    auto coverage = resourcePatch.checkCoverage(getEffectiveArea());

    if(coverage >= Coverage::Over50Percent)
    {
        applyFullCoverageResourcePatch(resourcePatch);
        return true;
    }
    else
    {
        return false;
    }
}


bool LeafTerrainCell::applyFullCoverageResourcePatch(const ResourcePatch &resourcePatch)
{
    setResourceSourcePatch(resourcePatch);
    return true;
}


bool LeafTerrainCell::applyPartialCoverageResourcePatch(const ResourcePatch &resourcePatch)
{
    return applyResourcePatch(resourcePatch);
}

void LeafTerrainCell::printCell(vector<pair<vector<double>, vector<unsigned int>>> &mapCellsInfo)
{
    if(!isObstacle())
    {
        unsigned int index = 0;
        for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
        {
            index += getPosition().getAxisValues().at(axis) * pow(getMap().getNumberOfCellsPerAxis(), axis);
        }

        for(unsigned int i = 0; i < getMap().getWorld()->getExistingResourceSpecies().size(); i++)
        {
            mapCellsInfo[index].first[i] += getBiomass(i);
        }

        for(auto &animalSpecies : getMap().getWorld()->getExistingAnimalSpecies())
        {
            unsigned int currentNumberOfAnimals = 0;

            auto population = getAnimalsBy(animalSpecies->getPopulationSearchParams());

            for(const auto &elem : *population)
            {
                currentNumberOfAnimals += elem->size();
            }

            mapCellsInfo[index].second[animalSpecies->getAnimalSpeciesId()] += currentNumberOfAnimals;
        }
    }
}
