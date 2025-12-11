

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/EdibleSearchParams.h"
#include "App/Model/IBM/Landscape/Landscape.h"


using namespace std;



const unsigned int SpatialTreeTerrainCell::numberOfChildren = static_cast<unsigned int>(std::pow(SpatialTree::numbreOfSubdivisions, DIMENSIONS));


SpatialTreeTerrainCell::SpatialTreeTerrainCell()
    : TerrainCell()
{

}

SpatialTreeTerrainCell::SpatialTreeTerrainCell(BranchTerrainCell* const parentTerrainCell, PointSpatialTree* const position, const PreciseDouble &size)
    : TerrainCell(position, size),
      parentTerrainCell(parentTerrainCell)
{

}

SpatialTreeTerrainCell::~SpatialTreeTerrainCell()
{
    
}


void SpatialTreeTerrainCell::setParent(BranchTerrainCell* newParentTerrainCell)
{
    parentTerrainCell = newParentTerrainCell;
}

const BranchTerrainCell* SpatialTreeTerrainCell::getParent() const
{
    return parentTerrainCell;
}

BranchTerrainCell* SpatialTreeTerrainCell::getMutableParent() const
{
    return parentTerrainCell;
}




TerrainCell* SpatialTreeTerrainCell::getCell(const PointMap &dstPos)
{
    return getCell(static_cast<const PointSpatialTree&>(dstPos));
}




pair<bool, pair<TerrainCell*, PointContinuous>> SpatialTreeTerrainCell::getCellByBearing(
        Map* const map, const pair<PointMap, PointContinuous> &targetNeighborToTravelTo, const PointContinuous &animalPosition, const PreciseDouble& remainingSearchArea)
{
    bool atDestination;
    pair<TerrainCell*, PointContinuous> cellToMoveTo;

    if(getPosition().getDepth() > 0)
    {
        vector<pair<unsigned char, int>> movementAxes;
        for(unsigned char i = 0; i < DIMENSIONS; i++)
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
            size_t randomIndex = Random::randomIndex(movementAxes.size());
            axisValues[movementAxes[randomIndex].first] += movementAxes[randomIndex].second;
        }

        PointSpatialTree nextCellPosition(axisValues, getPosition().getDepth());
        TerrainCell* nextCell = getCell(nextCellPosition);

        //Avoid obstacles using the least bearing difference path
        if(nextCell->getPatchApplicator().getCellObstacle().isObstacle())
        {
            vector<int> initialCoords(DIMENSIONS);

            for(unsigned char i = 0; i < DIMENSIONS; i++)
            {
                Axis axis = magic_enum::enum_cast<Axis>(i).value();

                initialCoords[i] = static_cast<int>(getPosition().get(axis)) - 1;
            }

            vector<PointSpatialTree> neighboursPoints;
            static_cast<const SpatialTree * const>(map)->generatePoints(neighboursPoints, 3, getPosition(), initialCoords);

            size_t bestBearingDifference = calculateManhattanDistanceToPoint(targetNeighborToTravelTo.first) + 2;
            vector<pair<TerrainCell*, PointSpatialTree>> surroundingTerrainCells;

            for(size_t i = 0; i < neighboursPoints.size(); i++)
            {
                if(calculateManhattanDistanceToPoint(neighboursPoints.at(i)) == 1)
                {
                    auto neighbour = getCell(neighboursPoints.at(i));
                
                    if(!neighbour->getPatchApplicator().getCellObstacle().isObstacle())
                    {
                        size_t bearingDifference = neighbour->calculateManhattanDistanceToPoint(targetNeighborToTravelTo.first);

                        if(bearingDifference < bestBearingDifference)
                        {
                            bestBearingDifference = bearingDifference;
                            surroundingTerrainCells.clear();
                            surroundingTerrainCells.push_back(make_pair(neighbour, neighboursPoints.at(i)));
                        }
                        else if(bearingDifference == bestBearingDifference)
                        {
                            surroundingTerrainCells.push_back(make_pair(neighbour, neighboursPoints.at(i)));
                        }
                    }
                }  
            }

            if(surroundingTerrainCells.empty())
            {
                throwLineInfoException("The animal is immobile");
            }

            size_t index = Random::randomIndex(surroundingTerrainCells.size());

            nextCell = surroundingTerrainCells[index].first;
            nextCellPosition = surroundingTerrainCells[index].second;
        }


        if(static_cast<PointMap>(nextCellPosition) == targetNeighborToTravelTo.first)
        {
            atDestination = true;
            cellToMoveTo = make_pair(nextCell, targetNeighborToTravelTo.second);
        }
        else
        {
            BoxModel* boxEffectiveArea = Geometry::makeBoxEffectiveArea(nextCellPosition, getSize());

            RingModel ringEffectiveArea;
            boost::geometry::convert(*boxEffectiveArea, ringEffectiveArea);

            delete boxEffectiveArea;

            PointContinuous closestPoint = Geometry::calculateClosestPoint(
                ringEffectiveArea, LineStringModel({animalPosition, targetNeighborToTravelTo.second})
            );

            atDestination = false;
            cellToMoveTo = make_pair(nextCell, closestPoint);
        }
    }
    else
    {
        atDestination = true;
        cellToMoveTo = make_pair(this, targetNeighborToTravelTo.second);
    }


    PreciseDouble distanceToCell = Geometry::calculateDistanceBetweenPoints(animalPosition, cellToMoveTo.second);


    if(distanceToCell > remainingSearchArea)
    {
        atDestination = false;
        cellToMoveTo = make_pair(this, Geometry::calculateLineStringPointAtDistance(animalPosition, cellToMoveTo.second, remainingSearchArea));
    }
    

    return make_pair(atDestination, cellToMoveTo);
}

