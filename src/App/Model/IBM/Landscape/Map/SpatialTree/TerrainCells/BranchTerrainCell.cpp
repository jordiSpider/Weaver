

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/EdibleSearchParams.h"
#include "App/Model/IBM/Landscape/Landscape.h"


using namespace std;
using json = nlohmann::json;
namespace bg = boost::geometry;
namespace fs = std::filesystem;



BranchTerrainCell::BranchTerrainCell()
    : SpatialTreeTerrainCell()
{

}

BranchTerrainCell::BranchTerrainCell(const std::vector<PreciseDouble>& cellSizes, const unsigned int& mapDepth, PointSpatialTree* const position, MoistureSource* const moistureBaseSource)
    : SpatialTreeTerrainCell(nullptr, position, cellSizes[position->getDepth()])
{
    setPatchApplicator(new BranchPatchApplicator(this, moistureBaseSource));

    generateChildren(cellSizes, mapDepth, moistureBaseSource);
}

BranchTerrainCell::BranchTerrainCell(const std::vector<PreciseDouble>& cellSizes, const unsigned int& mapDepth, BranchTerrainCell* const parentTerrainCell, PointSpatialTree* const position, MoistureSource* const moistureBaseSource)
    : SpatialTreeTerrainCell(parentTerrainCell, position, cellSizes[position->getDepth()])
{
    setPatchApplicator(new BranchPatchApplicator(this, moistureBaseSource));

    generateChildren(cellSizes, mapDepth, moistureBaseSource);
}


BranchTerrainCell::~BranchTerrainCell()
{
    for(auto& elem : childrenTerrainCells)
    {
        delete elem;
    }
}


void BranchTerrainCell::generateChildren(const std::vector<PreciseDouble>& cellSizes, const unsigned int& mapDepth, MoistureSource* const moistureBaseSource)
{
    unsigned int parentX = getPosition().get(Axis::X) * SpatialTree::numbreOfSubdivisions;
    unsigned int parentY = getPosition().get(Axis::Y) * SpatialTree::numbreOfSubdivisions;
    #if DIMENSIONS == 3
    unsigned int parentZ = getPosition().get(Axis::Z) * SpatialTree::numbreOfSubdivisions;
    #endif

    childrenTerrainCells.reserve(numberOfChildren);
    #if DIMENSIONS == 3
    for(unsigned int Z = 0; Z < SpatialTree::numbreOfSubdivisions; Z++)
    {
    #endif
        for(unsigned int Y = 0; Y < SpatialTree::numbreOfSubdivisions; Y++)
        {
            for(unsigned int X = 0; X < SpatialTree::numbreOfSubdivisions; X++)
            {
                #if DIMENSIONS == 2
                vector<unsigned int> axisValues = {parentX + X, parentY + Y};
                #elif DIMENSIONS == 3
                vector<unsigned int> axisValues = {parentX + X, parentY + Y, parentZ + Z};
                #endif
                PointSpatialTree* childPosition = new PointSpatialTree(axisValues, getPosition().getDepth() + 1);
                
                if(childPosition->getDepth() == mapDepth-1)
                {
                    childrenTerrainCells.push_back(new LeafTerrainCell(cellSizes, this, childPosition, moistureBaseSource));
                }
                else
                {
                    childrenTerrainCells.push_back(new BranchTerrainCell(cellSizes, mapDepth, this, childPosition, moistureBaseSource));
                }
            }
        }
    #if DIMENSIONS == 3
    }
    #endif
}

void BranchTerrainCell::addAnimalSpecies(const AnimalSpecies& animalSpecies, const AnimalSearchParams& allAnimalsSearchParams, const PreciseDouble& timeStepsPerDay)
{
    TerrainCell::addAnimalSpecies(animalSpecies, allAnimalsSearchParams, timeStepsPerDay);

    for(const auto &child : getChildrenTerrainCells())
    {
        child->addAnimalSpecies(animalSpecies, allAnimalsSearchParams, timeStepsPerDay);
    }
}

void BranchTerrainCell::addResourceSpecies(Landscape* const landscape, vector<vector<vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource, const AnimalSearchParams& allAnimalsSearchParams, const PreciseDouble& timeStepsPerDay)
{
    for(const auto &child : getChildrenTerrainCells())
    {
        child->addResourceSpecies(landscape, landscapeResources, resourceSpecies, resourceBaseSource, allAnimalsSearchParams, timeStepsPerDay);
    }

    TerrainCell::addResourceSpecies(landscape, landscapeResources, resourceSpecies, resourceBaseSource, allAnimalsSearchParams, timeStepsPerDay);
}

