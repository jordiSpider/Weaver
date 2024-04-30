

#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"
#include "IBM/World/Map/SpatialTree/SpatialTreeInterface.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCellInterface.h"
#include "IBM/World/Map/TerrainCells/EdibleSearchParams.h"

using namespace std;



const unsigned int SpatialTreeTerrainCell::numberOfChildren = std::pow(SpatialTreeInterface::numbreOfSubdivisions, DIMENSIONS);


SpatialTreeTerrainCell::SpatialTreeTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, 
        PointSpatialTree* const position, const Ring *const effectiveArea, const double &size, SpatialTreeInterface* const mapInterface,
        LifeStageVector* const animals, 
        const bool obstacle, const bool fullObstacle, const int obstaclePatchPriority, MoistureInterface* const moistureInfo, 
        const bool moistureSource, const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity)
    : SpatialTreeTerrainCellInterface(position, effectiveArea, size, mapInterface,
        animals,
        obstacle, fullObstacle, obstaclePatchPriority, moistureInfo, moistureSource, inMoisturePatch, moisturePatchPriority, totalMaximumResourceCapacity
      ),
      parentTerrainCell(parentTerrainCell)
{

}

SpatialTreeTerrainCell::~SpatialTreeTerrainCell()
{
    
}


const BranchTerrainCellInterface* const SpatialTreeTerrainCell::getParent() const
{
    return parentTerrainCell;
}

BranchTerrainCellInterface* const SpatialTreeTerrainCell::getMutableParent() const
{
    return parentTerrainCell;
}


bool SpatialTreeTerrainCell::checkFullCoverageObstaclePatch(const ObstaclePatch &obstaclePatch)
{
    if(canApplyObstaclePatch(obstaclePatch))
    {
        return applyFullCoverageObstaclePatch(obstaclePatch);
    }

    return false;
}


pair<bool, bool> SpatialTreeTerrainCell::checkFullCoverageMoisturePatch(const MoisturePatch &moisturePatch)
{
    if(canApplyMoisturePatch(moisturePatch))
    {
        return applyFullCoverageMoisturePatch(moisturePatch);
    }

    return make_pair(false, false);
}

pair<bool, bool> SpatialTreeTerrainCell::applyFullCoverageMoisturePatch(const MoisturePatch &moisturePatch)
{
    setMoistureSourcePatch(moisturePatch);
    return make_pair(true, true);
}


bool SpatialTreeTerrainCell::canApplyMoistureSourcePatch(const int moisturePathPriority)
{
    return moisturePathPriority == getMoisturePatchPriority();
}


void SpatialTreeTerrainCell::checkMoistureSourcePatch(const int moisturePathPriority)
{
    if(canApplyMoistureSourcePatch(moisturePathPriority))
    {
        applyMoistureSourcePatch();
    }
}


void SpatialTreeTerrainCell::applyMoistureSourcePatch()
{
    setMoistureSourcePatch(static_cast<SubdivisionMoisture* const>(getMutableMoistureInfo())->getMutableMoistureSource(), getMoisturePatchPriority());
}


void SpatialTreeTerrainCell::setSubdivisionMoisturePatch(const MoisturePatch &moisturePatch)
{
    setSubdivisionMoisturePatch(moisturePatch.getMoistureInfo().get(), moisturePatch.getPriority());
}

void SpatialTreeTerrainCell::setSubdivisionMoisturePatch(ExtendedMoisture* const moistureInfo, const int moisturePathPriority)
{
    setMoisturePatch(new SubdivisionMoisture(moistureInfo), moisturePathPriority, false, true);
}

bool SpatialTreeTerrainCell::canApplyFullCoverageResourcePatch(const ResourcePatch &resourcePatch) const
{
    return !this->isFullObstacle();
}

bool SpatialTreeTerrainCell::checkFullCoverageResourcePatch(const ResourcePatch &resourcePatch)
{
    if(canApplyFullCoverageResourcePatch(resourcePatch))
    {
        return applyFullCoverageResourcePatch(resourcePatch);
    }

    return false;
}

