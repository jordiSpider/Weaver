

#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "IBM/World/Map/SpatialTree.h"
#include "IBM/World/Map/TerrainCells/EdibleSearchParams.h"
#include "IBM/World/World.h"


using namespace std;
using json = nlohmann::json;
namespace bg = boost::geometry;
namespace fs = boost::filesystem;



BranchTerrainCell::BranchTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, SpatialTree* const map)
    : BranchTerrainCellInterface(
        parentTerrainCell, position, makeEffectiveArea(position, map->getCellSize(position->getDepth())), map->getCellSize(position->getDepth()), 
        map, makeDefaultAnimals(map), false, false, -1,
        new SummaryMoisture(this), false, false, -1, 0.0
      ),
      maximumMoisturePatchPriority(-1),
      maximumResourcePatchPriority(ResourceSpecies::getResourceSpeciesCounter(), -1)
{
    generateChildren();

    for(unsigned int i = 0; i < getResources().size(); i++)
    {
        setSummaryResourcePatch(i);
    }
}


BranchTerrainCell::BranchTerrainCell(TemporalLeafTerrainCell &leaf)
    : BranchTerrainCellInterface(
        leaf.getMutableParent(), &leaf.getMutablePosition(), &leaf.getEffectiveArea(), leaf.getSize(), 
        &static_cast<SpatialTree&>(leaf.getMutableMap()), &leaf.getMutableAnimals(),
        leaf.isObstacle(), leaf.isFullObstacle(),
        leaf.getObstaclePatchPriority(), leaf.getMutableMoistureInfo(), leaf.isMoistureSource(), leaf.isInMoisturePatch(),
        leaf.getMoisturePatchPriority(), leaf.getTotalMaximumResourceCapacity()
      ),
      maximumMoisturePatchPriority(leaf.getMoisturePatchPriority()),
      maximumResourcePatchPriority(ResourceSpecies::getResourceSpeciesCounter(), -1)
{
    generateChildren(&leaf.getMutableResources(), &leaf.getResourcePatchPriority());

    for(unsigned int i = 0; i < getResources().size(); i++)
    {
        setSummaryResourcePatch(i);
    }
}


BranchTerrainCell::~BranchTerrainCell()
{
    
}


void BranchTerrainCell::generateChildren(vector<ResourceInterface*>* const resources, const vector<int>* const resourcePatchPriority)
{
    unsigned int parentX = getPosition().get(Axis::X) * SpatialTree::numbreOfSubdivisions;
    unsigned int parentY = getPosition().get(Axis::Y) * SpatialTree::numbreOfSubdivisions;
    #if DIMENSIONS == 3
    unsigned int parentZ = getPosition().get(Axis::Z) * SpatialTree::numbreOfSubdivisions;
    #endif

    childrenTerrainCells.reserve(numberOfChildren);
    childrenTerrainCellPointers.reserve(numberOfChildren);
    childrenTerrainCellConstPointers.reserve(numberOfChildren);
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
                
                if(childPosition->getDepth() == static_cast<const SpatialTree&>(getMap()).getMapDepth()-1)
                {
                    childrenTerrainCells.push_back(move(make_unique<LeafTerrainCell>(this, childPosition, resources, resourcePatchPriority)));
                }
                else
                {
                    childrenTerrainCells.push_back(move(make_unique<TemporalLeafTerrainCell>(this, childPosition, resources, resourcePatchPriority)));
                }

                childrenTerrainCellPointers.push_back(childrenTerrainCells.back().get());
                childrenTerrainCellConstPointers.push_back(childrenTerrainCells.back().get());
            }
        }
    #if DIMENSIONS == 3
    }
    #endif
}


const vector<const SpatialTreeTerrainCellInterface*>& BranchTerrainCell::getChildrenTerrainCells() const
{
    return childrenTerrainCellConstPointers;
}

vector<SpatialTreeTerrainCellInterface*>& BranchTerrainCell::getMutableChildrenTerrainCells()
{
    return childrenTerrainCellPointers;
}

const SpatialTreeTerrainCellInterface* const BranchTerrainCell::getChildTerrainCell(const unsigned int childIndex) const
{
    return getChildrenTerrainCells().at(childIndex);
}

SpatialTreeTerrainCellInterface* BranchTerrainCell::getMutableChildTerrainCell(const unsigned int childIndex)
{
    return getMutableChildrenTerrainCells().at(childIndex);
}

const unsigned int BranchTerrainCell::calculateChildPositionOnVector(const PointContinuous &childPos) const
{
    unsigned int depth = static_cast<const SpatialTree &>(getMap()).getMapDepth()-1;

    return calculateChildPositionOnVector(PointSpatialTree(static_cast<const SpatialTree &>(getMap()).obtainPointMap(childPos, depth).getAxisValues(), depth));
}