const vector<SpatialTreeTerrainCell*>& BranchTerrainCell::getChildrenTerrainCells() const
{
    return childrenTerrainCells;
}

vector<SpatialTreeTerrainCell*>& BranchTerrainCell::getMutableChildrenTerrainCells()
{
    return childrenTerrainCells;
}

const SpatialTreeTerrainCell* BranchTerrainCell::getChildTerrainCell(const size_t childIndex) const
{
    return getChildrenTerrainCells().at(childIndex);
}

SpatialTreeTerrainCell* BranchTerrainCell::getMutableChildTerrainCell(const size_t childIndex)
{
    return getMutableChildrenTerrainCells().at(childIndex);
}

size_t BranchTerrainCell::calculateChildPositionOnVector(Landscape* const landscape, const PointContinuous &childPos) const
{
    unsigned int depth = static_cast<const SpatialTree * const>(landscape->getMap())->getMapDepth()-1;

    return calculateChildPositionOnVector(PointSpatialTree(static_cast<const SpatialTree * const>(landscape->getMap())->obtainPointMap(childPos, depth).getAxisValues(), depth));
}

size_t BranchTerrainCell::calculateChildPositionOnVector(const PointSpatialTree &childPos) const
{
    size_t posOnVector = 0;

    const unsigned int depthDifference = childPos.getDepth() - getPosition().getDepth();

    for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();
        posOnVector += ((childPos.get(axis) / static_cast<unsigned int>(displacementPower(1u, depthDifference-1))) % SpatialTree::numbreOfSubdivisions) * static_cast<unsigned int>(displacementPower(1u, i));
    }

    return posOnVector;
}

SpatialTreeTerrainCell* BranchTerrainCell::getCell(const PointSpatialTree &cellPos)
{
    if(cellPos == getPosition())
    {
        return this;
    }
    else
    {
        if(isChild(cellPos))
        {
            return getMutableChildTerrainCell(cellPos)->getCell(cellPos);
        }
        else
        {
            return getMutableParent()->getCell(cellPos);
        }
    }
}

bool BranchTerrainCell::isChild(Map* const map, const PointContinuous &childPos) const
{
    unsigned int depth = static_cast<const SpatialTree * const>(map)->getMapDepth()-1u;

    return isChild(PointSpatialTree(static_cast<const SpatialTree * const>(map)->obtainPointMap(childPos, depth).getAxisValues(), depth));
}

bool BranchTerrainCell::isChild(const PointSpatialTree &childPos) const
{
    vector<unsigned int> axisValues(DIMENSIONS);

    const unsigned int depthDifference = childPos.getDepth() - getPosition().getDepth();

    if(depthDifference > 0)
    {
        for(unsigned char i = 0; i < DIMENSIONS; i++)
        {
            Axis axis = magic_enum::enum_cast<Axis>(i).value();
            axisValues[i] = childPos.get(axis) / static_cast<unsigned int>(displacementPower(1u, depthDifference));
        }

        bool result = (getPosition() == PointSpatialTree(axisValues, getPosition().getDepth()));

        return result;
    }
    else
    {
        return false;
    }
}

const SpatialTreeTerrainCell* BranchTerrainCell::getChildTerrainCell(const PointSpatialTree &cellPos) const
{
    #ifdef DEBUG
    if(!isChild(cellPos))
    {
        throwLineInfoException("The target cell is not a descendant of the current cell");
    }
    #endif

    const size_t childPosOnVector = calculateChildPositionOnVector(cellPos);
    return getChildTerrainCell(childPosOnVector);
}

SpatialTreeTerrainCell* BranchTerrainCell::getMutableChildTerrainCell(const PointSpatialTree &cellPos)
{
    #ifdef DEBUG
    if(!isChild(cellPos))
    {
        throwLineInfoException("The target cell is not a descendant of the current cell");
    }
    #endif

    const size_t childPosOnVector = calculateChildPositionOnVector(cellPos);
    return getMutableChildTerrainCell(childPosOnVector);
}