pair<bool, pair<TerrainCellInterface *, PointContinuous>> SpatialTreeTerrainCell::getCellByBearing(
        const pair<PointMap, PointContinuous> &targetNeighborToTravelTo, const PointContinuous &animalPosition)
{
    if(getPosition().getDepth() > 0)
    {
        vector<pair<unsigned int, int>> movementAxes;
        for(unsigned int i = 0; i < DIMENSIONS; i++)
        {
            Axis axis = magic_enum::enum_cast<Axis>(i).value();

            if(targetNeighborToTravelTo.first.get(axis) > getPosition().get(axis))
            {
                movementAxes.push_back(make_pair(i, 1));
            }
            else if(targetNeighborToTravelTo.first.get(axis) < getPosition().get(axis))
            {
                movementAxes.push_back(make_pair(i, -1));
            }
        }

        vector<unsigned int> axisValues = getPosition().getAxisValues();
        
        if(!movementAxes.empty())
        {
            unsigned int randomIndex = Random::randomIndex(movementAxes.size());
            axisValues[movementAxes[randomIndex].first] += movementAxes[randomIndex].second;
        }

        PointSpatialTree nextCellPosition(axisValues, getPosition().getDepth());
        TerrainCellInterface* nextCell = static_cast<const SpatialTreeInterface &>(getMapInterface()).getCell(nextCellPosition, this);

        //Avoid obstacles using the least bearing difference path
        if(nextCell->isObstacle())
        {
            vector<int> initialCoords(DIMENSIONS);

            for(unsigned int i = 0; i < DIMENSIONS; i++)
            {
                Axis axis = magic_enum::enum_cast<Axis>(i).value();

                initialCoords[i] = getPosition().get(axis) - 1;
            }

            auto neighboursPoints = static_cast<const SpatialTreeInterface &>(getMapInterface()).generatePoints(3, getPosition(), initialCoords);

            unsigned int bestBearingDifference = calculateManhattanDistanceToPoint(targetNeighborToTravelTo.first) + 2;
            vector<pair<TerrainCellInterface*, PointSpatialTree>> surroundingTerrainCells;

            for(unsigned int i = 0; i < neighboursPoints->size(); i++)
            {
                if(calculateManhattanDistanceToPoint(neighboursPoints->at(i)) == 1)
                {
                    auto neighbour = static_cast<const SpatialTreeInterface &>(getMapInterface()).getCell(neighboursPoints->at(i), this);
                
                    if(!neighbour->isObstacle())
                    {
                        unsigned int bearingDifference = neighbour->calculateManhattanDistanceToPoint(targetNeighborToTravelTo.first);

                        if(bearingDifference < bestBearingDifference)
                        {
                            bestBearingDifference = bearingDifference;
                            surroundingTerrainCells.clear();
                            surroundingTerrainCells.push_back(make_pair(neighbour, neighboursPoints->at(i)));
                        }
                        else if(bearingDifference == bestBearingDifference)
                        {
                            surroundingTerrainCells.push_back(make_pair(neighbour, neighboursPoints->at(i)));
                        }
                    }
                }  
            }

            if(surroundingTerrainCells.empty())
            {
                throwLineInfoException("The animal is immobile");
            }

            auto index = Random::randomIndex(surroundingTerrainCells.size());

            nextCell = surroundingTerrainCells[index].first;
            nextCellPosition = surroundingTerrainCells[index].second;
        }


        if(static_cast<PointMap>(nextCellPosition) == targetNeighborToTravelTo.first)
        {
            return make_pair(true, make_pair(nextCell, targetNeighborToTravelTo.second));
        }
        else
        {
            auto boxEffectiveArea = Geometry::makeBoxEffectiveArea(nextCellPosition, getSize());

            Ring ringEffectiveArea;
            boost::geometry::convert(*boxEffectiveArea, ringEffectiveArea);

            PointContinuous closestPoint = Geometry::calculateClosestPoint(
                ringEffectiveArea, LineString({animalPosition, targetNeighborToTravelTo.second})
            );

            return make_pair(false, make_pair(nextCell, closestPoint)); 
        }
    }
    else
    {
        return make_pair(true, make_pair(this, targetNeighborToTravelTo.second)); 
    }
}

void SpatialTreeTerrainCell::migrateAnimalTo(AnimalInterface* animalToMigrate, TerrainCellInterface* newTerrainCell, const PointContinuous &newPosition)
{
    eraseAnimal(animalToMigrate);
    animalToMigrate->setPosition(newPosition);

    try
    {
        newTerrainCell->insertAnimal(animalToMigrate);
    }
    catch(SpatialTreeTerrainCell::TemporalLeaf2Branch &e)
    {
        static_cast<SpatialTreeTerrainCell*>(newTerrainCell)->getMutableParent()->insertAnimal(animalToMigrate);
    }
}

