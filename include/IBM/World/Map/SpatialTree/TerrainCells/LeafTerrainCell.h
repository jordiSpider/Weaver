#ifndef LEAF_TERRAINCELL_H_
#define LEAF_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>

#include "IBM/World/Map/SpatialTree/TerrainCells/LeafTerrainCellInterface.h"
#include "IBM/World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "Misc/Types.h"
#include "IBM/World/Map/Geometry/Coverage.h"
#include "IBM/World/Map/TerrainCells/Moisture/NullMoisture.h"


// Forward declaration

class BranchTerrainCell;


class LeafTerrainCell : public LeafTerrainCellInterface
{
protected:
    /**
     * @name Patches
     * @{
     */

    /**
     * @name Obstacle patches
     * @{
     */
    virtual bool applyObstaclePatch(const ObstaclePatch &obstaclePatch); //?
    bool applyFullCoverageObstaclePatch(const ObstaclePatch &obstaclePatch); //?
    bool applyPartialCoverageObstaclePatch(const ObstaclePatch &obstaclePatch); //?
    /** @} */

    /**
     * @name Moisture patches
     * @{
     */
    virtual std::pair<bool, bool> applyMoisturePatch(const MoisturePatch &moisturePatch); //?
    std::pair<bool, bool> applyPartialCoverageMoisturePatch(const MoisturePatch &moisturePatch); //?
    const int getMaximumMoisturePatchPriority() const; //?
    /** @} */

    /**
     * @name Resource patches
     * @{
     */
    virtual bool applyResourcePatch(const ResourcePatch &resourcePatch); //?
    bool applyFullCoverageResourcePatch(const ResourcePatch &resourcePatch); //?
    bool applyPartialCoverageResourcePatch(const ResourcePatch &resourcePatch); //?
    const int getMaximumResourcePatchPriority(const unsigned int resourceSpeciesId) const; //?
    /** @} */

    /** @} */

public:
    LeafTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, std::vector<ResourceInterface*>* const parentResources, const std::vector<int>* const parentResourcePatchPriority);
    virtual ~LeafTerrainCell();

    SpatialTreeTerrainCellInterface* const getCell(const PointSpatialTree &cellPos);
    SpatialTreeTerrainCell* const& getChildTerrainCell(const PointSpatialTree &cellPos) const;
    
    std::unique_ptr<std::vector<std::pair<TerrainCellInterface*, std::pair<TerrainCellInterface::TerrainCellCoverage, std::unique_ptr<Ring>>>>> getRadiusTerrainCells(
        const Ring &radiusArea, const unsigned int searchDepth
    );

    bool isChild(const PointSpatialTree &childPos) const;

    /**
     * @name Getters
     * @{
     */

    /**
     * @name Edibles
     * @{
     */

    /**
     * @name On radius
     * @{
     */
    EdiblesOnRadius getMutableEdiblesOnCellAndDown(
        std::function<bool(Animal&)> downChecker, const Ring &effectiveArea,
        const EdibleSearchParams &edibleSearchParams
    ) override;
    EdiblesOnRadius getMutableEdiblesDown(
        std::function<bool(Animal&)> downChecker, const Ring &effectiveArea,
        const EdibleSearchParams &edibleSearchParams
    );
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
    std::unique_ptr<FullCoverageAnimals> getMutableAnimalsDown(const AnimalSearchParams &animalSearchParams);
    /** @} */

    /**
     * @name On radius
     * @{
     */
    
    /** @} */

    /** @} */

    /** @} */

    
    const bool isLeaf() const;
    virtual void printCell(std::vector<std::pair<std::vector<double>, std::vector<unsigned int>>> &mapCellsInfo);
};

#endif /* LEAF_TERRAINCELL_H_ */
