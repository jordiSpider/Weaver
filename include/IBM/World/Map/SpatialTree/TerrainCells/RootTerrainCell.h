#ifndef ROOT_TERRAINCELL_H_
#define ROOT_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>
#include <memory>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "IBM/World/Map/Points/PointSpatialTree.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "Exceptions/LineInfoException.h"



class RootTerrainCell : public BranchTerrainCell
{
public:
    RootTerrainCell(SpatialTreeInterface* const mapInterface);
    RootTerrainCell(PointSpatialTree* const position, SpatialTreeInterface* const mapInterface);
    virtual ~RootTerrainCell();

    const BranchTerrainCell* const getParent() const override;
    BranchTerrainCell* const getMutableParent() const override;

    void obtainWorldResourceBiomassAndAnimalsPopulation(std::vector<double> &worldResourceBiomass, CustomIndexedVector<AnimalSpecies::AnimalID, CustomIndexedVector<LifeStage, unsigned int>> &worldAnimalsPopulation);

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
        std::function<bool(AnimalInterface&)> upChecker, const AnimalSearchParams &animalSearchParams
    ) override;
    /** @} */

    /** @} */

    /** @} */

    std::unique_ptr<std::vector<std::pair<TerrainCellInterface*, std::pair<TerrainCellInterface::TerrainCellCoverage, std::unique_ptr<Ring>>>>> getNeighboursCellsOnRadius(
        const Ring &radiusArea, const unsigned int searchDepth
    ) override;


    template <class Archive>
    void serialize(Archive &ar, const unsigned int version, std::vector<ExtendedMoisture*>& appliedMoisture);
};

namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, RootTerrainCell* &rootTerrainCellPtr, const unsigned int version, SpatialTreeInterface* const newMapInterface, std::vector<ExtendedMoisture*>& appliedMoisture);
    }
}

#endif /* ROOT_TERRAINCELL_H_ */