unique_ptr<FullCoverageAnimals> SpatialTreeTerrainCell::getMutableAnimalsOnAllCell(const EdibleSearchParams &edibleSearchParams)
{
    return getMutableEdiblesOnAllCell(edibleSearchParams).second.second;
}

unique_ptr<const FullCoverageAnimals> SpatialTreeTerrainCell::getAnimalsOnAllCell(const EdibleSearchParams &edibleSearchParams)
{
    unique_ptr<const FullCoverageAnimals> fullCoverageAnimals = move(getMutableEdiblesOnAllCell(edibleSearchParams).second.second);
    return fullCoverageAnimals;
}

unique_ptr<vector<pair<TerrainCellInterface*, pair<TerrainCellInterface::TerrainCellCoverage, unique_ptr<Ring>>>>> SpatialTreeTerrainCell::getNeighboursCellsOnRadius(
        const Ring &radiusArea, const unsigned int searchDepth)
{
    auto intersection = Geometry::calculateIntersection(radiusArea, getEffectiveArea());

    if(Geometry::checkCoverage(intersection.get(), radiusArea) == Coverage::Full)
    {
        return getRadiusTerrainCells(radiusArea, searchDepth);
    }
    else
    {
        return getMutableParent()->getNeighboursCellsOnRadius(radiusArea, searchDepth);
    }
}

unique_ptr<vector<pair<TerrainCellInterface*, pair<TerrainCellInterface::TerrainCellCoverage, unique_ptr<Ring>>>>> SpatialTreeTerrainCell::getNeighboursCellsOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const unsigned int searchDepth)
{
    auto sphere = Geometry::makeSphere(sourcePosition, radius);

    return getNeighboursCellsOnRadius(*sphere, searchDepth);
}

SearchableEdibles SpatialTreeTerrainCell::getEdiblesOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const Ring &radiusArea, const EdibleSearchParams &edibleSearchParams)
{   
    auto intersection = Geometry::calculateIntersection(radiusArea, getEffectiveArea());

    if(Geometry::checkCoverage(intersection.get(), radiusArea) == Coverage::Full)
    {
        return getMutableEdiblesOnAllCell(TerrainCellInterface::Partial, sourcePosition, radius, radiusArea, edibleSearchParams);
    }
    else
    {
        return getMutableParent()->getEdiblesOnRadius(sourcePosition, radius, radiusArea, edibleSearchParams);
    }
}

SearchableEdibles SpatialTreeTerrainCell::getEdiblesOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const EdibleSearchParams &edibleSearchParams)
{   
    auto sphere = Geometry::makeSphere(sourcePosition, radius);

    return getEdiblesOnRadius(sourcePosition, radius, *sphere, edibleSearchParams);
}