const unsigned int BranchTerrainCell::calculateChildPositionOnVector(const PointSpatialTree &childPos) const
{
    unsigned int posOnVector = 0;

    const unsigned int depthDifference = childPos.getDepth() - getPosition().getDepth();

    for(unsigned int i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();
        posOnVector += ((childPos.get(axis) / displacementPower(1, depthDifference-1)) % SpatialTree::numbreOfSubdivisions) * displacementPower(1, i);
    }

    return posOnVector;
}

SpatialTreeTerrainCellInterface* const BranchTerrainCell::getCell(const PointSpatialTree &cellPos)
{
    if(cellPos == getPosition())
    {
        return this;
    }
    else
    {
        #ifdef DEBUG
        if(!isChild(cellPos))
        {
            throwLineInfoException("The target cell is not a descendant of the current cell");
        }
        #endif

        return getMutableChildTerrainCell(cellPos)->getCell(cellPos); 
    }
}

bool BranchTerrainCell::isChild(const PointContinuous &childPos) const
{
    unsigned int depth = static_cast<const SpatialTree &>(getMap()).getMapDepth()-1;

    return isChild(PointSpatialTree(static_cast<const SpatialTree &>(getMap()).obtainPointMap(childPos, depth).getAxisValues(), depth));
}

bool BranchTerrainCell::isChild(const PointSpatialTree &childPos) const
{
    vector<unsigned int> axisValues(DIMENSIONS);

    const unsigned int depthDifference = childPos.getDepth() - getPosition().getDepth();

    if(depthDifference > 0)
    {
        for(unsigned int i = 0; i < DIMENSIONS; i++)
        {
            Axis axis = magic_enum::enum_cast<Axis>(i).value();
            axisValues[i] = childPos.get(axis) / displacementPower(1, depthDifference);
        }

        bool result = (getPosition() == PointSpatialTree(axisValues, getPosition().getDepth()));

        return result;
    }
    else
    {
        return false;
    }
}

const SpatialTreeTerrainCellInterface* const BranchTerrainCell::getChildTerrainCell(const PointSpatialTree &cellPos) const
{
    #ifdef DEBUG
    if(!isChild(cellPos))
    {
        throwLineInfoException("The target cell is not a descendant of the current cell");
    }
    #endif

    const unsigned int childPosOnVector = calculateChildPositionOnVector(cellPos);
    return getChildTerrainCell(childPosOnVector);
}

SpatialTreeTerrainCellInterface* BranchTerrainCell::getMutableChildTerrainCell(const PointSpatialTree &cellPos)
{
    #ifdef DEBUG
    if(!isChild(cellPos))
    {
        throwLineInfoException("The target cell is not a descendant of the current cell");
    }
    #endif

    const unsigned int childPosOnVector = calculateChildPositionOnVector(cellPos);
    return getMutableChildTerrainCell(childPosOnVector);
}


void BranchTerrainCell::insertAnimal(Animal* const newAnimal)
{
    if(getPosition().getDepth() == newAnimal->getCellDepthOnActualInstar())
    {
        #ifdef DEBUG
        if(isObstacle())
        {
            throwLineInfoException("The animal has been inserted in an obstacle");
        }
        #endif

        TerrainCell::insertAnimal(newAnimal);
    }
    else
    {
        #ifdef DEBUG
        if(!isChild(newAnimal->getPosition()))
        {
            throwLineInfoException("The animal has not been inserted into any child cell");
        }
        #endif

        const unsigned int childIndex = calculateChildPositionOnVector(newAnimal->getPosition());
        auto child = getMutableChildTerrainCell(childIndex);

        #ifdef DEBUG
        if(child->isFullObstacle())
        {
            throwLineInfoException("The animal has been inserted in an obstacle");
        }
        #endif

        try
        {
            child->insertAnimal(newAnimal);
        }
        catch(SpatialTreeTerrainCell::TemporalLeaf2Branch &e)
        {
            auto newChildTerrainCell = convertTemporalLeaf2Branch(childIndex);
            newChildTerrainCell->insertAnimal(newAnimal);

            setChildTerrainCell(childIndex, newChildTerrainCell);
        }
    }
}

tuple<TerrainCellInterface*, Animal*, unsigned int> BranchTerrainCell::randomInsertAnimalOnChild(const Instar &instar, AnimalSpecies* animalSpecies, TerrainCellInterface* child, const bool isStatistical)
{
    for(unsigned int i = 0; i < getChildrenTerrainCells().size(); i++)
    {
        if(getMutableChildTerrainCell(i) == child)
        {
            auto newChildTerrainCell = convertTemporalLeaf2Branch(i);
            auto result = newChildTerrainCell->randomInsertAnimal(instar, animalSpecies, isStatistical);

            setChildTerrainCell(i, newChildTerrainCell);

            return make_tuple<>(getMutableChildTerrainCell(i), get<3>(result), get<4>(result));
        }
    }

    throwLineInfoException("The cell is not a child of the branch");
}

