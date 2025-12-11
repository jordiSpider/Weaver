/**
 * @file LeafPatchApplicator.h
 * @brief Defines the LeafPatchApplicator class for applying patches to leaf terrain cells in a spatial tree.
 */

#ifndef LEAF_PATCH_APPLICATOR_H_
#define LEAF_PATCH_APPLICATOR_H_


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/SpatialTreePatchApplicator.h"
#include "App/Model/IBM/Landscape/Map/Geometry/Coverage.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Moisture/CellMoisture.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Obstacle/CellObstacle.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResource.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/HabitatDomain/CellHabitatDomain.h"


/**
 * @class LeafPatchApplicator
 * @brief Concrete implementation of SpatialTreePatchApplicator for leaf nodes.
 *
 * LeafPatchApplicator manages the application of patches for leaf cells
 * in the spatial tree. It handles moisture, resources, obstacles,
 * and habitat domains at the finest terrain resolution.
 * The class supports full and partial patch coverage and resource biomass management.
 */
class LeafPatchApplicator : public SpatialTreePatchApplicator
{
public:
    /// Default constructor.
    LeafPatchApplicator();

    /**
     * @brief Constructor initializing the leaf patch applicator with a terrain cell and a moisture source.
     * @param cell Pointer to the leaf terrain cell.
     * @param moistureBaseSource Pointer to the base moisture source.
     */
    LeafPatchApplicator(TerrainCell* cell, MoistureSource* const moistureBaseSource);

    /// Destructor.
    virtual ~LeafPatchApplicator();

    /// Deleted copy constructor
    LeafPatchApplicator(const LeafPatchApplicator&) = delete;

    /// Deleted copy assignment
    LeafPatchApplicator& operator=(const LeafPatchApplicator&) = delete;

    /**
     * @brief Get a const reference to the cell's moisture interface.
     * @return const reference to CellMoistureInterface
     */
    const CellMoisture& getCellMoisture() const override;

    /**
     * @brief Get a mutable reference to the cell's moisture interface.
     * @return mutable reference to CellMoistureInterface
     */
    CellMoisture& getMutableCellMoisture() override;

    /**
     * @brief Get a const reference to a resource interface by resource species ID.
     * @param resourceSpeciesId ID of the resource species
     * @return const reference to CellResourceInterface
     */
    const CellResource& getCellResource(const size_t resourceSpeciesId) const override;

    /**
     * @brief Get a mutable reference to a resource interface by resource species ID.
     * @param resourceSpeciesId ID of the resource species
     * @return mutable reference to CellResourceInterface
     */
    CellResource& getMutableCellResource(const size_t resourceSpeciesId) override;

    /**
     * @brief Get a const reference to the cell's obstacle interface.
     * @return const reference to CellObstacleInterface
     */
    const CellObstacle& getCellObstacle() const override;

    /**
     * @brief Get a mutable reference to the cell's obstacle interface.
     * @return mutable reference to CellObstacleInterface
     */
    CellObstacle& getMutableCellObstacle() override;

    /**
     * @brief Get a const reference to the cell's habitat domain interface.
     * @return const reference to CellHabitatDomainInterface
     */
    const CellHabitatDomain& getCellHabitatDomain() const override;

    /**
     * @brief Get a mutable reference to the cell's habitat domain interface.
     * @return mutable reference to CellHabitatDomainInterface
     */
    CellHabitatDomain& getMutableCellHabitatDomain() override;

    /**
     * @brief Adds a resource species to the leaf cell.
     * @param landscape Pointer to the landscape.
     * @param landscapeResources Reference to the landscape's 3D vector of resources.
     * @param resourceSpecies Resource species to add.
     * @param resourceBaseSource Base resource source.
     */
    void addResourceSpecies(Landscape* const landscape, std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource);