void BranchTerrainCell::insertAnimal(Landscape* const landscape, Animal* const newAnimal)
{
    AnimalNonStatistical* const newAnimalCast = static_cast<AnimalNonStatistical* const>(newAnimal);

    if(getPosition().getDepth() == newAnimalCast->getCellDepthOnActualInstar())
    {
        #ifdef DEBUG
        if(getPatchApplicator().getCellObstacle().isObstacle())
        {
            throwLineInfoException("The animal has been inserted in an obstacle");
        }
        #endif

        TerrainCell::insertAnimal(landscape, newAnimal);
    }
    else
    {
        if(!isChild(landscape->getMutableMap(), newAnimalCast->getPosition()))
        {
            PointContinuous actualPosition = newAnimalCast->getPosition();

            for(unsigned char i = 0; i < DIMENSIONS; i++)
            {
                if(static_cast<double>(getPosition().getAxisValues().at(i)+1)*getSize() == getPositionAxisValue(actualPosition, i))
                {
                    setPositionAxisValue(actualPosition, i, getPositionAxisValue(actualPosition, i)-PreciseDouble::EPS);
                }
            }

            newAnimalCast->setPosition(actualPosition);

            if(!isChild(landscape->getMutableMap(), newAnimalCast->getPosition()))
            {
                throwLineInfoException("The animal (id: " + to_string(newAnimalCast->getId()) + ") has not been inserted into any child cell");
            }
        }
        

        const size_t childIndex = calculateChildPositionOnVector(landscape, newAnimalCast->getPosition());
        auto child = getMutableChildTerrainCell(childIndex);

        #ifdef DEBUG
        if(child->getPatchApplicator().getCellObstacle().isFullObstacle())
        {
            throwLineInfoException("The animal has been inserted in an obstacle");
        }
        #endif

        child->insertAnimal(landscape, newAnimal);
    }
}

tuple<bool, TerrainCell*, TerrainCell*, Animal*, size_t> BranchTerrainCell::randomInsertAnimal(Landscape* const landscape, const Instar &instar, AnimalSpecies* animalSpecies, const bool isStatistical, const bool saveGenetics, const bool saveMassInfo, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
    if(getPosition().getDepth() == animalSpecies->getCellDepthPerInstar()[instar])
    {
        return TerrainCell::randomInsertAnimal(landscape, instar, animalSpecies, isStatistical, saveGenetics, saveMassInfo, actualTimeStep, timeStepsPerDay);
    }
    else
    {
        vector<size_t> randomIndexVector;
        Random::createIndicesVector(randomIndexVector, getChildrenTerrainCells().size());

        for(unsigned int i = 0; i < randomIndexVector.size(); i++)
        {
            if(!getChildTerrainCell(randomIndexVector.at(i))->getPatchApplicator().getCellObstacle().isObstacle())
            {
                return getMutableChildTerrainCell(randomIndexVector.at(i))->randomInsertAnimal(landscape, instar, animalSpecies, isStatistical, saveGenetics, saveMassInfo, actualTimeStep, timeStepsPerDay);
            }
        }

        throwLineInfoException("The animal has not been inserted into any child cell");
    }
}

void BranchTerrainCell::update(const TimeStep& numberOfTimeSteps)
{
    updateChildren(numberOfTimeSteps);

    TerrainCell::update(numberOfTimeSteps);
}

void BranchTerrainCell::updateChildren(const TimeStep& numberOfTimeSteps)
{
    // Create a random vector of indices
    vector<size_t> indexVector;
    Random::createIndicesVector(indexVector, getChildrenTerrainCells().size());

    // Random children updates
    for(const auto &index : indexVector)
    {
        getMutableChildTerrainCell(index)->update(numberOfTimeSteps);
    }
}



void BranchTerrainCell::obtainInhabitableTerrainCells(Map* const map)
{
    if(!getPatchApplicator().getCellObstacle().isFullObstacle())
    {
        TerrainCell::obtainInhabitableTerrainCells(map);

        for(auto &child : getMutableChildrenTerrainCells())
        {
            child->obtainInhabitableTerrainCells(map);
        }
    }
}

bool BranchTerrainCell::isLeaf() const
{
    return false;
}



/****************/
/*    Animal    */
/****************/

    /**************/
    /*    Down    */
    /**************/

void BranchTerrainCell::applyDownFunctionToAnimals(const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions)
{
    if(!getPatchApplicator().getCellObstacle().isFullObstacle())
    {
        for(auto &child : getMutableChildrenTerrainCells())
        {
            child->TerrainCell::applyFunctionToAnimals(animalFunctions);

            child->applyDownFunctionToAnimals(animalFunctions);
        }
    }
}

void BranchTerrainCell::applyDownFunctionToAnimals(
        function<bool(Animal&)> checker, const PointContinuous &sourcePosition, const PreciseDouble &radius,
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    )
{
    if(!getPatchApplicator().getCellObstacle().isFullObstacle())
    {
        for(auto &child : getMutableChildrenTerrainCells())
        {
            if(Geometry::fullCoveredBySphere(&child->getEffectiveArea(), sourcePosition, radius))
            {
                child->TerrainCell::applyFunctionToAnimals(animalFunctions);

                child->applyDownFunctionToAnimals(animalFunctions);
            }
            else
            {
                child->TerrainCell::applyFunctionToAnimals(checker, animalFunctions);

                child->applyDownFunctionToAnimals(checker, sourcePosition, radius, animalFunctions);
            }
        }
    }
}