tuple<bool, TerrainCellInterface*, TerrainCellInterface*, Animal*, unsigned int> BranchTerrainCell::randomInsertAnimal(const Instar &instar, AnimalSpecies* animalSpecies, const bool isStatistical)
{
    if(getPosition().getDepth() == animalSpecies->getCellDepthPerInstar()[instar])
    {
        return TerrainCell::randomInsertAnimal(instar, animalSpecies, isStatistical);
    }
    else
    {
        auto randomIndexVector = Random::createIndicesVector(getChildrenTerrainCells().size());

        for(unsigned int i = 0; i < randomIndexVector->size(); i++)
        {
            if(!getChildTerrainCell(randomIndexVector->at(i))->isObstacle())
            {
                try
                {
                    return getMutableChildTerrainCell(randomIndexVector->at(i))->randomInsertAnimal(instar, animalSpecies, isStatistical);
                }
                catch(SpatialTreeTerrainCell::TemporalLeaf2Branch &e)
                {
                    auto newChildTerrainCell = convertTemporalLeaf2Branch(randomIndexVector->at(i));
                    auto result = newChildTerrainCell->randomInsertAnimal(instar, animalSpecies, isStatistical);

                    auto oldTerrainCell = getMutableChildTerrainCell(randomIndexVector->at(i));
                    setChildTerrainCell(randomIndexVector->at(i), newChildTerrainCell);

                    return make_tuple(true, oldTerrainCell, getMutableChildTerrainCell(randomIndexVector->at(i)), get<3>(result), get<4>(result));
                }
            }
        }

        #ifdef DEBUG
        throwLineInfoException("The animal has not been inserted into any child cell");
        #endif
    }

    return make_tuple(false, nullptr, nullptr, nullptr, 0);
}

void BranchTerrainCell::updateMoisture()
{
    if(isInMoisturePatch())
    {
        updateChildrenMoisture();

        TerrainCell::updateMoisture();
    }
}

void BranchTerrainCell::updateChildrenMoisture()
{
    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->updateMoisture();
    }
}

void BranchTerrainCell::update(const unsigned int numberOfTimeSteps, ostream& tuneTraitsFile)
{
    updateChildren(numberOfTimeSteps, tuneTraitsFile);

    TerrainCell::update(numberOfTimeSteps, tuneTraitsFile);
}

void BranchTerrainCell::updateChildren(const unsigned int numberOfTimeSteps, ostream& tuneTraitsFile)
{
    // Create a random vector of indices
    auto indexVector = Random::createIndicesVector(getChildrenTerrainCells().size());

    // Random children updates
    for(const auto &index : *indexVector)
    {
        getMutableChildTerrainCell(index)->update(numberOfTimeSteps, tuneTraitsFile);
    }
}


void BranchTerrainCell::updateObstacleValue()
{
    unsigned int numberOfObstacles = 0;
    int minObstaclePatchPriority = getChildTerrainCell(0)->getObstaclePatchPriority();

    for(unsigned int i = 1; i < getChildrenTerrainCells().size(); i++)
    {
        if(minObstaclePatchPriority >= getChildTerrainCell(i)->getObstaclePatchPriority())
        {
            minObstaclePatchPriority = getChildTerrainCell(i)->getObstaclePatchPriority();
        }

        numberOfObstacles += (getChildTerrainCell(i)->isObstacle()) ? 1 : 0;
    }

    if(numberOfObstacles == getChildrenTerrainCells().size())
    {
        setFullObstacle(true, true, minObstaclePatchPriority);
    }
    else if(numberOfObstacles/(double)getChildrenTerrainCells().size() > 0.5)
    {
        setObstacle(true, minObstaclePatchPriority);
    }
}


void BranchTerrainCell::calculateChildrenPositionsRecursively(unique_ptr<vector<unsigned int>> &childrenPositions,
                                   const unsigned int contactAxis, const bool bottomContactSide, const unsigned int currentCombination, 
                                   const unsigned int indexVector) const
{
    if(indexVector == DIMENSIONS)
    {
        childrenPositions->push_back(currentCombination);
    }
    else
    {
        if(indexVector == contactAxis)
        {
            if(bottomContactSide)
            {
                calculateChildrenPositionsRecursively(
                    childrenPositions, contactAxis, bottomContactSide, 
                    currentCombination, indexVector + 1
                );
            }
            else
            {
                calculateChildrenPositionsRecursively(
                    childrenPositions, contactAxis, bottomContactSide, 
                    currentCombination + displacementPower(1, indexVector), indexVector + 1
                );
            }
        }
        else
        {
            for(unsigned int i = 0; i < SpatialTree::numbreOfSubdivisions; i++)
            {
                calculateChildrenPositionsRecursively(
                    childrenPositions, contactAxis, bottomContactSide, 
                    currentCombination + i*displacementPower(1, indexVector), indexVector + 1
                );
            }
        }
    }
}