pair<AnimalNonStatistical*, size_t> SpatialTreeTerrainCell::createAnimal(Landscape* const landscape, const Instar &instar, AnimalSpecies* animalSpecies, const bool saveGenetics, const bool saveMassInfo, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
    size_t numberOfDiscardedIndividualsOutsideRestrictedRanges = 0;

    AnimalNonStatistical* newAnimal = new SpatialTreeAnimal(instar, animalSpecies, this, actualTimeStep, timeStepsPerDay);

    while(!newAnimal->getGenetics().isInsideRestrictedRanges())
    {
        numberOfDiscardedIndividualsOutsideRestrictedRanges++;

        delete newAnimal;
        newAnimal = new SpatialTreeAnimal(instar, animalSpecies, this, actualTimeStep, timeStepsPerDay);
    }

    // Indicate that the animal created is the final animal, and therefore assign a final ID to it.
    newAnimal->doDefinitive(landscape, saveGenetics, saveMassInfo);

    return make_pair<>(newAnimal, numberOfDiscardedIndividualsOutsideRestrictedRanges);
}

void SpatialTreeTerrainCell::migrateAnimalTo(Landscape* const landscape, Animal* animalToMigrate, TerrainCell* newTerrainCell, const PointContinuous &newPosition)
{
    AnimalNonStatistical* animalToMigrateCast = static_cast<AnimalNonStatistical*>(animalToMigrate);

    eraseAnimal(animalToMigrate);
    animalToMigrateCast->setPosition(newPosition);

    newTerrainCell->insertAnimal(landscape, animalToMigrate);

    IndividualGrowthModel::Type previousGrowthModel = animalToMigrateCast->getMutableGrowthBuildingBlock().getIndividualGrowthModel()->getType();

    animalToMigrateCast->calculateGrowthModel(landscape->getTimeStepsPerDay());

    if(previousGrowthModel != animalToMigrateCast->getMutableGrowthBuildingBlock().getIndividualGrowthModel()->getType())
    {
        animalToMigrateCast->calculateGrowthCurves(landscape->getTimeStepsPerDay());
    }
}




/****************/
/*    Animal    */
/****************/

void SpatialTreeTerrainCell::applyFunctionToAnimals(const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions)
{
    applyUpFunctionToAnimals(
        [&polygon = getEffectiveArea()](Animal& animal) {
            return Geometry::withinPolygon(animal.getPosition(), &polygon);
        }, 
        animalFunctions
    );

    TerrainCell::applyFunctionToAnimals(animalFunctions);

    applyDownFunctionToAnimals(animalFunctions);
}

///////////////////