void BranchTerrainCell::randomApplyDownFunctionToAnimals(const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions)
{
    if(!getPatchApplicator().getCellObstacle().isFullObstacle())
    {
        vector<size_t> indexVector;
        Random::createIndicesVector(indexVector, getChildrenTerrainCells().size());

        for(const auto &index : indexVector)
        {
            vector<size_t> subIndexVector;
            Random::createIndicesVector(subIndexVector, 2);

            for(const auto &subIndex : subIndexVector)
            {
                switch(subIndex) {
                    case 0: {
                        getMutableChildTerrainCell(index)->TerrainCell::randomApplyFunctionToAnimals(animalFunctions);
                        break;
                    }
                    case 1: {
                        getMutableChildTerrainCell(index)->randomApplyDownFunctionToAnimals(animalFunctions);
                        break;
                    }
                    default: {
                        throwLineInfoException("Default case");
                        break;
                    }
                }
            }
        }
    }
}


void BranchTerrainCell::registerEdibles(vector<vector<vector<CellResource*>>>& landscapeResources, vector<AnimalNonStatistical*>& landscapeAnimals)
{
    registerAnimals(landscapeAnimals);

    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->registerEdibles(landscapeResources, landscapeAnimals);
    }
}


void BranchTerrainCell::getRadiusTerrainCells(
    vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const size_t searchDepth, const bool searchNeighborsWithFemales, const bool parentFullCoverage, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate)
{
    if(!getPatchApplicator().getCellObstacle().isFullObstacle())
    {
        if(static_cast<const PointSpatialTree&>(getPosition()).getDepth() == searchDepth)
        {
            if(!getPatchApplicator().getCellObstacle().isObstacle())
            {
                getCellEvaluation(bestEvaluations, animalWhoIsEvaluating, sourcePosition, radius, radiusArea, searchNeighborsWithFemales, parentFullCoverage, animalsHasTriedToPredate);
            }
        }
        else
        {
            bool currentFullCoverage = parentFullCoverage;

            if(!currentFullCoverage)
            {
                currentFullCoverage = Geometry::fullCoveredBySphere(&getEffectiveArea(), sourcePosition, radius);
            }


            for(auto &child : getMutableChildrenTerrainCells())
            {
                child->getRadiusTerrainCells(bestEvaluations, sourcePosition, radius, radiusArea, searchDepth, searchNeighborsWithFemales, currentFullCoverage, animalWhoIsEvaluating, animalsHasTriedToPredate);
            } 
        }
    }
}

void BranchTerrainCell::deserializeSources(
        vector<pair<size_t, MoistureSource*>>& appliedMoisture,
        std::vector<vector<pair<size_t, ResourceSource*>>>& appliedResource
    )
{
    SpatialTreeTerrainCell::deserializeSources(appliedMoisture, appliedResource);

    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->deserializeSources(appliedMoisture, appliedResource);
    }
}

void BranchTerrainCell::deserializeSpecies(std::vector<ResourceSpecies*>& existingResourceSpecies, std::vector<AnimalSpecies*>& existingAnimalSpecies)
{
    TerrainCell::deserializeSpecies(existingResourceSpecies, existingAnimalSpecies);

    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->deserializeSpecies(existingResourceSpecies, existingAnimalSpecies);
    }
}


void BranchTerrainCell::saveResourceSpeciesSnapshot(ofstream &file, const ResourceSpecies* const &species) const
{
    for(const auto &child : getChildrenTerrainCells())
    {
        child->saveResourceSpeciesSnapshot(file, species);
    }

    TerrainCell::saveResourceSpeciesSnapshot(file, species);
}


void BranchTerrainCell::saveWaterSnapshot(ofstream &file) const
{
    if(!getPatchApplicator().getCellObstacle().isFullObstacle())
    {
        for(const auto &child : getChildrenTerrainCells())
        {
            child->saveWaterSnapshot(file);
        }

        TerrainCell::saveWaterSnapshot(file);
    }
}



BOOST_CLASS_EXPORT(BranchTerrainCell)

template <class Archive>
void BranchTerrainCell::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<SpatialTreeTerrainCell>(*this);
    
    ar & childrenTerrainCells;
    for(SpatialTreeTerrainCell*& newChildrenTerrainCell : childrenTerrainCells)
	{
        newChildrenTerrainCell->setParent(this);
    }
}

// Specialisation
template void BranchTerrainCell::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void BranchTerrainCell::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void BranchTerrainCell::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void BranchTerrainCell::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
