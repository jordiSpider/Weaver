#ifndef ROOT_TERRAINCELL_H_
#define ROOT_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>
#include <memory>

#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "IBM/World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "Exceptions/LineInfoException.h"



class RootTerrainCell : public BranchTerrainCell
{
public:
    RootTerrainCell(SpatialTree* const map);
    virtual ~RootTerrainCell();

    const BranchTerrainCell* const getParent() const override;
    BranchTerrainCell* const getMutableParent() const override;

    void obtainWorldResourceBiomassAndAnimalsPopulation(std::vector<double> &worldResourceBiomass, std::vector<std::vector<unsigned int>> &worldAnimalsPopulation);

    void substractBiomassUp(const unsigned int resourceSpeciesId, double dryMassToBeSubstracted) override;

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
    SearchableEdibles getEdiblesOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const Ring &radiusArea, const EdibleSearchParams &edibleSearchParams
    ) override;
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
    std::unique_ptr<FullCoverageAnimals> getMutableAnimalsUp(const AnimalSearchParams &animalSearchParams) override;
    /** @} */

    /**
     * @name On area
     * @{
     */
    std::unique_ptr<PartialCoverageAnimals> getMutableAnimalsUp(
        std::function<bool(Animal&)> upChecker, const AnimalSearchParams &animalSearchParams
    ) override;
    /** @} */

    /** @} */

    /** @} */

    std::unique_ptr<std::vector<std::pair<TerrainCellInterface*, std::pair<TerrainCellInterface::TerrainCellCoverage, std::unique_ptr<Ring>>>>> getNeighboursCellsOnRadius(
        const Ring &radiusArea, const unsigned int searchDepth
    ) override;
};

#endif /* ROOT_TERRAINCELL_H_ */