unique_ptr<vector<unsigned int>> BranchTerrainCell::calculateChildrenPositions(const PointMap &sourcePosition) const
{
    auto childrenPositions = make_unique<vector<unsigned int>>();
    childrenPositions->reserve(displacementPower(1, DIMENSIONS-1));

    int contactAxis = -1;
    bool bottomContactSide = true;

    for(unsigned int i = 0; i < DIMENSIONS && contactAxis == -1; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();

        if(getPosition().get(axis) != sourcePosition.get(axis))
        {
            contactAxis = i;

            if(getPosition().get(axis) < sourcePosition.get(axis))
            {
                bottomContactSide = false;
            }
        }
    }

    calculateChildrenPositionsRecursively(childrenPositions, contactAxis, bottomContactSide, 0, 0);

    return childrenPositions;
}


pair<bool, bool> BranchTerrainCell::applyMoisturePatch(const MoisturePatch &moisturePatch)
{
    auto coverage = moisturePatch.checkCoverage(getEffectiveArea());

    if(coverage == Coverage::Full)
    {
        if(moisturePatch.getPriority() > getMaximumMoisturePatchPriority())
        {
            return applyFullCoverageMoisturePatch(moisturePatch);
        }
        else
        {
            return applyPartialCoverageMoisturePatch(moisturePatch);
        }
    }
    else if(coverage >= Coverage::Partial)
    {
        return applyPartialCoverageMoisturePatch(moisturePatch);
    }
    else
    {
        return make_pair(false, false);
    }
}

void BranchTerrainCell::updateTotalMaximumResourceCapacity()
{
    double sumTotalMaximumResourceCapacity = accumulate(
        getChildrenTerrainCells().cbegin(), getChildrenTerrainCells().cend(), 0.0,
        [](double accumulated, const auto &child) {
            return accumulated + child->getTotalMaximumResourceCapacity();
        }
    );

    setTotalMaximumResourceCapacity(sumTotalMaximumResourceCapacity);
}

void BranchTerrainCell::setMoistureSourcePatch(const MoisturePatch &moisturePatch)
{
    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->setSubdivisionMoisturePatch(moisturePatch);
    }

    TerrainCell::setMoistureSourcePatch(moisturePatch);
}

void BranchTerrainCell::setMoistureSourcePatch(MoistureInterface* const &newMoistureInfo, const int newMoisturePatchPriority)
{
    setMoisturePatch(newMoistureInfo, newMoisturePatchPriority, newMoisturePatchPriority, true, true);
}

void BranchTerrainCell::setResourceSourcePatch(unique_ptr<ResourceInterface> &newResource, const int newResourcePatchPriority)
{
    setResourcePatch(newResource->getSpecies()->getResourceSpeciesId(), newResource, newResourcePatchPriority, newResourcePatchPriority);
}

void BranchTerrainCell::setSubdivisionMoisturePatch(const MoisturePatch &moisturePatch)
{
    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->setSubdivisionMoisturePatch(moisturePatch);
    }

    SpatialTreeTerrainCell::setSubdivisionMoisturePatch(moisturePatch);
}


pair<bool, bool> BranchTerrainCell::applyPartialCoverageMoisturePatch(const MoisturePatch &moisturePatch)
{
    bool isFullApplied = true, isApplied = false;
    
    for(unsigned int i = 0; i < getChildrenTerrainCells().size(); i++)
    {
        bool isFullChildApplied, isChildApplied;

        try
        {
            tie(isFullChildApplied, isChildApplied) = getMutableChildTerrainCell(i)->checkMoisturePatch(moisturePatch);
        }
        catch(SpatialTreeTerrainCell::TemporalLeaf2Branch &e)
        {
            auto newChildTerrainCell = convertTemporalLeaf2Branch(i);
            tie(isFullChildApplied, isChildApplied) = newChildTerrainCell->applyPartialCoverageMoisturePatch(moisturePatch);

            if(isChildApplied)
            {
                setChildTerrainCell(i, newChildTerrainCell);
            }
        }

        isApplied = isApplied || isChildApplied;
        isFullApplied = isFullApplied && isFullChildApplied;
    }
    
    if(isFullApplied)
    {
        applyFullCoverageMoisturePatch(moisturePatch);
    }
    else if(isApplied)
    {
        setSummaryMoisturePatch();
    }

    return make_pair(isFullApplied, isApplied);
}


const int BranchTerrainCell::getMaximumMoisturePatchPriority() const
{
    return maximumMoisturePatchPriority;
}


const int BranchTerrainCell::getMaximumResourcePatchPriority(const unsigned int resourceSpeciesId) const
{
    return maximumResourcePatchPriority[resourceSpeciesId];
}

