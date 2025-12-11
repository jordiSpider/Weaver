/**
 * @file PatchApplicator.h
 * @brief Defines the PatchApplicator class responsible for applying patches (moisture, resource, obstacle, habitat) to a terrain cell.
 */

#ifndef PATCH_APPLICATOR_H_
#define PATCH_APPLICATOR_H_



#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/Map/Patches/Patch.h"


#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Moisture/CellMoistureInterface.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResourceInterface.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Obstacle/CellObstacleInterface.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/HabitatDomain/CellHabitatDomainInterface.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/ResourceSource.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/ObstacleSource.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/HabitatDomainSource.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResource.h"



class TerrainCell;
class Landscape;


/**
 * @class PatchApplicator
 * @brief Applies patches to a TerrainCell including moisture, resources, obstacles, and habitat domains.
 *
 * This class handles the application of different patch types to the terrain cell.
 * It manages the cell's interfaces for moisture, resources, obstacles, and habitat domains.
 * It also provides functions to check coverage, update sources, and manage priorities.
 */
class PatchApplicator
{
public:
    /**
     * @brief Default constructor.
     */
    PatchApplicator();

    /**
     * @brief Constructor with cell and its interfaces.
     * @param cell Pointer to the terrain cell to apply patches to
     * @param cellMoisture Pointer to the cell's moisture interface
     * @param cellObstacle Pointer to the cell's obstacle interface
     * @param cellHabitatDomain Pointer to the cell's habitat domain interface
     */
    PatchApplicator(
        TerrainCell* cell, CellMoistureInterface* cellMoisture, CellObstacleInterface* cellObstacle, 
        CellHabitatDomainInterface* cellHabitatDomain
    );

    /**
     * @brief Destructor.
     */
    virtual ~PatchApplicator();

    /// Deleted copy constructor
    PatchApplicator(const PatchApplicator&) = delete;
    
    /// Deleted copy assignment
    PatchApplicator& operator=(const PatchApplicator&) = delete;

    /**
     * @brief Get a const reference to the cell's moisture interface.
     * @return const reference to CellMoistureInterface
     */
    virtual const CellMoistureInterface& getCellMoisture() const;
    
    /**
     * @brief Get a mutable reference to the cell's moisture interface.
     * @return mutable reference to CellMoistureInterface
     */
    virtual CellMoistureInterface& getMutableCellMoisture();

    /**
     * @brief Get a const reference to a resource interface by resource species ID.
     * @param resourceSpeciesId ID of the resource species
     * @return const reference to CellResourceInterface
     */
    virtual const CellResourceInterface& getCellResource(const size_t resourceSpeciesId) const;
    
    /**
     * @brief Get a mutable reference to a resource interface by resource species ID.
     * @param resourceSpeciesId ID of the resource species
     * @return mutable reference to CellResourceInterface
     */
    virtual CellResourceInterface& getMutableCellResource(const size_t resourceSpeciesId);

    /**
     * @brief Get the number of resources present in the cell.
     * @return Number of resource types
     */
    size_t getNumberOfResources() const;

    /**
     * @brief Get a const reference to the cell's obstacle interface.
     * @return const reference to CellObstacleInterface
     */
    virtual const CellObstacleInterface& getCellObstacle() const;
    
    /**
     * @brief Get a mutable reference to the cell's obstacle interface.
     * @return mutable reference to CellObstacleInterface
     */
    virtual CellObstacleInterface& getMutableCellObstacle();

    /**
     * @brief Get a const reference to the cell's habitat domain interface.
     * @return const reference to CellHabitatDomainInterface
     */
    virtual const CellHabitatDomainInterface& getCellHabitatDomain() const;
    
    /**
     * @brief Get a mutable reference to the cell's habitat domain interface.
     * @return mutable reference to CellHabitatDomainInterface
     */
    virtual CellHabitatDomainInterface& getMutableCellHabitatDomain();

    /**
     * @brief Sets the owner terrain cell of this PatchApplicator.
     * @param cell Pointer to the TerrainCell
     */
    void setOwner(TerrainCell* cell);

    /**
     * @brief Deserialize resource and moisture species from saved data.
     * @param existingResourceSpecies Vector of existing resource species
     */
    void deserializeSpecies(std::vector<ResourceSpecies*>& existingResourceSpecies);

    /**
     * @brief Deserialize applied patches (moisture and resources) from saved data.
     * @param appliedMoisture Vector of applied moisture patches
     * @param appliedResource Vector of applied resource patches
     */
    void deserializeSources(
        std::vector<std::pair<size_t, MoistureSource*>>& appliedMoisture,
        std::vector<std::vector<std::pair<size_t, ResourceSource*>>>& appliedResource
    );

    /**
     * @brief Get the patch priority of a resource species in the cell.
     * @param resourceSpeciesId Resource species ID
     * @return Patch priority
     */
    size_t getCellResourcePatchPriority(const size_t resourceSpeciesId) const;

    /**
     * @brief Update all patches and dynamics in the cell.
     */
    void update();

    /**
     * @brief Add a resource species to the cell. Must be implemented by derived classes.
     * @param landscape Pointer to the Landscape
     * @param landscapeResources 3D vector of landscape resources
     * @param resourceSpecies Resource species to add
     * @param resourceBaseSource Base resource source
     */
    virtual void addResourceSpecies(Landscape* const landscape, std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource)=0;

