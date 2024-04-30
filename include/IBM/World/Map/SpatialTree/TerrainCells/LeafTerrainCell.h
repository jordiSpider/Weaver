#ifndef LEAF_TERRAINCELL_H_
#define LEAF_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "IBM/World/Map/SpatialTree/TerrainCells/LeafTerrainCellInterface.h"
#include "IBM/World/Map/Points/PointSpatialTree.h"
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
    virtual std::pair<bool, bool> applyMoisturePatch(MoisturePatch &moisturePatch); //?
    std::pair<bool, bool> applyPartialCoverageMoisturePatch(MoisturePatch &moisturePatch); //?
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
    LeafTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, SpatialTreeInterface* const mapInterface);
    LeafTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, const std::vector<ResourceInterface*>* const parentResources, const std::vector<int>* const parentResourcePatchPriority);
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
        std::function<bool(AnimalInterface&)> downChecker, const Ring &effectiveArea,
        const EdibleSearchParams &edibleSearchParams
    ) override;
    EdiblesOnRadius getMutableEdiblesDown(
        std::function<bool(AnimalInterface&)> downChecker, const Ring &effectiveArea,
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

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version, std::vector<ExtendedMoisture*>& appliedMoisture);
};

#endif /* LEAF_TERRAINCELL_H_ */