void SpatialTreeTerrainCell::randomApplyFunctionToAnimals(const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions)
{
    vector<size_t> indexVector;
    Random::createIndicesVector(indexVector, 3);

    for(const auto &index : indexVector)
    {
        switch(index) {
            case 0: {
                randomApplyUpFunctionToAnimals(
                    [&polygon = getEffectiveArea()](Animal& animal) {
                        return Geometry::withinPolygon(animal.getPosition(), &polygon);
                    },
                    animalFunctions
                );
                break;
            }
            case 1: {
                TerrainCell::randomApplyFunctionToAnimals(animalFunctions);
                break;
            }
            case 2: {
                randomApplyDownFunctionToAnimals(animalFunctions);
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    }
}


    /************/
    /*    Up    */
    /************/

void SpatialTreeTerrainCell::applyUpFunctionToAnimals(
        function<bool(Animal&)> checker,
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    )
{
    getMutableParent()->applyUpFunctionToAnimals(checker, animalFunctions);

    getMutableParent()->TerrainCell::applyFunctionToAnimals(checker, animalFunctions);
}

/////////////////

void SpatialTreeTerrainCell::randomApplyUpFunctionToAnimals(
        function<bool(Animal&)> checker,
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    )
{
    vector<size_t> indexVector;
    Random::createIndicesVector(indexVector, 2);

    for(const auto &index : indexVector)
    {
        switch(index) {
            case 0: {
                getMutableParent()->randomApplyUpFunctionToAnimals(checker, animalFunctions);
                break;
            }
            case 1: {
                getMutableParent()->TerrainCell::randomApplyFunctionToAnimals(checker, animalFunctions);
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    }
}


/****************/
/*    Edible    */
/****************/

void SpatialTreeTerrainCell::applyFunctionToEdibles(
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    )
{
    applyUpFunctionToAnimals(
        [&polygon = getEffectiveArea()](Animal& animal) {
            return Geometry::withinPolygon(animal.getPosition(), &polygon);
        },
        animalFunctions
    );

    TerrainCell::applyFunctionToEdibles(animalFunctions, resourceFunctions);

    applyDownFunctionToAnimals(animalFunctions);
}

void SpatialTreeTerrainCell::applyFunctionToEdibles(
        function<bool(Animal&)> checker, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    )
{   
    applyUpFunctionToAnimals(
        [&polygon = getEffectiveArea()](Animal& animal) {
            return Geometry::withinPolygon(animal.getPosition(), &polygon);
        }, 
        animalFunctions
    );

    TerrainCell::applyFunctionToEdibles(checker, sourcePosition, radius, radiusArea, animalFunctions, resourceFunctions);

    applyDownFunctionToAnimals(checker, sourcePosition, radius, animalFunctions);
}


/**************************/
/*    Edible in Radius    */
/**************************/

void SpatialTreeTerrainCell::applyFunctionToEdiblesInRadius(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    )
{   
    if(Geometry::fullCoveredBySphere(&getEffectiveArea(), sourcePosition, radius))
    {
        getMutableParent()->applyFunctionToEdiblesInRadius(sourcePosition, radius, radiusArea, animalFunctions, resourceFunctions);
    }
    else
    {
        applyFunctionToEdiblesInCell(false, sourcePosition, radius, radiusArea, animalFunctions, resourceFunctions);
    }
}






void SpatialTreeTerrainCell::getNeighboursCellsOnRadius(
    vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const size_t searchDepth, const bool searchNeighborsWithFemales, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate)
{
    if(Geometry::fullCoveredBySphere(&getEffectiveArea(), sourcePosition, radius))
    {
        getMutableParent()->getNeighboursCellsOnRadius(bestEvaluations, sourcePosition, radius, radiusArea, searchDepth, searchNeighborsWithFemales, animalWhoIsEvaluating, animalsHasTriedToPredate);
    }
    else
    {
        getRadiusTerrainCells(bestEvaluations, sourcePosition, radius, radiusArea, searchDepth, searchNeighborsWithFemales, false, animalWhoIsEvaluating, animalsHasTriedToPredate);
    }
}

void SpatialTreeTerrainCell::getNeighboursCellsOnRadius(
    vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const size_t searchDepth, const bool searchNeighborsWithFemales, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate)
{
    if(radius > 0.0)
    {
        RingModel* sphere = Geometry::makeSphere(sourcePosition, radius);

        getNeighboursCellsOnRadius(bestEvaluations, sourcePosition, radius, sphere, searchDepth, searchNeighborsWithFemales, animalWhoIsEvaluating, animalsHasTriedToPredate);
    
        delete sphere;
    }
}




const PointSpatialTree& SpatialTreeTerrainCell::getPosition() const
{
    return static_cast<const PointSpatialTree&>(TerrainCell::getPosition());
}

PointSpatialTree& SpatialTreeTerrainCell::getMutablePosition() const
{
    return static_cast<PointSpatialTree&>(TerrainCell::getMutablePosition());
}


SpatialTreePatchApplicator& SpatialTreeTerrainCell::getMutablePatchApplicator()
{
    return static_cast<SpatialTreePatchApplicator&>(TerrainCell::getMutablePatchApplicator());
}

const SpatialTreePatchApplicator& SpatialTreeTerrainCell::getPatchApplicator() const
{
    return static_cast<const SpatialTreePatchApplicator&>(TerrainCell::getPatchApplicator());
}





BOOST_SERIALIZATION_ASSUME_ABSTRACT(SpatialTreeTerrainCell)

template <class Archive>
void SpatialTreeTerrainCell::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<TerrainCell>(*this);
}

// // Specialisation
template void SpatialTreeTerrainCell::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SpatialTreeTerrainCell::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SpatialTreeTerrainCell::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SpatialTreeTerrainCell::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
