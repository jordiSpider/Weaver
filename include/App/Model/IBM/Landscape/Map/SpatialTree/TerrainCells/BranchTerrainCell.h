/**
 * @file BranchTerrainCell.h
 * @brief Branch node implementation of a spatial tree terrain cell.
 *
 * This class represents internal nodes of the spatial tree. Branch cells
 * contain child cells (either branch or leaf) and manage hierarchical
 * traversal, animal insertion, radius-based queries, and resource/animal
 * propagation down the tree.
 */

#ifndef BRANCH_TERRAINCELL_H_
#define BRANCH_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>


#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>


#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Resources/BranchCellResource.h"
#include "App/Model/IBM/Landscape/Map/Points/Axis.h"
#include "App/Model/IBM/Maths/OptimisedOperators.h"
#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/LeafTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/BranchPatchApplicator.h"


/**
 * @class BranchTerrainCell
 * @brief Represents an internal node (branch) of the spatial tree in the landscape.
 *
 * A BranchTerrainCell is a non-leaf node that contains child cells, which
 * can be either further branches or leaf cells. It is responsible for:
 * - Managing child cells and their positions in the spatial tree.
 * - Routing animals to the correct child for insertion or migration.
 * - Performing radius-based queries for animals and resources across children.
 * - Applying functions to animals and resources recursively (downward or upward).
 * - Deserializing moisture, resource, and species data for all children.
 * - Propagating updates for time steps to children.
 *
 * Branch cells are never leaves themselves; `isLeaf()` always returns false.
 */
class BranchTerrainCell : public SpatialTreeTerrainCell
{
protected:
    /// Vector storing pointers to all child terrain cells.
    std::vector<SpatialTreeTerrainCell*> childrenTerrainCells;

    /**
     * @brief Generate children for this branch cell.
     *
     * @param cellSizes Vector of sizes for each hierarchical division.
     * @param mapDepth Depth of the map/tree at this branch.
     * @param moistureBaseSource Pointer to the base moisture source.
     */
    void generateChildren(const std::vector<PreciseDouble>& cellSizes, const unsigned int& mapDepth, MoistureSource* const moistureBaseSource);

    /**
     * @brief Calculate the index of a child cell from a continuous position.
     *
     * @param landscape Pointer to the landscape.
     * @param childPos Continuous position to locate.
     * @return Index in the children vector corresponding to childPos.
     */
    size_t calculateChildPositionOnVector(Landscape* const landscape, const PointContinuous &childPos) const;

    /**
     * @brief Calculate the index of a child cell from a spatial tree position.
     *
     * @param childPos Spatial tree position to locate.
     * @return Index in the children vector corresponding to childPos.
     */
    size_t calculateChildPositionOnVector(const PointSpatialTree &childPos) const;

public:
    /// Default constructor.
    BranchTerrainCell();

    /**
     * @brief Constructor with cell sizes, map depth, position, and moisture source.
     *
     * @param cellSizes Vector of hierarchical cell sizes.
     * @param mapDepth Depth of the spatial tree at this branch.
     * @param position Pointer to the spatial position.
     * @param moistureBaseSource Pointer to the base moisture source.
     */
    BranchTerrainCell(const std::vector<PreciseDouble>& cellSizes, const unsigned int& mapDepth, PointSpatialTree* const position, MoistureSource* const moistureBaseSource);
    
    /**
     * @brief Constructor with parent branch, cell sizes, map depth, position, and moisture source.
     *
     * @param cellSizes Vector of hierarchical cell sizes.
     * @param mapDepth Depth of the spatial tree at this branch.
     * @param parentTerrainCell Pointer to parent branch cell.
     * @param position Pointer to spatial position.
     * @param moistureBaseSource Pointer to base moisture source.
     */
    BranchTerrainCell(const std::vector<PreciseDouble>& cellSizes, const unsigned int& mapDepth, BranchTerrainCell* const parentTerrainCell, PointSpatialTree* const position, MoistureSource* const moistureBaseSource);
    
    /// Virtual destructor.
    virtual ~BranchTerrainCell();

    /// Get a const reference to the vector of children terrain cells.
    const std::vector<SpatialTreeTerrainCell*>& getChildrenTerrainCells() const;

    /// Get a mutable reference to the vector of children terrain cells.
    std::vector<SpatialTreeTerrainCell*>& getMutableChildrenTerrainCells();

    /// Get a const pointer to a child terrain cell by index.
    const SpatialTreeTerrainCell* getChildTerrainCell(const size_t childIndex) const;

    /// Get a mutable pointer to a child terrain cell by index.
    SpatialTreeTerrainCell* getMutableChildTerrainCell(const size_t childIndex);

    /// Get a const pointer to a child terrain cell by spatial position.
    const SpatialTreeTerrainCell* getChildTerrainCell(const PointSpatialTree &cellPos) const;

    /// Get a mutable pointer to a child terrain cell by spatial position.
    SpatialTreeTerrainCell* getMutableChildTerrainCell(const PointSpatialTree &cellPos);