void BranchTerrainCell::substractBiomassUp(const unsigned int resourceSpeciesId, double dryMassToBeSubstracted)
{
    getMutableParent()->getMutableResource(resourceSpeciesId)->setBiomass(
        getParent()->getResource(resourceSpeciesId)->getBiomass() - getParent()->getResource(resourceSpeciesId)->getSpecies()->getConversionToWetMass() * dryMassToBeSubstracted
    );
    getMutableParent()->getMutableResource(resourceSpeciesId)->updateFullCapacityStatus();
    
    getMutableParent()->substractBiomassUp(resourceSpeciesId, dryMassToBeSubstracted);
}

void BranchTerrainCell::setMaximumMoisturePatchPriority(const int newMaximumMoisturePatchPriority)
{
    maximumMoisturePatchPriority = newMaximumMoisturePatchPriority;
}


void BranchTerrainCell::setSummaryMoisturePatch()
{
    if(isInMoisturePatch())
    {
        for(auto &child : getMutableChildrenTerrainCells())
        {
            child->checkMoistureSourcePatch(getMoisturePatchPriority());
        }
    }

    int minMoisturePatchPriority = getChildTerrainCell(0)->getMoisturePatchPriority();
    int maxMoisturePatchPriority = getChildTerrainCell(0)->getMaximumMoisturePatchPriority();

    for(unsigned int i = 1; i < getChildrenTerrainCells().size(); i++)
    {
        if(minMoisturePatchPriority > getChildTerrainCell(i)->getMoisturePatchPriority())
        {
            minMoisturePatchPriority = getChildTerrainCell(i)->getMoisturePatchPriority();
        }

        if(maxMoisturePatchPriority < getChildTerrainCell(i)->getMaximumMoisturePatchPriority())
        {
            maxMoisturePatchPriority = getChildTerrainCell(i)->getMaximumMoisturePatchPriority();
        }
    }

    setMoisturePatch(new SummaryMoisture(this), minMoisturePatchPriority, maxMoisturePatchPriority, false, false);

    getMutableMoistureInfo()->updateInfo();
}


void BranchTerrainCell::setMoisturePatch(MoistureInterface* const &newMoistureInfo, const int newMoisturePatchPriority, const bool newMoistureSourceValue, const bool newInMoisturePatchValue)
{
    TerrainCell::setMoisturePatch(newMoistureInfo, newMoisturePatchPriority, newMoistureSourceValue, newInMoisturePatchValue);
    setMaximumMoisturePatchPriority(newMoisturePatchPriority);
}


void BranchTerrainCell::setMoisturePatch(MoistureInterface* const &newMoistureInfo, const int newMoisturePatchPriority, const int newMaximumMoisturePatchPriority, const bool newMoistureSourceValue, const bool newInMoisturePatchValue)
{
    TerrainCell::setMoisturePatch(newMoistureInfo, newMoisturePatchPriority, newMoistureSourceValue, newInMoisturePatchValue);
    setMaximumMoisturePatchPriority(newMaximumMoisturePatchPriority);
}


void BranchTerrainCell::setChildTerrainCell(const unsigned int childIndex, unique_ptr<BranchTerrainCell> &newChild)
{
    newChild->setCopied(false);
    childrenTerrainCells[childIndex]->setCopied(true);
    
    childrenTerrainCells[childIndex] = move(newChild);
    childrenTerrainCellPointers[childIndex] = childrenTerrainCells[childIndex].get();
    childrenTerrainCellConstPointers[childIndex] = childrenTerrainCells[childIndex].get();

    static_cast<BranchTerrainCell*>(getMutableChildTerrainCell(childIndex))->refreshAnimalPosition();
}


unique_ptr<BranchTerrainCell> BranchTerrainCell::convertTemporalLeaf2Branch(const unsigned int childIndex)
{
    auto newChildTerrainCell = make_unique<BranchTerrainCell>(static_cast<TemporalLeafTerrainCell&>(*getMutableChildTerrainCell(childIndex)));
    newChildTerrainCell->setCopied(true);

    return newChildTerrainCell;
}


bool BranchTerrainCell::applyResourcePatch(const ResourcePatch &resourcePatch)
{
    auto coverage = resourcePatch.checkCoverage(getEffectiveArea());

    if(coverage == Coverage::Full)
    {
        if(resourcePatch.getPriority() > getMaximumResourcePatchPriority(resourcePatch.getSpecies()->getResourceSpeciesId()))
        {
            return applyFullCoverageResourcePatch(resourcePatch);
        }
        else
        {
            return applyPartialCoverageResourcePatch(resourcePatch);
        }
    }
    else if(coverage >= Coverage::Partial)
    {
        return applyPartialCoverageResourcePatch(resourcePatch);
    }
    else
    {
        return false;
    }
}


bool BranchTerrainCell::applyFullCoverageResourcePatch(const ResourcePatch &resourcePatch)
{
    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->checkFullCoverageResourcePatch(resourcePatch);
    }

    setSummaryResourcePatch(resourcePatch.getSpecies()->getResourceSpeciesId());

    return true;
}


