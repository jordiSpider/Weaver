#ifndef SPATIAL_TREE_TERRAINCELL_INTERFACE_H_
#define SPATIAL_TREE_TERRAINCELL_INTERFACE_H_


#include <vector>
#include <exception>

#include "IBM/World/Map/TerrainCells/TerrainCell.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "IBM/World/Map/SpatialTree/Points/PointSpatialTree.h"


class SpatialTreeInterface;

class BranchTerrainCellInterface;


class SpatialTreeTerrainCellInterface : public TerrainCell
{
protected:
    /**
     * @name Patches
     * @{
     */

    /**
     * @name Moisture patches
     * @{
     */
    virtual std::pair<bool, bool> applyPartialCoverageMoisturePatch(const MoisturePatch &moisturePatch)=0; //?
    /** @} */

    /**
     * @name Resource patches
     * @{
     */
    virtual bool applyPartialCoverageResourcePatch(const ResourcePatch &resourcePatch)=0; //?
    /** @} */

    /** @} */

public:
    class TemporalLeaf2Branch : public std::exception {};

    SpatialTreeTerrainCellInterface(PointSpatialTree* const &position, const Ring *const effectiveArea, const double &size, SpatialTreeInterface* const &mapInterface,
        LifeStageVector* const animals, const bool obstacle, const bool fullObstacle, 
        const int obstaclePatchPriority, MoistureInterface* const moistureInfo, const bool moistureSource, 
        const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity);
    virtual ~SpatialTreeTerrainCellInterface();

    virtual const BranchTerrainCellInterface* const getParent() const=0;
    virtual BranchTerrainCellInterface* const getMutableParent() const=0;
    virtual SpatialTreeTerrainCellInterface* const getCell(const PointSpatialTree &dstPos)=0;

    const PointSpatialTree& getPosition() const;
    PointSpatialTree& getMutablePosition() const;

    virtual bool isChild(const PointSpatialTree &childPos) const=0;

    virtual const bool isLeaf() const=0;

    virtual std::unique_ptr<std::vector<std::pair<TerrainCellInterface*, std::pair<TerrainCellInterface::TerrainCellCoverage, std::unique_ptr<Ring>>>>> getRadiusTerrainCells(
        const Ring &radiusArea, const unsigned int searchDepth
    )=0;


    /**
     * @name Getters
     * @{
     */

    /**
     * @name Edibles
     * @{
     */

    /**
     * @name Non-conditional
     * @{
     */
    virtual EdiblesOnRadius getMutableEdiblesOnCellAndDown(const EdibleSearchParams &edibleSearchParams)=0;
    /** @} */

    /**
     * @name On area
     * @{
     */
    virtual EdiblesOnRadius getMutableEdiblesOnCellAndDown(
        std::function<bool(AnimalInterface&)> downChecker, const Ring &effectiveArea,
        const EdibleSearchParams &edibleSearchParams
    )=0;
    virtual EdiblesOnRadius getMutableEdiblesDown(
        std::function<bool(AnimalInterface&)> downChecker, const Ring &effectiveArea,
        const EdibleSearchParams &edibleSearchParams
    )=0;
    /** @} */

    /** @} */

    /**
     * @name Animals
     * @{
     */

    /**
     * @name Non-conditional
     * @{
     */
    virtual std::unique_ptr<FullCoverageAnimals> getMutableAnimalsUp(const AnimalSearchParams &animalSearchParams)=0;
    virtual std::unique_ptr<FullCoverageAnimals> getMutableAnimalsDown(const AnimalSearchParams &animalSearchParams)=0;
    /** @} */

    /**
     * @name On area
     * @{
     */
    virtual std::unique_ptr<PartialCoverageAnimals> getMutableAnimalsUp(
        std::function<bool(AnimalInterface&)> upChecker, const AnimalSearchParams &animalSearchParams
    )=0;
    /** @} */

    /** @} */



    /**
     * @name Patches
     * @{
     */

    /**
     * @name Obstacle patches
     * @{
     */
    virtual bool checkFullCoverageObstaclePatch(const ObstaclePatch &obstaclePatch)=0; //?
    /** @} */

    /**
     * @name Moisture patches
     * @{
     */
    virtual std::pair<bool, bool> checkFullCoverageMoisturePatch(const MoisturePatch &moisturePatch)=0; //?
    virtual const int getMaximumMoisturePatchPriority() const=0; //?
    virtual void checkMoistureSourcePatch(const int moisturePathPriority)=0; //?
    virtual void setSubdivisionMoisturePatch(const MoisturePatch &moisturePatch)=0;
    /** @} */

    /**
     * @name Resource patches
     * @{
     */
    virtual bool checkFullCoverageResourcePatch(const ResourcePatch &resourcePatch)=0; //?
    virtual const int getMaximumResourcePatchPriority(const unsigned int resourceSpeciesId) const=0; //?
    /** @} */

    /** @} */
};

#endif /* SPATIAL_TREE_TERRAINCELL_INTERFACE_H_ */
