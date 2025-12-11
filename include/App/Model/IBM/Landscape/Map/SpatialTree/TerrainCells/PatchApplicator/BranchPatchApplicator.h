/**
 * @file BranchPatchApplicator.h
 * @brief Defines the BranchPatchApplicator class for applying patches to branch terrain cells in a spatial tree.
 */

#ifndef BRANCH_PATCH_APPLICATOR_H_
#define BRANCH_PATCH_APPLICATOR_H_


#include <boost/serialization/export.hpp>
#include <vector>


#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/SpatialTreePatchApplicator.h"
#include "App/Model/IBM/Landscape/Map/Geometry/Coverage.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Moisture/BranchCellMoisture.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Obstacle/BranchCellObstacle.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Resources/BranchCellResource.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/HabitatDomain/BranchCellHabitatDomain.h"



/**
 * @class BranchPatchApplicator
 * @brief Concrete implementation of SpatialTreePatchApplicator for branch nodes.
 *
 * BranchPatchApplicator manages the application of patches for branch cells
 * in the spatial tree. It handles moisture, resources, obstacles,
 * and habitat domains for cells that have children.
 * It provides mechanisms for applying patches both downwards (to children)
 * and upwards (from children to parent branch).
 */
class BranchPatchApplicator : public SpatialTreePatchApplicator
{
public:
    /// Default constructor.
    BranchPatchApplicator();

    /**
     * @brief Constructor initializing the branch patch applicator with a terrain cell and a moisture source.
     * @param cell Pointer to the branch terrain cell.
     * @param moistureBaseSource Pointer to the base moisture source.
     */
    BranchPatchApplicator(TerrainCell* cell, MoistureSource* const moistureBaseSource);

    /// Destructor.
    virtual ~BranchPatchApplicator();

    /// Deleted copy constructor
    BranchPatchApplicator(const BranchPatchApplicator&) = delete;

    /// Deleted copy assignment
    BranchPatchApplicator& operator=(const BranchPatchApplicator&) = delete;

    /**
     * @brief Get a const reference to the cell's moisture interface.
     * @return const reference to CellMoistureInterface
     */
    const BranchCellMoisture& getCellMoisture() const override;
    
    /**
     * @brief Get a mutable reference to the cell's moisture interface.
     * @return mutable reference to CellMoistureInterface
     */
    BranchCellMoisture& getMutableCellMoisture() override;

    /**
     * @brief Get a const reference to a resource interface by resource species ID.
     * @param resourceSpeciesId ID of the resource species
     * @return const reference to CellResourceInterface
     */
    const BranchCellResource& getCellResource(const size_t resourceSpeciesId) const override;
    
    /**
     * @brief Get a mutable reference to a resource interface by resource species ID.
     * @param resourceSpeciesId ID of the resource species
     * @return mutable reference to CellResourceInterface
     */
    BranchCellResource& getMutableCellResource(const size_t resourceSpeciesId) override;

    /**
     * @brief Get a const reference to the cell's obstacle interface.
     * @return const reference to CellObstacleInterface
     */
    const BranchCellObstacle& getCellObstacle() const override;
    
    /**
     * @brief Get a mutable reference to the cell's obstacle interface.
     * @return mutable reference to CellObstacleInterface
     */
    BranchCellObstacle& getMutableCellObstacle() override;

    /**
     * @brief Get a const reference to the cell's habitat domain interface.
     * @return const reference to CellHabitatDomainInterface
     */
    const BranchCellHabitatDomain& getCellHabitatDomain() const override;
    
    /**
     * @brief Get a mutable reference to the cell's habitat domain interface.
     * @return mutable reference to CellHabitatDomainInterface
     */
    BranchCellHabitatDomain& getMutableCellHabitatDomain() override;

    /**
     * @brief Adds a resource species to the branch cell and propagates it to children.
     * @param landscape Pointer to the landscape.
     * @param landscapeResources Reference to the landscape's 3D vector of resources.
     * @param resourceSpecies Resource species to add.
     * @param resourceBaseSource Base resource source.
     */
    void addResourceSpecies(Landscape* const landscape, std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource);