void BranchTerrainCell::refreshAnimalPosition()
{
    for(auto &lifeStage : *animals)
    {
        for(auto &animalSpecies : lifeStage)
        {
            for(auto &instar : animalSpecies)
            {
                for(auto &gender : instar)
                {
                    for(auto &animal : gender)
                    {
                        animal->setTerrainCellInterface(this);
                    }
                }
            }
        }
    }
}


void BranchTerrainCell::setSummaryResourcePatch(const unsigned int resourceSpeciesId)
{
    int minResourcePatchPriority = getChildTerrainCell(0)->getMoisturePatchPriority();
    int maxResourcePatchPriority = getChildTerrainCell(0)->getMaximumMoisturePatchPriority();

    for(const auto &child : getChildrenTerrainCells())
    {
        if(minResourcePatchPriority > child->getResourcePatchPriority(resourceSpeciesId))
        {
            minResourcePatchPriority = child->getResourcePatchPriority(resourceSpeciesId);
        }

        if(maxResourcePatchPriority < child->getMaximumResourcePatchPriority(resourceSpeciesId))
        {
            maxResourcePatchPriority = child->getMaximumResourcePatchPriority(resourceSpeciesId);
        }
    }

    unique_ptr<ResourceInterface> newResource = make_unique<SummaryResource>(resourceSpeciesId, this);
    setResourcePatch(resourceSpeciesId, newResource, minResourcePatchPriority, maxResourcePatchPriority);
}

void BranchTerrainCell::setMaximumResourcePatchPriority(const unsigned int resourceSpeciesId, const int newMaximumResourcePatchPriority)
{
    maximumResourcePatchPriority[resourceSpeciesId] = newMaximumResourcePatchPriority;
}


void BranchTerrainCell::setResourcePatch(const unsigned int resourceSpeciesId, unique_ptr<ResourceInterface> &newResource, const int newResourcePatchPriority)
{
    throwLineInfoException("This method cannot be used with this class");
}


void BranchTerrainCell::setResourcePatch(const unsigned int resourceSpeciesId, unique_ptr<ResourceInterface> &newResource, const int newResourcePatchPriority, const int newMaximumResourcePatchPriority)
{
    TerrainCell::setResourcePatch(resourceSpeciesId, newResource, newResourcePatchPriority);
    setMaximumResourcePatchPriority(resourceSpeciesId, newMaximumResourcePatchPriority);
}


bool BranchTerrainCell::applyPartialCoverageResourcePatch(const ResourcePatch &resourcePatch)
{
    bool isApplied = false;

    for(unsigned int i = 0; i < getChildrenTerrainCells().size(); i++)
    {
        bool isChildApplied;

        try
        {
            isChildApplied = getMutableChildTerrainCell(i)->checkResourcePatch(resourcePatch);
        }
        catch(SpatialTreeTerrainCell::TemporalLeaf2Branch &e)
        {
            auto newChildTerrainCell = convertTemporalLeaf2Branch(i);
            isChildApplied = newChildTerrainCell->applyPartialCoverageResourcePatch(resourcePatch);

            if(isChildApplied)
            {
                setChildTerrainCell(i, newChildTerrainCell);
            }
        }

        isApplied = isApplied || isChildApplied;
    }

    if(isApplied)
    {
        setSummaryResourcePatch(resourcePatch.getSpecies()->getResourceSpeciesId());
    }

    return isApplied;
}


bool BranchTerrainCell::applyObstaclePatch(const ObstaclePatch &obstaclePatch)
{
    auto coverage = obstaclePatch.checkCoverage(getEffectiveArea());

    if(coverage == Coverage::Full)
    {
        return applyFullCoverageObstaclePatch(obstaclePatch);
    }
    else if(coverage >= Coverage::Partial)
    {
        return applyPartialCoverageObstaclePatch(obstaclePatch);
    }
    else
    {
        return false;
    }
}


bool BranchTerrainCell::applyFullCoverageObstaclePatch(const ObstaclePatch &obstaclePatch)
{
    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->checkFullCoverageObstaclePatch(obstaclePatch);
    }

    setFullObstacle(obstaclePatch);

    return true;
}


bool BranchTerrainCell::applyPartialCoverageObstaclePatch(const ObstaclePatch &obstaclePatch)
{
    bool isApplied = false;

    for(unsigned int i = 0; i < getChildrenTerrainCells().size(); i++)
    {
        bool isChildApplied;

        try
        {
            isChildApplied = getMutableChildTerrainCell(i)->checkObstaclePatch(obstaclePatch);
        }
        catch(SpatialTreeTerrainCellInterface::TemporalLeaf2Branch &e)
        {
            auto newChildTerrainCell = convertTemporalLeaf2Branch(i);
            isChildApplied = newChildTerrainCell->applyPartialCoverageObstaclePatch(obstaclePatch);

            if(isChildApplied)
            {
                setChildTerrainCell(i, newChildTerrainCell);
            }
        }

        if(isChildApplied)
        {
            isApplied = true;
        }
    }

    if(isApplied)
    {
        updateObstacleValue();
        return true;
    }
    else
    {
        return false;
    }
}