    /// Deserialize moisture and resource sources for all children.
    void deserializeSources(
        std::vector<std::pair<size_t, MoistureSource*>>& appliedMoisture,
        std::vector<std::vector<std::pair<size_t, ResourceSource*>>>& appliedResource
    ) override;

    /// Deserialize species data for animals and resources.
    void deserializeSpecies(std::vector<ResourceSpecies*>& existingResourceSpecies, std::vector<AnimalSpecies*>& existingAnimalSpecies) override;

    /// Add an animal species to all children according to search parameters.
    void addAnimalSpecies(const AnimalSpecies& animalSpecies, const AnimalSearchParams& allAnimalsSearchParams, const PreciseDouble& timeStepsPerDay) override;
    
    /// Add a resource species to all children.
    void addResourceSpecies(Landscape* const landscape, std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource, const AnimalSearchParams& allAnimalsSearchParams, const PreciseDouble& timeStepsPerDay);

    /// Insert an individual animal into the appropriate child.
    void insertAnimal(Landscape* const landscape, Animal* const newAnimal) override;

    /// Randomly insert an animal based on instar and species, returning insertion info.
    std::tuple<bool, TerrainCell*, TerrainCell*, Animal*, size_t> randomInsertAnimal(Landscape* const landscape, const Instar &instar, AnimalSpecies* animalSpecies, const bool isStatistical, const bool saveGenetics, const bool saveMassInfo, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay) override;

    /// Check if a given continuous position belongs to one of the children.
    bool isChild(Map* const map, const PointContinuous &childPos) const;

    /// Check if a given spatial position belongs to one of the children.
    bool isChild(const PointSpatialTree &childPos) const;

    /// Retrieve terrain cells within a radius for evaluation across children.
    void getRadiusTerrainCells(
        std::vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const size_t searchDepth, const bool searchNeighborsWithFemales, const bool parentFullCoverage, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate
    );

    /// Check if this node is a leaf (always false for branch).
    bool isLeaf() const;

    /// Register edibles and animals for all children.
    void registerEdibles(std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, std::vector<AnimalNonStatistical*>& landscapeAnimals);

    /** @name Downward traversal functions
     */
    ///@{
    /**
     * @brief Applies a set of downward functions to all animals in the cell.
     * 
     * Each function is associated with a set of animal search parameters (`AnimalSearchParams`) 
     * that determines which animals will be affected.
     * 
     * @param animalFunctions Vector of pairs, each containing search parameters and functions to apply.
     */
    void applyDownFunctionToAnimals(const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions) override;
    
    /**
     * @brief Applies downward functions to animals within a specific area of the cell.
     * 
     * Functions are applied only to animals that satisfy the `checker` predicate and are located
     * within a radius around the `sourcePosition`.
     * 
     * @param checker Function returning true if an animal should be affected.
     * @param sourcePosition Center of the influence area.
     * @param radius Radius around `sourcePosition` defining the influence area.
     * @param animalFunctions Vector of pairs with search parameters and functions to apply.
     */
    void applyDownFunctionToAnimals(
        std::function<bool(Animal&)> checker, const PointContinuous &sourcePosition, const PreciseDouble &radius,
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    ) override;

    /**
     * @brief Applies downward functions to the animals in the cell in a random order.
     * 
     * This ensures that the execution of functions on animals does not depend on a fixed iteration order,
     * preventing potential bias.
     * 
     * @param animalFunctions Vector of pairs with search parameters and functions to apply.
     */
    void randomApplyDownFunctionToAnimals(const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions) override;
    ///@}

    /** @name Update the branch and propagate updates to children
     */
    ///@{
    /**
     * @brief Updates the state of the object over a given number of time steps.
     * 
     * This method overrides the base class update and applies the update logic
     * for the current object.
     * 
     * @param numberOfTimeSteps The number of time steps to advance.
     */
    void update(const TimeStep& numberOfTimeSteps) override;
    
    /**
     * @brief Updates the state of all child objects over a given number of time steps.
     * 
     * This function iterates over all child entities and updates them according
     * to their individual update rules.
     * 
     * @param numberOfTimeSteps The number of time steps to advance for each child.
     */
    void updateChildren(const TimeStep& numberOfTimeSteps);
    ///@}

    /// Obtain all inhabitable terrain cells recursively.
    void obtainInhabitableTerrainCells(Map* const map) override;

    /// Retrieve a child cell by spatial position.
    virtual SpatialTreeTerrainCell* getCell(const PointSpatialTree &cellPos);

    /// Save resource species snapshot to file.
    void saveResourceSpeciesSnapshot(std::ofstream &file, const ResourceSpecies* const &species) const override;

    /// Save water (moisture) snapshot to file.
    void saveWaterSnapshot(std::ofstream &file) const override;

    /**
     * @brief Serialization method for persistence.
     *
     * @tparam Archive Archive type.
     * @param ar Archive object.
     * @param version Version number of the class.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* BRANCH_TERRAINCELL_H_ */