    /**
     * @brief Add an animal species to the cell.
     * @param animalSpecies Animal species to add
     */
    void addAnimalSpecies(const AnimalSpecies& animalSpecies);

    /**
     * @brief Apply a patch to the cell. Must be implemented by derived classes.
     * @param landscape Pointer to the Landscape
     * @param patch Patch to apply
     * @return Pair of booleans indicating whether patch was applied and full coverage
     */
    virtual std::pair<bool, bool> applyPatch(Landscape* const landscape, const Patch &patch)=0;
    
    /**
     * @brief Serialization method.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    /// Pointer to the terrain cell owner
    TerrainCell* owner;

    /// Pointer to the cell moisture interface
    CellMoistureInterface* cellMoisture;

    /// Vector of resource interfaces in the cell
    std::vector<CellResourceInterface*> cellResources;

    /// Pointer to the cell obstacle interface
    CellObstacleInterface* cellObstacle;

    /// Pointer to the cell habitat domain interface
    CellHabitatDomainInterface* cellHabitatDomain;

    /**
     * @brief Check if a patch can be applied as full coverage in the cell.
     * @param patch Patch to check
     * @return True if full coverage is possible
     */
    bool canApplyFullCoverage(const Patch &patch) const;    

    /**
     * @brief Set a cell element to a specific source with a given patch priority.
     * @param landscape Pointer to the Landscape
     * @param source PathSource to apply
     * @param newPatchPriority Patch priority
     */
    void setCellElement(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority);

    /**
     * @brief Applies a path source to the landscape cell.
     *
     * This method updates the cell properties based on the given PathSource
     * and the specified patch priority.
     *
     * @param landscape Pointer to the Landscape containing the cell.
     * @param source The PathSource to apply.
     * @param newPatchPriority Priority of the patch to apply.
     */
    virtual void applySource(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority) = 0;

    /**
     * @brief Sets the cell moisture from a MoistureSource.
     *
     * Updates the moisture of the cell according to the given source and patch priority.
     *
     * @param landscape Pointer to the Landscape containing the cell.
     * @param source The MoistureSource to apply.
     * @param newMoisturePatchPriority Priority of the moisture patch.
     */
    virtual void setCellMoisture(Landscape* const landscape, const MoistureSource &source, const size_t newMoisturePatchPriority) = 0;

    /**
     * @brief Sets the cell resource from a ResourceSource.
     *
     * Updates the resource content of the cell according to the given source and patch priority.
     *
     * @param landscape Pointer to the Landscape containing the cell.
     * @param source The ResourceSource to apply.
     * @param newResourcePatchPriority Priority of the resource patch.
     */
    virtual void setCellResource(Landscape* const landscape, const ResourceSource &source, const size_t newResourcePatchPriority) = 0;

    /**
     * @brief Sets the cell obstacle from an ObstacleSource.
     *
     * Updates the obstacle state of the cell according to the given source and patch priority.
     *
     * @param landscape Pointer to the Landscape containing the cell.
     * @param source The ObstacleSource to apply.
     * @param newObstaclePatchPriority Priority of the obstacle patch.
     */
    virtual void setCellObstacle(Landscape* const landscape, const ObstacleSource &source, const size_t newObstaclePatchPriority) = 0;

    /**
     * @brief Sets the cell habitat domain from a HabitatDomainSource.
     *
     * Updates the habitat domain type of the cell according to the given source and patch priority.
     *
     * @param landscape Pointer to the Landscape containing the cell.
     * @param source The HabitatDomainSource to apply.
     * @param newHabitatDomainPatchPriority Priority of the habitat domain patch.
     */
    virtual void setCellHabitatDomain(Landscape* const landscape, const HabitatDomainSource &source, const size_t newHabitatDomainPatchPriority) = 0;

private:
    /**
     * @brief Checks if full coverage moisture can be applied to this cell.
     *
     * Determines whether the moisture from a patch with the given priority
     * can fully overwrite the current cell moisture.
     *
     * @param patchPriority Priority of the moisture patch.
     * @return true if the full coverage moisture can be applied, false otherwise.
     */
    bool canApplyFullCoverageMoisture(const size_t patchPriority) const;

    /**
     * @brief Checks if full coverage resource can be applied to this cell.
     *
     * Determines whether the resource from the given source with the specified
     * priority can fully overwrite the current cell resource.
     *
     * @param source Resource source to check.
     * @param patchPriority Priority of the resource patch.
     * @return true if the full coverage resource can be applied, false otherwise.
     */
    bool canApplyFullCoverageResource(const ResourceSource &source, const size_t patchPriority) const;

    /**
     * @brief Checks if full coverage obstacle can be applied to this cell.
     *
     * Determines whether the obstacle from a patch with the given priority
     * can fully overwrite the current cell obstacle.
     *
     * @param patchPriority Priority of the obstacle patch.
     * @return true if the full coverage obstacle can be applied, false otherwise.
     */
    bool canApplyFullCoverageObstacle(const size_t patchPriority) const;

    /**
     * @brief Checks if full coverage habitat domain can be applied to this cell.
     *
     * Determines whether the habitat domain from a patch with the given priority
     * can fully overwrite the current cell habitat domain.
     *
     * @param patchPriority Priority of the habitat domain patch.
     * @return true if the full coverage habitat domain can be applied, false otherwise.
     */
    bool canApplyFullCoverageHabitatDomain(const size_t patchPriority) const;
};

#endif /* PATCH_APPLICATOR_H_ */
