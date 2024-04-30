#ifndef TEMPORAL_LEAF_TERRAINCELL_H_
#define TEMPORAL_LEAF_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>

#include "World/Map/SpatialTree/TerrainCells/LeafTerrainCell.h"
#include "World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "World/Map/Geometry/Coverage.h"



class TemporalLeafTerrainCell : public LeafTerrainCell
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
    bool applyObstaclePatch(const ObstaclePatch &obstaclePatch) override; //?
    /** @} */

    /**
     * @name Moisture patches
     * @{
     */
    std::pair<bool, bool> applyMoisturePatch(const MoisturePatch &moisturePatch) override; //?
    /** @} */

    /**
     * @name Resource patches
     * @{
     */
    bool applyResourcePatch(const ResourcePatch &resourcePatch) override; //?
    /** @} */

    /** @} */

public:
    TemporalLeafTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, const std::vector<const ResourceInterface*>* const parentResources, const std::vector<int>* const parentResourcePatchPriority);
    virtual ~TemporalLeafTerrainCell();

    void insertAnimal(AnimalInterface* const newAnimal) override;
    std::tuple<bool, TerrainCellInterface*, TerrainCellInterface*> randomInsertAnimal(AnimalInterface* const newAnimal) override;
    void printCell(std::vector<std::pair<std::vector<double>, std::vector<unsigned int>>> &mapCellsInfo) override;
};

#endif /* TEMPORAL_LEAF_TERRAINCELL_H_ */
