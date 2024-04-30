#ifndef TEMPORAL_LEAF_TERRAINCELL_H_
#define TEMPORAL_LEAF_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>

#include "IBM/World/Map/SpatialTree/TerrainCells/LeafTerrainCell.h"
#include "IBM/World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "IBM/World/Map/Geometry/Coverage.h"



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
    TemporalLeafTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, std::vector<ResourceInterface*>* const parentResources, const std::vector<int>* const parentResourcePatchPriority);
    virtual ~TemporalLeafTerrainCell();

    void insertAnimal(Animal* const newAnimal) override;
    std::tuple<bool, TerrainCellInterface*, TerrainCellInterface*, Animal*, unsigned int> randomInsertAnimal(const Instar &instar, AnimalSpecies* animalSpecies, const bool isStatistical) override;
    void printCell(std::vector<std::pair<std::vector<double>, std::vector<unsigned int>>> &mapCellsInfo) override;
};

#endif /* TEMPORAL_LEAF_TERRAINCELL_H_ */