SearchableEdibles SpatialTreeTerrainCell::getMutableEdiblesOnAllCell(TerrainCellInterface::TerrainCellCoverage coverage, 
        const PointContinuous &sourcePosition, const double &radius, const Ring &effectiveArea, 
        const EdibleSearchParams &edibleSearchParams)
{
    auto radiusChecker = [&sourcePosition, &radius](AnimalInterface& animal) {
        return Geometry::calculateDistanceBetweenPoints(sourcePosition, animal.getPosition()) < radius;
    };

    switch(coverage) {
		case TerrainCellInterface::Full: {
            return getMutableEdiblesOnAllCell(radiusChecker, edibleSearchParams);
			break;
		}
        case TerrainCellInterface::Sub_Terrain_Cell: {
            auto downChecker = [&polygon = effectiveArea](AnimalInterface& animal) {
                return Geometry::withinPolygon(animal.getPosition(), polygon);
            };

            return getMutableEdiblesOnAllCell(radiusChecker, downChecker, effectiveArea, edibleSearchParams);
			break;
		}
        case TerrainCellInterface::Partial: {
            return getMutableEdiblesOnAllCell(radiusChecker, radiusChecker, effectiveArea, edibleSearchParams);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}

SearchableEdibles SpatialTreeTerrainCell::getMutableEdiblesOnAllCell(const EdibleSearchParams &edibleSearchParams)
{
    unique_ptr<ResourcesOnRadius> searchableResources;
    unique_ptr<PartialCoverageAnimals> searchablePartialCoverageAnimals;

    auto searchableFullCoverageAnimals = getMutableAnimalsUp(edibleSearchParams.getAnimalSearchParams());

    if(!isFullObstacle())
    {
        auto searchableEdibles = getMutableEdiblesOnCellAndDown(edibleSearchParams);
        
        searchableResources = move(searchableEdibles.first);
        searchablePartialCoverageAnimals = move(searchableEdibles.second.first);
        auto fullCoverageAnimalsOnCellAndDown = move(searchableEdibles.second.second);
        
        for(auto &fullCoverageAnimals : *fullCoverageAnimalsOnCellAndDown)
        {
            searchableFullCoverageAnimals->push_back(move(fullCoverageAnimals));
        }
    }
    else
    {
        searchablePartialCoverageAnimals = make_unique<PartialCoverageAnimals>();
        searchableResources = make_unique<ResourcesOnRadius>();
    }

    auto resourcesOnRadius = make_unique<SearchableResources>();

    auto it = edibleSearchParams.getResourceSearchParams().getSearchParams().begin();
    for(unsigned int i = 0; i < searchableResources->size(); i++, ++it)
    {
        if(searchableResources->at(i) > 0.0)
        {
            (*resourcesOnRadius)[getMutableResource(*it)] = searchableResources->at(i);
        }
    }

    return make_pair(move(resourcesOnRadius), make_pair(move(searchablePartialCoverageAnimals), move(searchableFullCoverageAnimals)));
}

SearchableEdibles SpatialTreeTerrainCell::getMutableEdiblesOnAllCell(
        std::function<bool(AnimalInterface&)> upChecker, std::function<bool(AnimalInterface&)> downChecker, 
        const Ring &effectiveArea, const EdibleSearchParams &edibleSearchParams)
{
    unique_ptr<ResourcesOnRadius> searchableResources;
    unique_ptr<FullCoverageAnimals> searchableFullCoverageAnimals;

    auto searchablePartialCoverageAnimals = getMutableAnimalsUp(upChecker, edibleSearchParams.getAnimalSearchParams());

    if(!isFullObstacle())
    {
        auto searchableEdibles = getMutableEdiblesOnCellAndDown(downChecker, effectiveArea, edibleSearchParams);

        searchableResources = move(searchableEdibles.first);
        auto partialCoverageAnimalsOnCellAndDown = move(searchableEdibles.second.first);
        searchableFullCoverageAnimals = move(searchableEdibles.second.second);

        for(auto &partialCoverageAnimals : *partialCoverageAnimalsOnCellAndDown)
        {
            searchablePartialCoverageAnimals->push_back(move(partialCoverageAnimals));
        }
    }
    else
    {
        searchableFullCoverageAnimals = make_unique<FullCoverageAnimals>();
        searchableResources = make_unique<ResourcesOnRadius>();
    }

    auto resourcesOnRadius = make_unique<SearchableResources>();

    auto it = edibleSearchParams.getResourceSearchParams().getSearchParams().begin();
    for(unsigned int i = 0; i < searchableResources->size(); i++, ++it)
    {
        if(searchableResources->at(i) > 0.0)
        {
            (*resourcesOnRadius)[getMutableResource(*it)] = searchableResources->at(i);
        }
    }

    return make_pair(move(resourcesOnRadius), make_pair(move(searchablePartialCoverageAnimals), move(searchableFullCoverageAnimals)));
}

SearchableEdibles SpatialTreeTerrainCell::getMutableEdiblesOnAllCell(
        std::function<bool(AnimalInterface&)> upChecker, const EdibleSearchParams &edibleSearchParams)
{
    unique_ptr<ResourcesOnRadius> searchableResources;
    unique_ptr<FullCoverageAnimals> searchableFullCoverageAnimals;

    auto searchablePartialCoverageAnimals = getMutableAnimalsUp(upChecker, edibleSearchParams.getAnimalSearchParams());

    if(!isFullObstacle())
    {
        auto searchableEdibles = getMutableEdiblesOnCellAndDown(edibleSearchParams);
        
        searchableResources = move(searchableEdibles.first);
        auto partialCoverageAnimalsOnCellAndDown = move(searchableEdibles.second.first);
        searchableFullCoverageAnimals = move(searchableEdibles.second.second);
        
        for(auto &partialCoverageAnimals : *partialCoverageAnimalsOnCellAndDown)
        {
            searchablePartialCoverageAnimals->push_back(move(partialCoverageAnimals));
        }
    }
    else
    {
        searchableFullCoverageAnimals = make_unique<FullCoverageAnimals>();
        searchableResources = make_unique<ResourcesOnRadius>();
    }

    auto resourcesOnRadius = make_unique<SearchableResources>();

    auto it = edibleSearchParams.getResourceSearchParams().getSearchParams().begin();
    for(unsigned int i = 0; i < searchableResources->size(); i++, ++it)
    {
        if(searchableResources->at(i) > 0.0)
        {
            (*resourcesOnRadius)[getMutableResource(*it)] = searchableResources->at(i);
        }
    }

    return make_pair(move(resourcesOnRadius), make_pair(move(searchablePartialCoverageAnimals), move(searchableFullCoverageAnimals)));
}

unique_ptr<FullCoverageAnimals> SpatialTreeTerrainCell::getMutableAnimalsUp(const AnimalSearchParams &animalSearchParams)
{
    auto searchableAnimals = getMutableParent()->getMutableAnimalsUp(animalSearchParams);

    if(!getParent()->isObstacle())
    {
        auto animalsOnCell = getMutableParent()->getMutableAnimalsBy(animalSearchParams);
        searchableAnimals->push_back(move(animalsOnCell));
    }

    return searchableAnimals;
}

unique_ptr<PartialCoverageAnimals> SpatialTreeTerrainCell::getMutableAnimalsUp(
        function<bool(AnimalInterface&)> upChecker, const AnimalSearchParams &animalSearchParams)
{
    auto searchableAnimals = getMutableParent()->getMutableAnimalsUp(upChecker, animalSearchParams);

    if(!getParent()->isObstacle())
    {
        auto animalsOnCell = getMutableParent()->getMutableAnimalsBy(upChecker, animalSearchParams);
        searchableAnimals->push_back(move(animalsOnCell));
    }

    return searchableAnimals;
}

EdiblesOnRadius SpatialTreeTerrainCell::getMutableEdiblesOnCellAndDown(const EdibleSearchParams &edibleSearchParams)
{
    unique_ptr<ResourcesOnRadius> searchableResources;
    unique_ptr<PartialCoverageAnimals> searchablePartialCoverageAnimals = make_unique<PartialCoverageAnimals>();
    unique_ptr<FullCoverageAnimals> searchableFullCoverageAnimals;

    searchableFullCoverageAnimals = getMutableAnimalsDown(edibleSearchParams.getAnimalSearchParams());

    auto searchableEdibles = getMutableEdiblesBy(edibleSearchParams);

    searchableResources = move(searchableEdibles.first);
    auto animalsOnCell = move(searchableEdibles.second);

    searchableFullCoverageAnimals->push_back(move(animalsOnCell));

    return make_pair(move(searchableResources), make_pair(move(searchablePartialCoverageAnimals), move(searchableFullCoverageAnimals)));
}

EdiblesOnRadius SpatialTreeTerrainCell::getMutableEdiblesOnCellAndDown(
        function<bool(AnimalInterface&)> downChecker, const Ring &effectiveArea, const EdibleSearchParams &edibleSearchParams)
{
    unique_ptr<ResourcesOnRadius> searchableResources;
    unique_ptr<PartialCoverageAnimals> searchablePartialCoverageAnimals;
    unique_ptr<FullCoverageAnimals> searchableFullCoverageAnimals;

    auto searchableEdibles = getMutableEdiblesDown(downChecker, effectiveArea, edibleSearchParams);

    searchableResources = move(searchableEdibles.first);
    searchablePartialCoverageAnimals = move(searchableEdibles.second.first);
    searchableFullCoverageAnimals = move(searchableEdibles.second.second);

    auto animalsOnCell = getMutableAnimalsBy(downChecker, edibleSearchParams.getAnimalSearchParams());
    searchablePartialCoverageAnimals->push_back(move(animalsOnCell));

    return make_pair(move(searchableResources), make_pair(move(searchablePartialCoverageAnimals), move(searchableFullCoverageAnimals)));
}