    /**
     * @brief Applies a patch to the leaf terrain cell.
     * @param landscape Pointer to the landscape.
     * @param patch Patch to apply.
     * @return Pair of booleans indicating whether the patch was applied and whether full coverage was achieved.
     */
    std::pair<bool, bool> applyPatch(Landscape* const landscape, const Patch &patch);

    /**
     * @brief Adds resource biomass to the cell.
     * @param biomassToAdd Biomass amount to add.
     */
    void addResourceBiomass(const WetMass& biomassToAdd);
    
    /**
     * @brief Removes a specified amount of resource biomass from the cell.
     *
     * Decreases the internal resource biomass of the cell by the given amount.
     *
     * @param biomassToRemove Amount of biomass to remove.
     */
    void removeResourceBiomass(const WetMass& biomassToRemove);

    /**
     * @brief Sets whether the total resource capacity is fully reached.
     * @param newTotalFullCapacityValue True if full capacity, false otherwise.
     */
    void setTotalFullCapacity(const bool newTotalFullCapacityValue);

    /**
     * @brief Returns the total resource biomass in the cell.
     * @return Reference to total biomass.
     */
    const WetMass& getTotalResourceBiomass() const;

    /**
     * @brief Checks if the cell has reached total full capacity.
     * @return True if total capacity is full.
     */
    bool isTotalFullCapacity() const;

    /**
     * @brief Registers resources in the landscape vector.
     * @param landscapeResources Reference to the landscape's 3D vector of resources.
     */
    void registerResources(std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources);

    /**
     * @brief Serialization function.
     * @tparam Archive Type of the archive.
     * @param ar Archive object.
     * @param version Version of serialization.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    WetMass totalResourceBiomass; ///< Total biomass of resources in the leaf cell.
    bool totalFullCapacity;        ///< Flag indicating if the cell reached total resource capacity.

    /**
     * @brief Applies a patch partially covering the leaf cell.
     * @param landscape Pointer to the landscape.
     * @param patch Patch to apply.
     * @return Pair of booleans indicating partial coverage success.
     */
    std::pair<bool, bool> applyPartialCoveragePatch(Landscape* const landscape, const Patch &patch);

    /**
     * @brief Applies a path source to the leaf cell.
     * @param landscape Pointer to the landscape.
     * @param source Source to apply.
     * @param newPatchPriority Priority of the patch.
     */
    void applySource(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority);

    /**
     * @brief Applies the downward effect of a patch on the leaf cell.
     * @param landscape Pointer to the landscape.
     * @param source Source to apply.
     * @param newPatchPriority Patch priority.
     */
    void applyDown(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority);

    /**
     * @brief Updates the cell moisture from a source.
     * @param landscape Pointer to the landscape.
     * @param source Moisture source.
     * @param newMoisturePatchPriority Patch priority.
     */
    void setCellMoisture(Landscape* const landscape, const MoistureSource &source, const size_t newMoisturePatchPriority);

    /**
     * @brief Updates the cell resource from a source.
     * @param landscape Pointer to the landscape.
     * @param source Resource source.
     * @param newResourcePatchPriority Patch priority.
     */
    void setCellResource(Landscape* const landscape, const ResourceSource &source, const size_t newResourcePatchPriority);

    /**
     * @brief Updates the cell obstacle from a source.
     * @param landscape Pointer to the landscape.
     * @param source Obstacle source.
     * @param newObstaclePatchPriority Patch priority.
     */
    void setCellObstacle(Landscape* const landscape, const ObstacleSource &source, const size_t newObstaclePatchPriority);

    /**
     * @brief Updates the cell habitat domain from a source.
     * @param landscape Pointer to the landscape.
     * @param source Habitat domain source.
     * @param newHabitatDomainPatchPriority Patch priority.
     */
    void setCellHabitatDomain(Landscape* const landscape, const HabitatDomainSource &source, const size_t newHabitatDomainPatchPriority);
};

#endif /* LEAF_PATCH_APPLICATOR_H_ */