    /**
     * @brief Applies a patch to the branch cell and propagates it downwards to children.
     * @param landscape Pointer to the landscape.
     * @param patch Patch to apply.
     * @return Pair of booleans indicating whether the patch was applied and whether full coverage was achieved.
     */
    std::pair<bool, bool> applyPatch(Landscape* const landscape, const Patch &patch);

    /**
     * @brief Applies a patch upwards from children to this branch cell.
     * @param source Path source to apply.
     */
    void applyUp(const PathSource &source);

    /**
     * @brief Serialization function.
     * @tparam Archive Type of the archive.
     * @param ar Archive object.
     * @param version Version of serialization.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    /**
     * @brief Applies a patch partially covering the branch cell.
     * @param landscape Pointer to the landscape.
     * @param patch Patch to apply.
     * @return Pair of booleans indicating partial coverage success.
     */
    std::pair<bool, bool> applyPartialCoveragePatch(Landscape* const landscape, const Patch &patch);

    /**
     * @brief Applies a path source downwards to this branch cell and its children.
     * @param landscape Pointer to the landscape.
     * @param source Path source to apply.
     * @param newPatchPriority Patch priority.
     */
    void applySource(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority);

    /**
     * @brief Applies the downward effect of a patch on the branch cell.
     * @param landscape Pointer to the landscape.
     * @param source Path source to apply.
     * @param newPatchPriority Patch priority.
     */
    void applyDown(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority);

    /// @brief Sets the cell's path element using a PathSource.
    /// @param source Source defining the path properties.
    void setCellElement(const PathSource &source);

    /// @brief Sets the cell's moisture value from a MoistureSource.
    /// @param source Source defining the moisture properties.
    void setCellMoisture(const MoistureSource &source);

    /// @brief Sets the cell's moisture in a specific landscape with priority handling.
    /// @param landscape Pointer to the landscape to modify.
    /// @param source Source defining the moisture properties.
    /// @param newMoisturePatchPriority Priority of the new moisture patch.
    void setCellMoisture(Landscape* const landscape, const MoistureSource &source, const size_t newMoisturePatchPriority);

    /// @brief Sets the cell's resource value from a ResourceSource.
    /// @param source Source defining the resource properties.
    void setCellResource(const ResourceSource &source);

    /// @brief Sets the cell's resource in a specific landscape with priority handling.
    /// @param landscape Pointer to the landscape to modify.
    /// @param source Source defining the resource properties.
    /// @param newResourcePatchPriority Priority of the new resource patch.
    void setCellResource(Landscape* const landscape, const ResourceSource &source, const size_t newResourcePatchPriority);

    /// @brief Sets the cell's obstacle from an ObstacleSource.
    /// @param source Source defining the obstacle properties.
    void setCellObstacle(const ObstacleSource &source);

    /// @brief Sets the cell's obstacle in a specific landscape with priority handling.
    /// @param landscape Pointer to the landscape to modify.
    /// @param source Source defining the obstacle properties.
    /// @param newObstaclePatchPriority Priority of the new obstacle patch.
    void setCellObstacle(Landscape* const landscape, const ObstacleSource &source, const size_t newObstaclePatchPriority);

    /// @brief Sets the cell's habitat domain from a HabitatDomainSource.
    /// @param source Source defining the habitat domain properties.
    void setCellHabitatDomain(const HabitatDomainSource &source);

    /// @brief Sets the cell's habitat domain in a specific landscape with priority handling.
    /// @param landscape Pointer to the landscape to modify.
    /// @param source Source defining the habitat domain properties.
    /// @param newHabitatDomainPatchPriority Priority of the new habitat domain patch.
    void setCellHabitatDomain(Landscape* const landscape, const HabitatDomainSource &source, const size_t newHabitatDomainPatchPriority);
};


#endif /* BRANCH_PATCH_APPLICATOR_H_ */
