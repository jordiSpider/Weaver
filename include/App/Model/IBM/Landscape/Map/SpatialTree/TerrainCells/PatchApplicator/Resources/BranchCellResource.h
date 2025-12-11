/**
 * @file BranchCellResource.h
 * @brief Defines the BranchCellResource class for managing resources in branch terrain cells.
 */

#ifndef BRANCH_CELL_RESOURCE_H_
#define BRANCH_CELL_RESOURCE_H_

#include <vector>
#include <numeric>

#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResourceInterface.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Resources/Growth/BranchCellResourceGrowth.h"



class BranchTerrainCell;


/**
 * @class BranchCellResource
 * @brief Resource representation for branch nodes in a spatial tree.
 *
 * This class aggregates the resource data from child terrain cells in a branch.
 * It provides methods to update biomass, calculate available dry mass, apply branch-level updates,
 * and deserialize resource information from applied sources.
 */
class BranchCellResource : public CellResourceInterface
{
public:
    /// Default constructor.
    BranchCellResource();

    /**
     * @brief Constructor with resource species and patch priority.
     * @param summaryTerrainCell Pointer to the branch terrain cell to summarize.
     * @param newResourcePatchPriority Patch priority for resource updates.
     * @param resourceSpecies Reference to the resource species represented by this cell.
     */
    BranchCellResource(BranchTerrainCell* const summaryTerrainCell, const size_t newResourcePatchPriority, ResourceSpecies& resourceSpecies);
    
    /// Destructor.
    virtual ~BranchCellResource();

    /// Deleted copy constructor
    BranchCellResource(const BranchCellResource&) = delete;

    /// Deleted copy assignment
	BranchCellResource& operator=(const BranchCellResource&) = delete;

    /**
     * @brief Get the immutable growth building block for this resource.
     * @return Reference to the growth building block.
     */
    const BranchCellResourceGrowth& getGrowthBuildingBlock() const override;

    /**
     * @brief Get a mutable growth building block for this resource.
     * @return Reference to the growth building block.
     */
	BranchCellResourceGrowth& getMutableGrowthBuildingBlock() override;

    /// Update the resource state by aggregating child cell data.
    void update();

    /// Apply the aggregated data from child cells to the branch-level resource.
    void applyBranch();

    /**
     * @brief Set the terrain cell for this resource.
     * @param newTerrainCell Pointer to the terrain cell to set.
     */
    void setTerrainCell(TerrainCell* newTerrainCell) override;

    /**
     * @brief Deserialize resource data from applied resource sources.
     * @param appliedResource Vector of pairs containing resource patch IDs and pointers to resource sources.
     */
    void deserializeCellResource(std::vector<std::pair<size_t, ResourceSource*>>& appliedResource);

    /**
     * @brief Calculate the available dry mass for this resource.
     * @param fullCoverage If true, consider full coverage of the cell.
     * @param sourcePosition Optional source position for distance-based calculations.
     * @param radius Radius to consider for the calculation.
     * @param radiusArea Optional ring model defining the area.
     * @return Available dry mass.
     */
    DryMass calculateDryMassAvailable(const bool fullCoverage, const PointContinuous* const sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea) const;

    /**
     * @brief Subtract biomass from this resource within a given radius.
     * @param dryMassToBeSubstracted Amount of biomass to subtract.
     * @param sourcePosition Position of the source.
     * @param radius Radius over which biomass is subtracted.
     * @param competitionAmongResourceSpecies Flag indicating if competition should be considered.
     */
    void substractBiomass(const DryMass& dryMassToBeSubstracted, const PointContinuous &sourcePosition, const PreciseDouble &radius, const bool competitionAmongResourceSpecies);

    /**
     * @brief Subtract biomass with optional full coverage and radius area.
     * @param dryMassToBeSubstracted Amount of biomass to subtract.
     * @param fullCoverage Whether to consider full coverage.
     * @param sourcePosition Position of the source.
     * @param radius Radius over which biomass is subtracted.
     * @param radiusArea Optional ring model defining the area.
     * @param competitionAmongResourceSpecies Flag indicating if competition should be considered.
     */
    void substractBiomass(const DryMass& dryMassToBeSubstracted, const bool fullCoverage, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const bool competitionAmongResourceSpecies);

    /// Subtract biomass aggregated upwards in the tree.
    void substractBiomassUp(const DryMass& dryMassToBeSubstracted);

    /**
     * @brief Serialization function.
     * @tparam Archive Type of the archive.
     * @param ar Archive object.
     * @param version Version of serialization.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    /// Growth building block for this resource.
    BranchCellResourceGrowth growthBuildingBlock;

    /// Set the species growth object.
    void setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth);

private:
    /**
     * @brief Calculate the patch priority for a specific resource in the branch.
     * @param summaryTerrainCell Pointer to the branch terrain cell.
     * @param resourceSpeciesId ID of the resource species.
     * @return Patch priority.
     */
    static size_t calculateResourcePatchPriority(const BranchTerrainCell* const summaryTerrainCell, const ResourceSpecies::ResourceID& resourceSpeciesId);
};

#endif /* BRANCH_CELL_RESOURCE_H_ */
