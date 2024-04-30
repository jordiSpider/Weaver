#ifndef SPATIAL_TREE_TERRAINCELL_H_
#define SPATIAL_TREE_TERRAINCELL_H_


#include <vector>
#include <cmath>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCellInterface.h"
#include "IBM/World/Map/TerrainCells/Moisture/SubdivisionMoisture.h"



class SpatialTreeInterface;

class BranchTerrainCellInterface;


class SpatialTreeTerrainCell : public SpatialTreeTerrainCellInterface
{
protected:
    BranchTerrainCellInterface* parentTerrainCell;

    /**
     * @name Patches
     * @{
     */

    /**
     * @name Obstacle patches
     * @{
     */
    bool checkFullCoverageObstaclePatch(const ObstaclePatch &obstaclePatch); //?
    virtual bool applyFullCoverageObstaclePatch(const ObstaclePatch &obstaclePatch)=0; //?
    virtual bool applyPartialCoverageObstaclePatch(const ObstaclePatch &obstaclePatch)=0; //?
    /** @} */

    /**
     * @name Moisture patches
     * @{
     */
    std::pair<bool, bool> checkFullCoverageMoisturePatch(MoisturePatch &moisturePatch); //?
    std::pair<bool, bool> applyFullCoverageMoisturePatch(MoisturePatch &moisturePatch); //?
    void setSubdivisionMoisturePatch(ExtendedMoisture* const moistureInfo, const int moisturePathPriority);
    bool canApplyMoistureSourcePatch(const int moisturePathPriority); //?
    void checkMoistureSourcePatch(const int moisturePathPriority); //?
    void applyMoistureSourcePatch(); //?
    /** @} */

    /**
     * @name Resource patches
     * @{
     */
    bool checkFullCoverageResourcePatch(const ResourcePatch &resourcePatch); //?
    bool canApplyFullCoverageResourcePatch(const ResourcePatch &resourcePatch) const;
    virtual bool applyFullCoverageResourcePatch(const ResourcePatch &resourcePatch)=0; //?
    /** @} */

    /** @} */

public:
    static const unsigned int numberOfChildren;


    SpatialTreeTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, SpatialTreeInterface* const mapInterface);
    SpatialTreeTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, const Ring *const effectiveArea, const double &size, 
        SpatialTreeInterface* const mapInterface, LifeStageVector* const animals, 
        const bool obstacle, const bool fullObstacle, 
        const int obstaclePatchPriority, MoistureInterface* const moistureInfo, const bool moistureSource, 
        const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity);
    virtual ~SpatialTreeTerrainCell();

    const BranchTerrainCellInterface* const getParent() const;
    BranchTerrainCellInterface* const getMutableParent() const;
    std::pair<bool, std::pair<TerrainCellInterface*, PointContinuous>> getCellByBearing(
        const std::pair<PointMap, PointContinuous> &targetNeighborToTravelTo, const PointContinuous &animalPosition
    );
    void migrateAnimalTo(AnimalInterface* animalToMigrate, TerrainCellInterface* newTerrainCell, const PointContinuous &newPosition);

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
    SearchableEdibles getMutableEdiblesOnAllCell(const EdibleSearchParams &edibleSearchParams);
    EdiblesOnRadius getMutableEdiblesOnCellAndDown(const EdibleSearchParams &edibleSearchParams);
    /** @} */

    /**
     * @name On radius
     * @{
     */
    virtual SearchableEdibles getEdiblesOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const Ring &radiusArea, const EdibleSearchParams &edibleSearchParams
    );
    SearchableEdibles getEdiblesOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const EdibleSearchParams &edibleSearchParams
    );
    /** @} */

    /**
     * @name On area
     * @{
     */

    SearchableEdibles getMutableEdiblesOnAllCell(
        TerrainCellInterface::TerrainCellCoverage coverage, const PointContinuous &sourcePosition, const double &radius, 
        const Ring &effectiveArea, const EdibleSearchParams &edibleSearchParams
    );
    virtual EdiblesOnRadius getMutableEdiblesOnCellAndDown(
        std::function<bool(AnimalInterface&)> downChecker, const Ring &effectiveArea,
        const EdibleSearchParams &edibleSearchParams
    );

    /**
     * @name Partial area
     * @{
     */
    SearchableEdibles getMutableEdiblesOnAllCell(
        std::function<bool(AnimalInterface&)> upChecker, std::function<bool(AnimalInterface&)> downChecker, 
        const Ring &effectiveArea, const EdibleSearchParams &edibleSearchParams
    );
    /** @} */

    /**
     * @name Total area
     * @{
     */
    SearchableEdibles getMutableEdiblesOnAllCell(
        std::function<bool(AnimalInterface&)> upChecker, const EdibleSearchParams &edibleSearchParams
    );
    /** @} */

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
    std::unique_ptr<FullCoverageAnimals> getMutableAnimalsOnAllCell(const EdibleSearchParams &edibleSearchParams);
    std::unique_ptr<const FullCoverageAnimals> getAnimalsOnAllCell(const EdibleSearchParams &edibleSearchParams);
    virtual std::unique_ptr<FullCoverageAnimals> getMutableAnimalsUp(const AnimalSearchParams &animalSearchParams);
    /** @} */

    /**
     * @name On area
     * @{
     */
    virtual std::unique_ptr<PartialCoverageAnimals> getMutableAnimalsUp(
        std::function<bool(AnimalInterface&)> upChecker, const AnimalSearchParams &animalSearchParams
    );
    /** @} */

    /** @} */

    /** @} */

    virtual std::unique_ptr<std::vector<std::pair<TerrainCellInterface*, std::pair<TerrainCellInterface::TerrainCellCoverage, std::unique_ptr<Ring>>>>> getNeighboursCellsOnRadius(
        const Ring &radiusArea, const unsigned int searchDepth
    );
    std::unique_ptr<std::vector<std::pair<TerrainCellInterface*, std::pair<TerrainCellInterface::TerrainCellCoverage, std::unique_ptr<Ring>>>>> getNeighboursCellsOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const unsigned int searchDepth
    );


    /**
     * @name Patches
     * @{
     */

    /**
     * @name Moisture patches
     * @{
     */
    virtual void setSubdivisionMoisturePatch(MoisturePatch &moisturePatch); //?
    /** @} */

    /** @} */


    template <class Archive>
    void serialize(Archive &ar, const unsigned int version, std::vector<ExtendedMoisture*>& appliedMoisture);
};

#endif /* SPATIAL_TREE_TERRAINCELL_H_ */
