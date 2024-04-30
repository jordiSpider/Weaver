#ifndef TEMPORAL_LEAF_TERRAINCELL_H_
#define TEMPORAL_LEAF_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "IBM/World/Map/SpatialTree/TerrainCells/LeafTerrainCell.h"
#include "IBM/World/Map/Points/PointSpatialTree.h"
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
    std::pair<bool, bool> applyMoisturePatch(MoisturePatch &moisturePatch) override; //?
    /** @} */

    /**
     * @name Resource patches
     * @{
     */
    bool applyResourcePatch(const ResourcePatch &resourcePatch) override; //?
    /** @} */

    /** @} */

public:
    TemporalLeafTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, SpatialTreeInterface* const mapInterface);
    TemporalLeafTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, const std::vector<ResourceInterface*>* const parentResources, const std::vector<int>* const parentResourcePatchPriority);
    virtual ~TemporalLeafTerrainCell();

    void insertAnimal(AnimalInterface* const newAnimal) override;
    std::tuple<bool, TerrainCellInterface*, TerrainCellInterface*> randomInsertAnimal(AnimalInterface* const newAnimal) override;
    void printCell(std::vector<std::pair<std::vector<double>, std::vector<unsigned int>>> &mapCellsInfo) override;

    const Type getSpatialTreeTerrainCellType() const override;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version, std::vector<ExtendedMoisture*>& appliedMoisture);
};

#endif /* TEMPORAL_LEAF_TERRAINCELL_H_ */
