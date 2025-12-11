/**
 * @file BranchCellResourceGrowth.h
 * @brief Defines the BranchCellResourceGrowth class for aggregating resource growth in branch terrain cells.
 */

#ifndef BRANCH_CELL_RESOURCE_GROWTH_H_
#define BRANCH_CELL_RESOURCE_GROWTH_H_

#include <vector>
#include <numeric>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/Growth/CellResourceInterfaceGrowth.h"



/**
 * @class BranchCellResourceGrowth
 * @brief Aggregates the growth information of resources for branch terrain cells.
 *
 * This class summarizes the growth of all child resources under a branch terrain cell.
 * It tracks the minimum edible biomass available and allows updating the growth state
 * at the branch level.
 */
class BranchCellResourceGrowth : public CellResourceInterfaceGrowth
{
public:
    /// Default constructor.
    BranchCellResourceGrowth();

    /**
     * @brief Constructor with owner and species growth.
     * @param owner Pointer to the owning CellResourceInterface.
     * @param newSpeciesGrowth Pointer to the resource species growth.
     */
    BranchCellResourceGrowth(CellResourceInterface* owner, ResourceSpeciesGrowth* const newSpeciesGrowth);

    /// Destructor.
    virtual ~BranchCellResourceGrowth();

    /// Aggregate growth information from child terrain cells to the branch.
    void applyBranch();

    /**
     * @brief Get the minimum edible biomass of the resource at the branch level.
     * @return Reference to the minimum edible biomass.
     */
    const WetMass& getMinimumEdibleBiomass() const;

    /// Update the growth state of the resource for the branch.
    void update();

    /**
     * @brief Serialization function for saving/loading the object.
     * @tparam Archive Type of the archive.
     * @param ar Archive object.
     * @param version Serialization version.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    /// Minimum amount of biomass available for consumption at the branch level.
    WetMass minimumEdibleBiomass;
};

#endif /* BRANCH_CELL_RESOURCE_GROWTH_H_ */