void BranchTerrainCell::obtainInhabitableTerrainCells()
{
    if(!isFullObstacle())
    {
        TerrainCell::obtainInhabitableTerrainCells();

        for(auto &child : getMutableChildrenTerrainCells())
        {
            child->obtainInhabitableTerrainCells();
        }
    }
}

const bool BranchTerrainCell::isLeaf() const
{
    return false;
}

void BranchTerrainCell::eraseAllAnimals()
{
    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->eraseAllAnimals();
    }

    TerrainCell::eraseAllAnimals();
}


void BranchTerrainCell::purgeDeadAnimals()
{
    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->purgeDeadAnimals();
    }

    TerrainCell::purgeDeadAnimals();
}

unique_ptr<FullCoverageAnimals> BranchTerrainCell::getMutableAnimalsDown(const AnimalSearchParams &animalSearchParams)
{
    auto searchableAnimals = make_unique<FullCoverageAnimals>();

    for(auto &child : getMutableChildrenTerrainCells())
    {
        if(!child->isFullObstacle())
        {
            if(!child->isObstacle())
            {
                auto animalsOnChildCell = child->getMutableAnimalsBy(animalSearchParams);
                searchableAnimals->push_back(move(animalsOnChildCell));
            }

            auto animalsDown = child->getMutableAnimalsDown(animalSearchParams);
            for(auto &animals : *animalsDown)
            {
                searchableAnimals->push_back(move(animals));
            }
        }
    }

    return searchableAnimals;
}

EdiblesOnRadius BranchTerrainCell::getMutableEdiblesDown(
        function<bool(Animal&)> downChecker, const Ring &effectiveArea,
        const EdibleSearchParams &edibleSearchParams)
{
    auto searchableResources = make_unique<ResourcesOnRadius>(edibleSearchParams.getResourceSearchParams().getSearchParams().size(), 0.0);
    auto searchablePartialCoverageAnimals = make_unique<PartialCoverageAnimals>();
    auto searchableFullCoverageAnimals = make_unique<FullCoverageAnimals>();

    for(auto &child : getMutableChildrenTerrainCells())
    {
        if(!child->isFullObstacle())
        {
            unique_ptr<Ring> childIntersection = Geometry::calculateIntersection(effectiveArea, child->getEffectiveArea());

            auto coverage = Geometry::checkCoverage(childIntersection.get(), child->getEffectiveArea());

            if(coverage == Coverage::Full)
            {
                if(!child->isObstacle())
                {
                    unique_ptr<ResourcesOnRadius> resourcesOnChildCell;
                    unique_ptr<vector<AnimalVector*>> animalsOnChildCell;

                    tie(resourcesOnChildCell, animalsOnChildCell) = child->getMutableEdiblesBy(
                        edibleSearchParams
                    );

                    for(unsigned int i = 0; i < resourcesOnChildCell->size(); i++)
                    {
                        searchableResources->at(i) += resourcesOnChildCell->at(i);
                    }

                    searchableFullCoverageAnimals->push_back(move(animalsOnChildCell));
                }

                auto fullCoverageAnimalsDown = child->getMutableAnimalsDown(edibleSearchParams.getAnimalSearchParams());
                for(auto &fullCoverageAnimals : *fullCoverageAnimalsDown)
                {
                    searchableFullCoverageAnimals->push_back(move(fullCoverageAnimals));
                }
            }
            else if(coverage >= Coverage::Partial)
            {
                if(!child->isObstacle())
                {
                    auto animalsOnChildCell = child->getMutableAnimalsBy(downChecker, edibleSearchParams.getAnimalSearchParams());
                    searchablePartialCoverageAnimals->push_back(move(animalsOnChildCell));
                }

                auto ediblesDown = child->getMutableEdiblesDown(
                    downChecker, *childIntersection, edibleSearchParams
                );

                unique_ptr<ResourcesOnRadius> resourcesDown = move(ediblesDown.first);
                unique_ptr<PartialCoverageAnimals> partialCoverageAnimalsDown = move(ediblesDown.second.first);
                unique_ptr<FullCoverageAnimals> fullCoverageAnimalsDown = move(ediblesDown.second.second);

                for(unsigned int i = 0; i < resourcesDown->size(); i++)
                {
                    searchableResources->at(i) += resourcesDown->at(i);
                }

                for(auto &partialCoverageAnimals : *partialCoverageAnimalsDown)
                {
                    searchablePartialCoverageAnimals->push_back(move(partialCoverageAnimals));
                }

                for(auto &fullCoverageAnimals : *fullCoverageAnimalsDown)
                {
                    searchableFullCoverageAnimals->push_back(move(fullCoverageAnimals));
                }
            }
        }
    }

    return make_pair(move(searchableResources), make_pair(move(searchablePartialCoverageAnimals), move(searchableFullCoverageAnimals)));
}

unique_ptr<vector<pair<TerrainCellInterface*, pair<TerrainCellInterface::TerrainCellCoverage, unique_ptr<Ring>>>>> BranchTerrainCell::getRadiusTerrainCells(
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
        for(auto &child : getMutableChildrenTerrainCells())
        {
            if(Geometry::checkCoverage(&radiusArea, child->getEffectiveArea()) >= Coverage::Partial)
            {
                auto childRadiusTerrainCells = child->getRadiusTerrainCells(radiusArea, searchDepth);

                for(auto &terrainCell : *childRadiusTerrainCells)
                {
                    radiusTerrainCells->push_back(move(terrainCell));
                }
            }
        } 
    }

    return radiusTerrainCells;
}


void BranchTerrainCell::printAnimals(ofstream &file) const
{
    if(!isFullObstacle())
    {
        for(const auto &child : getChildrenTerrainCells())
        {
            child->printAnimals(file);
        }

        TerrainCell::printAnimals(file);
    }
}


void BranchTerrainCell::printCell(vector<pair<vector<double>, vector<unsigned int>>> &mapCellsInfo)
{
    if(!isFullObstacle())
    {
        for(auto &child : getMutableChildrenTerrainCells())
        {
            child->printCell(mapCellsInfo);
        }

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
    }
}


void BranchTerrainCell::obtainWorldAnimalsPopulation(vector<vector<unsigned int>> &worldAnimalsPopulation)
{
    if(!isFullObstacle())
    {
        for(auto &child : getMutableChildrenTerrainCells())
        {
            child->obtainWorldAnimalsPopulation(worldAnimalsPopulation);
        }

        TerrainCell::obtainWorldAnimalsPopulation(worldAnimalsPopulation);
    }
}


void BranchTerrainCell::obtainAnimalsPopulationAndGeneticsFrequencies(vector<vector<unsigned int>> &worldAnimalsPopulation, vector<vector<pair<vector<double>, vector<double>>>> &worldGeneticsFrequencies)
{
    if(!isFullObstacle())
    {
        for(auto &child : getMutableChildrenTerrainCells())
        {
            child->obtainAnimalsPopulationAndGeneticsFrequencies(worldAnimalsPopulation, worldGeneticsFrequencies);
        }

        TerrainCell::obtainAnimalsPopulationAndGeneticsFrequencies(worldAnimalsPopulation, worldGeneticsFrequencies);
    }
}


void BranchTerrainCell::saveAnimalSpeciesSnapshot(ofstream &file, const AnimalSpecies* const &species)
{
    for(auto &child : getMutableChildrenTerrainCells())
    {
        child->saveAnimalSpeciesSnapshot(file, species);
    }

    TerrainCell::saveAnimalSpeciesSnapshot(file, species);
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
    if(!isFullObstacle())
    {
        for(const auto &child : getChildrenTerrainCells())
        {
            child->saveWaterSnapshot(file);
        }

        TerrainCell::saveWaterSnapshot(file);
    }
}

void BranchTerrainCell::moveAnimals(const unsigned int numberOfTimeSteps, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, ostream& edibilitiesFile, float exitTimeThreshold, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
    auto indexVector = Random::createIndicesVector(getChildrenTerrainCells().size() + 1);

    for(const auto &index : *indexVector)
    {
        if(index == getChildrenTerrainCells().size())
        {
            TerrainCell::moveAnimals(numberOfTimeSteps, encounterProbabilitiesFile, predationProbabilitiesFile, saveEdibilitiesFile, edibilitiesFile, exitTimeThreshold, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
        }
        else
        {
            getMutableChildTerrainCell(index)->moveAnimals(numberOfTimeSteps, encounterProbabilitiesFile, predationProbabilitiesFile, saveEdibilitiesFile, edibilitiesFile, exitTimeThreshold, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
        }
    }
}

void BranchTerrainCell::performAnimalsActions(const unsigned int numberOfTimeSteps, ostream& voracitiesFile, fs::path outputFolder, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
    auto indexVector = Random::createIndicesVector(getChildrenTerrainCells().size() + 1);

    for(const auto &index : *indexVector)
    {
        if(index == getChildrenTerrainCells().size())
        {
            TerrainCell::performAnimalsActions(numberOfTimeSteps, voracitiesFile, outputFolder, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
        }
        else
        {
            getMutableChildTerrainCell(index)->performAnimalsActions(numberOfTimeSteps, voracitiesFile, outputFolder, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
        }
    }
}
